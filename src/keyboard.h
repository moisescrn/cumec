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
 * and will be invoked by keyboard commands
*/
void ChangeBeat(TimeSignature* metre, char new_beat_char);
void IncreaseBPM(TimeSignature* metre);
void DecreaseBPM(TimeSignature* metre);
void Increase10BPM(TimeSignature* metre);
void Decrease10BPM(TimeSignature* metre);

/* **************** KEYWORD COMMANDS **************** */
/* I want that pressing one key is enough to quit, to pause or to resume
 * the metronome, but by default, pressing enter is necessary.
 * To avoid it we need to disable line buffering on the terminal
 * with help of termios
 * set_raw_mode() disables line buffering
 * and restore_terminal() resets it to the normal usage
*/
void* KeyboardCmds(void* arg);     // MetrState* state

#endif /* KEYBOARD_H */
