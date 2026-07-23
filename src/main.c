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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // multitreading to run metronome and keyboard listener at the same time

#include "metronome.h"
#include "keyboard.h"
#include "display.h"

void Start(MetrState* metSt) {
    // Assure we have quited the zeros we do not need
    // At the end of the strong array
    QuitZeros(metSt->metre->strong, metSt->metre->length, MAX_LENGTH);

    // Start the metronome toghether with keyboard and display
    pthread_t threadMetr, threadKeyboard, threadDisplay;

    pthread_create(&threadMetr, NULL, Metronome, (void*) metSt);
    pthread_create(&threadKeyboard, NULL, KeyboardCmds, (void*) metSt);
    pthread_create(&threadDisplay, NULL, ShowVariables, (void* ) metSt);

    pthread_join(threadMetr, NULL);
    pthread_join(threadKeyboard, NULL);
    pthread_join(threadDisplay, NULL);
}

int main() {
    bool pause = false;
    bool Qu = false;

    TimeSignature seguiriyas = {
        .length = 5,
        .proportions = {1.0f, 1.0f, 1.5f, 1.5f, 1.0f},
        .strong = {3,4},
        .bpm = 90
    };
    
    MetrState Seg = {
        .metre = &seguiriyas,
        .paused = &pause,
        .quit = &Qu
    };

    TimeSignature bulerias = {
        .length = 12,
        .proportions = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, 
        .strong = {3,6,8,10,0},
        .bpm = 190
    };

    MetrState Bul = {
        .metre = &bulerias,
        .paused = &pause,
        .quit = &Qu
    };

    TimeSignature sevillanas = {
        .length = 3,
        .proportions = {1.0f, 1.0f, 1.0f},
        .strong = {1},
        .bpm = 150
    };

    MetrState Sev = {
        .metre = &sevillanas,
        .paused = &pause,
        .quit = &Qu
    };

    Start(&Bul);
    return 0;
}
