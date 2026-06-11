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

#ifndef METRONOME_H
#define METRONOME_H

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <unistd.h> // for usleep()
#include <errno.h>
#include <stdio.h>

typedef struct {
    unsigned int beat;
    unsigned int bpm;
} TimeSignature;

typedef struct {
    TimeSignature* metre;
    bool* paused;
    bool* quit;
} MetrState;

// We want to allow multithreading, so this function needs this type and arguments
// then the arguments will be set inside the function
void* Metronome(void* arg);   // MetrState* state

#endif /* METRONOME_H */
