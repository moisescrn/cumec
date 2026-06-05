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

#include "src/metronome.h"

int main(void) {
    TimeSignature vals; 
    vals.beat = 4;
    vals.bpm = 140;

    PlayAudio("src/AudioFiles/sound.wav", 10000);
    StartMetronome(vals);

    printf("It has worked");
    return 0;
}
