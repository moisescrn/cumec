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

#define MAX_LENGTH 20 // maximum length of the meter

// With this struct we can make any kind of beat we can imagine,
// even irregular ones, and we can modify that irregularity
// Each entry of the array proportions gives the duration of that beat,
// 1 being the "normal" beat duration [bpm], and something above longer, and below shorter
// strong makes it possible to put more than one strong beat
typedef struct {
    unsigned int length; // length of meter
    float proportions[MAX_LENGTH];
    unsigned int strong[MAX_LENGTH];
    unsigned int bpm;
} TimeSignature;
// In this set up a meter begins with one,
// so if we want the very last pulse to be strong
// we would need to plug in 0 into strong

typedef struct {
    TimeSignature* metre;
    bool* paused;
    bool* quit;
} MetrState;

void QuitZeros(unsigned int arr[], size_t stop_point, size_t size);
// Function that quits all rubbish zeros, we do not want at the end of TimeSignature.strong

// We want to allow multithreading, so this function needs this type and arguments
void* Metronome(void* arg);   // MetrState* state

#endif /* METRONOME_H */
