#ifndef IO_H
#define IO_H

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_display_check(bool player);
void io_display_checkmate(bool player);
void io_erase_check();
void io_handle_input();

#endif