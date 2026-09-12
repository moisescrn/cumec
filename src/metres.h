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

#ifndef METRES_H
#define METRES_H

#include "metronome.h"

/* --------- List of METRES --------- */
/* Traditional metres */





/* Flamenco metres */

TimeSignature siguiriyas = {
    .length = 5,
    .proportions = {1.0f, 1.0f, 1.5f, 1.5f, 1.0f},
    .strong = {3,4},
    .bpm = 90
};

TimeSignature bulerias = {
    .length = 12,
    .proportions = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, 
    .strong = {3,6,8,10,0},
    .bpm = 190
};

TimeSignature sevillanas = {
    .length = 3,
    .proportions = {1.0f, 1.0f, 1.0f},
    .strong = {1},
    .bpm = 150
};

TimeSignature tangos = {
    .length = 4,
    .proportions = {1.0f, 1.0f, 1.0f, 1.0f},
    .strong = {1},
    .bpm = 150
};

#endif /* METRES_H */
