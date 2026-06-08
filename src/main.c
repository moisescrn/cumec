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

#include <stdio.h>
#include <pthread.h> // multitreading to run metronome and keyboard listener at the same time

#include "metronome.h"

int main(void) {
    TimeSignature vals; 
    vals.beat = 0;
    vals.bpm = 140;

    bool pause = false;
    bool Qu = false;

    MetrState sttt;
    sttt.metre = &vals;
    sttt.paused = &pause;
    sttt.quit = &Qu;

    pthread_t threadMetr, threadKeyboard;

    pthread_create(&threadMetr, NULL, Metronome, &sttt);
    pthread_create(&threadKeyboard, NULL, KeyboardCmds, &sttt);

    // Let both threads run independently
    pthread_join(threadMetr, NULL);
    pthread_join(threadKeyboard, NULL);
    
    return 0;
}
