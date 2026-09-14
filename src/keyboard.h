/* cumec is free software: you can redistribute it and/or modify
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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include <termios.h>

#include "metronome.h"

/* **************** MODIFYING FUNCTIONS **************** */
/* These functions modify the metre of the metronome
 * and will be invoked by keyboard commands in display.c
*/
void ChangeBeat(TimeSignature* metre, char new_beat_char);
void IncreaseBPM(TimeSignature* metre);
void DecreaseBPM(TimeSignature* metre);
void Increase10BPM(TimeSignature* metre);
void Decrease10BPM(TimeSignature* metre);


#endif /* KEYBOARD_H */
