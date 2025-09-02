#ifndef STATE_H
#define STATE_H
#include "util_types.h"

extern Mode mode;
extern Colour active_colour;

Mode next_mode();
Colour toggle_active_colour();
void print_state();

#endif // STATE_H
