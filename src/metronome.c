/* cumec - A lightweight overlay volume/anything bar for the X Window System.
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

#include "metronome.h"
#include <errno.h>

void PlayAudio(char* filename, unsigned int delay) {
    SDL_Init(SDL_INIT_AUDIO);

    // Variables of the audio file
    SDL_AudioSpec wavSpecifier;
    Uint8* wavBuffer;
    Uint32 wavLength;

    // SDL_LoadWAV sets the Buffer-pointer to the audio file, and determines
    // the length of the file, adjusting this parameters correctly
    if (!SDL_LoadWAV(filename, &wavSpecifier, &wavBuffer, &wavLength)) {
        printf("Error loading the wav-file %s. Value of errno: %d\n", filename, errno);
        return 1;
    }

    // SDL3 works with streams
    // direct this stream into the default audio device
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK.
                                                        &wavSpecifier,
                                                        NULL,
                                                        NULL);

    SDL_PutAudioStreamData(stream, wavBuffer, wavLength); // load audio to stream

    SDL_ResumAudioStreamDevice(stream);

    SDL_Delay(4); // time of playing the audio (in ms)

    SDL_free(wavBuffer);
    SDL_free(stream);
    SDL_Quit();
}


void StartMetronome(TimeSignature metre) {
}
