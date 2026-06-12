/* cumec - A lightweight metronome that can be fully customizable.
 * Copyright (C) 2026 Moisés CRN
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

#include "metronome.h"

void QuitZeros(unsigned int arr[], size_t stop_point, size_t size) {
    // Function that quits all rubbish zeros, we do not want at the end of TimeSignature.strong
    // This function shall be used, when defining the meters
    for (int i = size; i > stop_point; i--){
        if (arr[i] == 0)
            arr[i] = 100;
    }
}

// Check if a value is inside an array
bool contains(unsigned int arr[], int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return true;
        }
    }
    return false;
}

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
    // direct the stream into the default audio device
    // we only need one stream, since strong and weak beats are never simultaneous
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                                        &wavSpec1,
                                                        NULL,
                                                        NULL);

    /* --------- Metronome loop --------- */
playing:
    counter = 0;

    // Let's interpreat a beat of 0, as if it had no strong beats
    if (state->metre->length == 0) {
        while ( !(*(state->paused)) ) { // stops when we change the variable paused to true
            time_pulses = (useconds_t) 60000000 / (state->metre->bpm);
            time_pulses = time_pulses - 100000;

            usleep(time_pulses);
            SDL_PutAudioStreamData(stream,wavBuffer2, wavLength2); // load audio to stream
            SDL_ResumeAudioStreamDevice(stream);
            SDL_Delay(100);
        }
    }

    while ( !(*(state->paused)) ) { // stops when we change the paused or quit to true
        time_pulses = (useconds_t) 60000000 / (state->metre->bpm);
//        time_pulses = time_pulses - 100000;

        usleep(time_pulses * (state->metre->proportions)[(counter-1)%(state->metre->length)] - 100000);   // check the if duration should be longer
        if (state->metre->length == 0) // we could set the beat to 0, while being inside the loop
            goto playing;
        if ( contains(state->metre->strong, state->metre->length-1, (counter+1)%(state->metre->length)) ) {    // strong beat (check if it is inside the array of strong beats)
            SDL_PutAudioStreamData(stream, wavBuffer1, wavLength1);
        }
        else {                                          // weak beat
            SDL_PutAudioStreamData(stream, wavBuffer2, wavLength2);
        }
        SDL_ResumeAudioStreamDevice(stream);
        SDL_Delay(100);
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
