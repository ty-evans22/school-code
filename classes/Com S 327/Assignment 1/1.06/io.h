#ifndef IO_H
#define IO_H

#include "world.h"

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(pair_t dest);
void io_queue_message(const char *format, ...);
void io_battle(trainer_t *aggressor, trainer_t *defender);

#endif