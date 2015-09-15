#include "io.h"

#include <termios.h>
#include <stdio.h>

/*
 * Heavily based on: http://stackoverflow.com/a/7469410
 */

static struct termios old, new;

void init_io(int echo) {
    tcgetattr(0, &old); /* grab old terminal io settings */
    new = old; /* make new settings same as old settings */
    new.c_lflag &= ~ICANON; /* disabled buffered io */
    new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new); /* use the new settings. */
}

void denit_io(void) {
    tcsetattr(0, TCSANOW, &old);
}

int _getch(int echo) {
    int i;
    init_io(echo);
    i = getchar();
    denit_io();
    return i;
}

int getch(void) {
    return _getch(0);
}

int getche(void) {
    return _getch(1);
}
