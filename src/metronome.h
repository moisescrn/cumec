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

#ifndef MUSIC_H
#define MUSIC_H

#include <SDL3/SDL_audio.h>
#include <SDL/SDL_timer.h>

typedef struct {
    unsigned int beat;
    unsigned int bpm;
} TimeSignature;


void PlayAudio(char* filename, unsigned int delay);

void StartMetronome(TimeSignature metre);

void PauseMetronome();

#endif /* MUSIC_H */
