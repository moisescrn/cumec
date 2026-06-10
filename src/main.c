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
#include <pthread.h> // multitreading to run metronome and keyboard listener at the same time

#include "metronome.h"
#include "display.h"

int main() {
    TimeSignature vals; 
    vals.beat = 5;
    vals.bpm = 140;

    bool pause = false;
    bool Qu = false;

    MetrState sttt;
    sttt.metre = &vals;
    sttt.paused = &pause;
    sttt.quit = &Qu;

    pthread_t threadMetr, threadKeyboard, threadDisplay;

    pthread_create(&threadMetr, NULL, Metronome, &sttt);
    pthread_create(&threadKeyboard, NULL, KeyboardCmds, &sttt);
    pthread_create(&threadDisplay, NULL, ShowVariables, &sttt);

    // Let both threads run independently
    pthread_join(threadMetr, NULL);
    pthread_join(threadKeyboard, NULL);
    pthread_join(threadDisplay, NULL);
    
    return 0;
}
