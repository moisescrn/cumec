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

void draw_bar(int cy, int cx, int width, int color, bool filled) {
    /* cy, cx     center point
     * width     width of the bar (height = 3 * width), it has to be an even number
     * color      color pair
     * filled     either yes or not
     */

    int height = 3 * width;
    int start_y = cy - height/2;
    int start_x = cx - width/2;

    wattron(stdscr, COLOR_PAIR(color));

    if (filled) {
        for (int i = start_y; i <= start_y + height; i ++) {
            mvhline(i, start_x, 0, width);
        }
    }

    mvhline(start_y, start_x, 0, width);
    mvhline(start_y + height, start_x, 0, width);
    mvvline(start_y, start_x, 0, height);
    mvvline(start_y, start_x + width, 0, height);
    mvaddch(start_y, start_x, ACS_ULCORNER);
    mvaddch(start_y + height, start_x, ACS_LLCORNER);
    mvaddch(start_y, start_x + width, ACS_URCORNER);
    mvaddch(start_y + height, start_x + width, ACS_LRCORNER);
    
    wattroff(stdscr, COLOR_PAIR(color));

    /*
     * I do not know why but this did not work properly
    WINDOW *bar_win = newwin(height, width, start_y, start_x);
    wattron(bar_win, COLOR_PAIR(color));
    box(bar_win, 0, 0);
    wattroff(bar_win, COLOR_PAIR(color));
    wrefresh(bar_win);
    */
}


void get_bars_conf(MetrState* state, BarStructure* bars) {
    int len = state->metre->length;
    
    if (len % 2 == 0) {
      
    }
    else {

    }

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

    initscr();    // creates stdscr (standard screen) k
    //refresh();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, max_y, max_x);

    // Colors
    start_color();
    init_pair(TEXT_PAIR, COLOR_CYAN, COLOR_BLACK);
    init_pair(BOX_PAIR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(BAR_PAIR, COLOR_RED, COLOR_BLACK);

    // Heights for the shown variables
    int metre_height = max_y / 4 - 1;
    int beats_height = max_y / 2;
    int bpm_height = 3 * max_y / 4;

    // BASE SCREEN
    attron(COLOR_PAIR(BOX_PAIR));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(BOX_PAIR));

    // BARS
    //BarStructure bars_conf; // configuration of bars (position, size, filling)
    //get_bars_conf();

    BarStructure bars_conf = {
        .x_positions = {max_x/2 - 10, max_x/2, max_x/2 + 10},
        .width = {4,2,4},
        .filling = {true, true, false}
    };

    for (int i = 1; i <= state->metre->length; i++) {
        draw_bar(beats_height, bars_conf.x_positions[i-1], bars_conf.width[i-1], BAR_PAIR, bars_conf.filling[i-1]);
    }

    // REST: title, text
    attron(COLOR_PAIR(TEXT_PAIR));
    mvprintw(0, max_x/2 - 3, " cumec ");
    mvprintw(metre_height, max_x/2, "%u", state->metre->length);
    mvprintw(bpm_height, max_x/2 - 3, "%u bpm", state->metre->bpm);
    //getch();

    refresh();
}
