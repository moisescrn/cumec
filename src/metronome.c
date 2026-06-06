/* cumec - A lightweight metronome that can be fully customizable.
 * Copyright (C) 2021 Florent Ch.
 *
 * cumec is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cumec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cumec.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <unistd.h> // for usleep()
#include <errno.h>
#include <stdio.h>
#include <termios.h>

#include "metronome.h"

void PlayAudio(char* filename, Uint32 delay) {
    SDL_Init(SDL_INIT_AUDIO);

    // Variables of the audio file
    SDL_AudioSpec wavSpecifier;
    Uint8* wavBuffer;
    Uint32 wavLength;

    // SDL_LoadWAV sets the Buffer-pointer to the audio file, and determines
    // the length of the file, adjusting this parameters correctly
    if (!SDL_LoadWAV(filename, &wavSpecifier, &wavBuffer, &wavLength)) {
        printf("Error loading the wav-file %s. Value of errno: %d\n", filename, errno);
        return;
    }

    // SDL3 works with streams
    // direct this stream into the default audio device
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                                        &wavSpecifier,
                                                        NULL,
                                                        NULL);

    SDL_PutAudioStreamData(stream, wavBuffer, wavLength); // load audio to stream

    SDL_ResumeAudioStreamDevice(stream);

    SDL_Delay(delay); // time of playing the audio (in ms)

    SDL_free(wavBuffer); // do not free stream without finishing the thread!!
    SDL_Quit();
}

void* StartMetronome(void* arg) {
    MetrState* state = (MetrState*) arg;

    useconds_t time_pulses = (useconds_t) 60000000 / (state->metre->bpm); // time (in microseconds) between two pulses
    time_pulses = time_pulses - 300000; // quit 300 miliseconds
    Uint8 counter = 0;

    while (!(*(state->paused))) {
        usleep(time_pulses);
        if (counter % (state->metre->beat) == 0) {
            PlayAudio("src/AudioFiles/fuerte.wav", 300);
            printf("Strong beat done\n");
        }
        else {
            PlayAudio("src/AudioFiles/flojo.wav", 300);
            printf("Beat done\n");
        }
        counter++;
    }
    printf("Metronome closed ");
    return NULL;
}

/* **************** KEYWORD COMMANDS **************** */
/* I want that pressing one key is enough to pause, quit, resume,
 * the metronome, but by default, pressing enter is necessary.
 * To avoid it we need to disable line buffering on the terminal
 * with help of termios
 * set_raw_mode() disables line buffering
 * and restore_terminal() resets it to the normal usage
*/

void set_raw_mode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);    // get current settings
    t.c_lflag &= ~(ICANON | ECHO);  // disable line buffering and acho
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void restore_terminal() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void* KeyboardCmds(void* arg) {
    bool* paused = (bool*) arg;

    set_raw_mode();
    int c;
    while (true) {
        c = getchar();
        switch (c) {
            case 'q': printf("QUITTING\n"); break;
            case 'p': *paused = !(*paused); break;
            case 'r': printf("RESUMING\n"); break;
            default: printf("uknown command\n"); break;
        }
    }
    restore_terminal();
    return NULL;
}
