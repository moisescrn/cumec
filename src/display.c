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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"

// ANSI escape codes
#define CURSOR_SAVE "\033[s"
#define CURSOR_RESTORE "\033[u"
#define CLEAR_LINE "\033[2K"
#define CURSOR_HIDE "\033[?25l"
#define CURSOR_SHOW "\033[?25h"

void* ShowVariables(void* arg) {
    MetrState* state = (MetrState*) arg;
    /* --- Comparing variables ---*/
    // Update output only if any variable has changed
    TimeSignature previous_metre;
    memcpy(&previous_metre,state->metre,sizeof(TimeSignature)); // copy initial state
    bool previous_paused = false;
    bool previous_quit = false;

    printf(CURSOR_HIDE);
    printf("============ CUMEC ============\n");
    printf(CURSOR_SAVE);
    printf(CURSOR_RESTORE);
    printf(CLEAR_LINE "Beat: %u\n", state->metre->length);
    printf(CLEAR_LINE "Structure: \n");
    for (int i = 0; i < state->metre->length; i++) {
        printf(CLEAR_LINE "%.2f\n", (state->metre->proportions)[i]);
    }
    printf("\n");
    printf(CLEAR_LINE "BPM: %u\n", state->metre->bpm);
    printf(CLEAR_LINE "Paused: %d\n", *(state->paused));
    printf(CLEAR_LINE "Quit: %d\n", *(state->quit));

    while ( !(*(state->quit)) ) {
        // Check if any variable has changed
        if ( memcmp(state->metre, &previous_metre, sizeof(TimeSignature)) != 0 ||
             memcmp(state->paused, &previous_paused, sizeof(bool)) != 0 ||
             memcmp(state->quit, &previous_quit, sizeof(bool)) != 0 ) {

            printf(CURSOR_RESTORE);
            printf(CLEAR_LINE "Beat: %u\n", state->metre->length);
            printf(CLEAR_LINE "Structure: \n");
            for (int i = 0; i < state->metre->length; i++) {
                printf(CLEAR_LINE "%.2f\n", (state->metre->proportions)[i]);
            }
            printf("\n");
            printf(CLEAR_LINE "BPM: %u\n", state->metre->bpm);
            printf(CLEAR_LINE "Paused: %d\n", *(state->paused));
            printf(CLEAR_LINE "Quit: %d\n", *(state->quit));
            fflush(stdout);

            // Update comparison variables
            previous_metre = *(state->metre);
            previous_paused = *(state->paused);
            previous_quit = *(state->quit);
        }
    }
    printf(CURSOR_SHOW);
    //printf("\n");
}

void* ShowPanel(void* arg) {
    MetrState* state = (MetrState*) arg;
    int max_y, max_x;

    initscr();    // creates stdscr (standard screen) 
    cbreak();
    noecho();
    curs_set(0);
    getmaxyx(stdscr, max_y, max_x);

    // Heights for the shown variables
    int metre_height = max_y / 4;
    int beats_height = max_y / 2;
    int bpm_height = 3 * max_y / 4;

    attron(COLOR_PAIR(1));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(1));


    attron(A_BOLD);
    mvprintw(0, max_x / 2, " cumec ");
    mvprintw(metre_height, max_x / 2, "%u", state->metre->length);
    mvprintw(beats_height, max_x / 2, "Circles");
    mvprintw(bpm_height, max_x / 2, "%u", state->metre->bpm);
    attroff(A_BOLD);

    wattron(stdscr, COLOR_PAIR(2));
    box(stdscr, 0, 0);
    wattroff(stdscr, COLOR_PAIR(2));

    wrefresh(stdscr);
}
