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

void get_bars_conf(int max_x, TimeSignature* metre, BarStructure* bars) {
    int len = metre->length;
    int distance = 8; // distance between bars 

    // x_positions
    int first_bar_x = (max_x - (len-1)*distance ) / 2; // Position of the left most bar
    for (int i = 0; i < len; i++) {
        bars->x_positions[i] = first_bar_x + i * distance;
    }

    // width
    // standard width is 2, if the length is longer we multiply by that
    for (int i = 0; i < len; i++) {
        bars->width[i] = metre->proportions[i] * 2;
    }

    // filling
    // if the beat is strong it is filled
    for (int i = 0; i < len; i++) {
        bars->filling[i] = contains(metre->strong, len-1, i+1); // use contains() from metronome.c
    }
}

void ShowPanel(MetrState* state) {
    int max_y, max_x;

    initscr();    // creates stdscr (standard screen)
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
    BarStructure bars_conf; // configuration of bars (position, size, filling)

    get_bars_conf(max_x, state->metre, &bars_conf);

    for (int i = 1; i <= state->metre->length; i++) {
        draw_bar(beats_height, bars_conf.x_positions[i-1], bars_conf.width[i-1], BAR_PAIR, bars_conf.filling[i-1]);
    }

    // REST: title, text
    attron(COLOR_PAIR(TEXT_PAIR));
    mvprintw(0, max_x/2 - 3, " cumec ");
    mvprintw(metre_height, max_x/2, "%u", state->metre->length);
    mvprintw(bpm_height, max_x/2 - 3, "%u bpm", state->metre->bpm);

    refresh();
}

void* RedrawWindow(void* arg) {
    // This function reads input and refreshes the window after each change
    MetrState* state = (MetrState*) arg;
    bool show_commands_window = false;

    int c; // Character that stores the command
    c = getch();
    while ( true ) {
        if ( strchr("0123456789",c) ) {
            ChangeBeat(state->metre,c);
        }

        else {
            switch (c) {
                case 'q': 
                          *(state->quit) = true;            // quit metronome
                          *(state->paused) = true;          // pause to exit the loop
                          endwin(); return NULL; break;
                case ' ': *(state->paused) = !(*(state->paused)); break; // pause and resume 
                // Modifiying functions
                case '+': IncreaseBPM(state->metre); break;
                case '-': DecreaseBPM(state->metre); break;
                case '.': Increase10BPM(state->metre); break;
                case ',': Decrease10BPM(state->metre); break;
                case 'd': DoubleBPM(state->metre); break;
                case 'h': HalveBPM(state->metre); break;
                case 'c': WINDOW* comm_win = draw_commands_window(); wgetch(comm_win); close_window(comm_win); break;
            }
        }


        ShowPanel( (void*) state );  

        c = getch();

        erase();
        refresh();
    }
}

WINDOW* draw_commands_window() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int window_height = 10;       // number of aviable lines to write
    int window_width = 45;
    int start_y = (max_y - window_height) / 2 + 1;
    int start_x = (max_x - window_width) / 2 + 1;

    WINDOW* commands_wind = newwin(window_height, window_width, start_y, start_x);
    werase(commands_wind);

    // surrounding box
    wattron(commands_wind, COLOR_PAIR(BOX_PAIR));
    box(commands_wind, 0, 0);
    wattroff(commands_wind, COLOR_PAIR(BOX_PAIR));

    // list of commands
    wattron(commands_wind, COLOR_PAIR(TEXT_PAIR));
    mvwprintw(commands_wind, 2, 2, "c       |  show this command's window");
    mvwprintw(commands_wind, 3, 2, "SPACE   |  toggle pause and resume");
    mvwprintw(commands_wind, 4, 2, "+/-     |  increase/decrease by 1 bpm");
    mvwprintw(commands_wind, 5, 2, "./,     |  increase/decrease by 10 bpm");
    mvwprintw(commands_wind, 6, 2, "NUMBER  |  change to beat of that number");
    wattroff(commands_wind, COLOR_PAIR(BOX_PAIR));

    wrefresh(commands_wind);

    return commands_wind;

}

void close_window(WINDOW* win) {
    delwin(win);
    touchwin(stdscr);
    refresh();
}
