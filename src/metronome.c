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

void* StartMetronome(void* arg) {
    /* --------- Initialize metronome variables --------- */
    MetrState* state = (MetrState*) arg;
    useconds_t time_pulses = (useconds_t) 60000000 / (state->metre->bpm); // time (in microseconds) between two pulses
    time_pulses = time_pulses - 100000; // quit 300 miliseconds
    Uint8 counter = 0;

    /* --------- Define audio SDL variables --------- */
    SDL_Init(SDL_INIT_AUDIO);

    // Variables of both audio files
    SDL_AudioSpec wavSpec1;
    Uint8* wavBuffer1;
    Uint32 wavLength1;
    SDL_AudioSpec wavSpec2;
    Uint8* wavBuffer2;
    Uint32 wavLength2;

    // SDL_LoadWAV sets the Buffer-pointer to the audio file, and determines
    // the length of the file, adjusting this parameters correctly
    if (!SDL_LoadWAV("src/AudioFiles/fuerte.wav", &wavSpec1, &wavBuffer1, &wavLength1)) {
        printf("Error loading the wav-file '%s'. Value of errno: %d\n", "AudioFiles/fuerte.wav", errno);
        return NULL;
    }
    if (!SDL_LoadWAV("src/AudioFiles/flojo.wav", &wavSpec2, &wavBuffer2, &wavLength2)) {
        printf("Error loading the wav-file '%s'. Value of errno: %d\n", "AudioFiles/flojo.wav", errno);
        return NULL;
    }

    // SDL3 works with streams
    // direct the streamm into the default audio device
    // we only need one stream, since strong and weak beats are never simultaneous
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                                        &wavSpec1,
                                                        NULL,
                                                        NULL);

    /* --------- Metronome loop --------- */
    // Let's interpreat a beat of 0, as if it had no strong beats
    if (state->metre->beat == 0) { // repeat weak beat
        while (!(*(state->paused))) {
            usleep(time_pulses);
            SDL_PutAudioStreamData(stream,wavBuffer2, wavLength2); // load audio to stream
//            printf("Beat done\n");
            SDL_ResumeAudioStreamDevice(stream);
            SDL_Delay(110);
        }
    }
    while (!(*(state->paused))) {
        usleep(time_pulses);
        if (counter % (state->metre->beat) == 0) {
            SDL_PutAudioStreamData(stream, wavBuffer1, wavLength1); // load audio to stream
//            printf("Strong beat done\n");
        }
        else {
            SDL_PutAudioStreamData(stream, wavBuffer2, wavLength2); // load audio to stream
//            printf("Beat done\n");
        }
        SDL_ResumeAudioStreamDevice(stream);
        SDL_Delay(110);
        counter++;
    }
    SDL_free(wavBuffer1);
    SDL_free(wavBuffer2);
    SDL_Quit();
//    printf("Metronome closed ");
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
            case 'q': printf("METRONOME QUITTED!\n"); *paused = !(*paused); return NULL; break;
            case 'p': *paused = !(*paused); break;
            case 'r': printf("RESUMING\n"); break;
            default: printf("uknown command\n"); break;
        }
    }
    restore_terminal();
    return NULL;
}
