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

#include "keyboard.h"

/* **************** MODIFYING FUNCTIONS **************** */
void ChangeBeat(TimeSignature* metre, char new_length_char) {
    unsigned int new_length = new_length_char - '0';
    metre->length = new_length;
    // set normal proportions       [[[ TO BE CHANGED!!!! ]]]
    for (int i = 0; i < new_length; i++) {
        (metre->proportions)[i] = 1;
    }
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
