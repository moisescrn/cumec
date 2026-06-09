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

void* Metronome(void* arg) {
    /* --------- Initialize metronome variables --------- */
    MetrState* state = (MetrState*) arg;
    useconds_t time_pulses = (useconds_t) 60000000 / (state->metre->bpm); // time (in microseconds) between two pulses
    time_pulses = time_pulses - 100000; // quit 100 miliseconds
    Uint8 counter = 0;

    /* --------- Define audio SDL3 variables --------- */
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
playing:                                // label for goto statement
    time_pulses = (useconds_t) 60000000 / (state->metre->bpm);
    time_pulses = time_pulses - 100000; // quit 100 miliseconds
    counter = 0;

    // Let's interpreat a beat of 0, as if it had no strong beats
    if (state->metre->beat == 0) {      // repeat weak beat
        while ( !(*(state->paused)) ) { // stops when we change the variable paused to true
            usleep(time_pulses);
            SDL_PutAudioStreamData(stream,wavBuffer2, wavLength2); // load audio to stream
            SDL_ResumeAudioStreamDevice(stream);
            SDL_Delay(110);
        }
    }

    while ( !(*(state->paused)) ) { // stops when we change the paused or quit to true
        usleep(time_pulses);
        if (state->metre->beat == 0) // we could set the beat to 0, while being inside the loop
            goto playing;
        if (counter % (state->metre->beat) == 0) {    // strong beat
            SDL_PutAudioStreamData(stream, wavBuffer1, wavLength1);
        }
        else {                                        // weak beat
            SDL_PutAudioStreamData(stream, wavBuffer2, wavLength2);
        }
        SDL_ResumeAudioStreamDevice(stream);
        SDL_Delay(110);                               // beat duration
        counter++;
    }
    
    /* --------- Quit metronome --------- */
    // If metronome is to be quitted then quit is set to true
quit:
    if (*(state->quit)) {
        SDL_free(wavBuffer1);
        SDL_free(wavBuffer2);
        SDL_Quit();
        return NULL;
    }

    /* --------- Resume metronome --------- */
    // If metronome has been paused, it will enter this loop
    // waiting for being resumed
    while (true) {
        if ( !(*(state->paused)) )     // resume
            goto playing;
        if ( (*(state->quit)) )        // quit while being in pause mode
            goto quit;
    }
}

/* **************** MODIFYING FUNCTIONS **************** */
/* These functions modify the metre of the metronome
 * and will be invoked by keyboard commands
*/

void ChangeBeat(TimeSignature* metre, char new_beat_char) {
    unsigned int new_beat = new_beat_char - '0';
    metre->beat = new_beat;
}

void IncreaseBPM(TimeSignature* metre) {
    (metre->bpm)++;
}

void DecreaseBPM(TimeSignature* metre) {
    if ( (metre->bpm) == 1 )
        return;
    (metre->bpm)--;
}

void Increase10BPM(TimeSignature* metre) {
    metre->bpm += 10;
}

void Decrease10BPM(TimeSignature* metre) {
    if ( (metre->bpm) == 1 )
        return;
    if ( (metre->bpm) < 10 ) {
        return;
    }
    metre->bpm -= 10;
}

/* **************** KEYWORD COMMANDS **************** */
/* I want that pressing one key is enough to quit, to pause or to resume
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
    MetrState* state = (MetrState*) arg;

    set_raw_mode();
    int c;
    while (true) {
        c = getchar();
        printf("Beat: %u\n", state->metre->beat);
        printf("BPM: %u\n", state->metre->bpm);
        printf("Paused: %d\n", *(state->paused));
        printf("Quit: %d\n", *(state->quit));
        fflush(stdout);

        printf("INPUT COMMANDS: ");

        printf("%c",c);
        if ( strchr("0123456789",c) ) {       // change beat
            ChangeBeat(state->metre,c);
        }
        else {
            switch (c) {
                case 'q': printf("METRONOME QUITTED!\n");
                          *(state->quit) = true;            // quit metronome
                          *(state->paused) = true;                       // pause to exist the loop
                          return NULL; break;
                case 'p': *(state->paused) = !(*(state->paused)); break; // pause and resume
                // Modifiying functions
                case '+': IncreaseBPM(state->metre); break;
                case '-': DecreaseBPM(state->metre); break;
                case '.': Increase10BPM(state->metre); break;
                case ',': Decrease10BPM(state->metre); break;
                default: printf("uknown command\n"); break;
            }
        }
    }
    restore_terminal();
    return NULL;
}
