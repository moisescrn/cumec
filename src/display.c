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

// ncurses colors indeces
#define TEXT_PAIR 1 
#define BOX_PAIR 2
#define BAR_PAIR 3
//#define 

void draw_bar(WINDOW* win, int cy, int cx, int width, int color, bool filled) {
    /* win        window
     * cy, cx     center point
     * width     width of the bar (height = 3 * width), it has to be an even number
     * color      color pair
     * filled     either yes or not
     */

    int height = 3 * width;
    // Characters are roughly twice as tall as they are wide.
    //const double aspect = 2.0;

    wattron(win, COLOR_PAIR(color));

    if (filled) {
        for (int y = cy - height/2; y <= cy + height/2; y++) {
           for (int x = cx - width/2; x <= cx + width/2; x++) {

               mvwaddch(win, y, x, 'Q');
           }
        }
    }

    // unfilled 
    else {
        // top and bottom
        for (int x = cx - width/2; x <= cx + width/2; x++) {
            mvwaddch(win, cy - height/2, x, '-');
            mvwaddch(win, cy + height/2, x, '-');
        }

        // left and right
        for (int y = cy - height/2; y <= cy + height/2; y++) {
            mvwaddch(win, y, cx - width/2, '|');
            mvwaddch(win, y, cx + width/2, '|');
        }
    }

    wattroff(win, COLOR_PAIR(color));
}

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

    // Colors
    start_color();

    init_pair(TEXT_PAIR, COLOR_CYAN, COLOR_BLACK);
    init_pair(BOX_PAIR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(BAR_PAIR, COLOR_RED, COLOR_BLACK);
    //init_pair(4, COLOR_BLUE, COLOR_BLACK);

    // Heights for the shown variables
    int metre_height = max_y / 4;
    int beats_height = max_y / 2;
    int bpm_height = 3 * max_y / 4;

    attron(COLOR_PAIR(BOX_PAIR));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(BOX_PAIR));


    attron(COLOR_PAIR(TEXT_PAIR));
    mvprintw(0, max_x / 2, " cumec ");
    mvprintw(metre_height, max_x / 2, "%u", state->metre->length);
    //mvprintw(beats_height, max_x / 2, "Circles");
    draw_bar(stdscr, beats_height, max_x/2, 2, BAR_PAIR, true); 
    mvprintw(bpm_height, max_x / 2, "%u", state->metre->bpm);
    attroff(COLOR_PAIR(TEXT_PAIR));



    refresh();
}
