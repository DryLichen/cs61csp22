#ifndef _SNK_STATE_H
#define _SNK_STATE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct snake_t {
  unsigned int tail_x;
  unsigned int tail_y;
  unsigned int head_x;
  unsigned int head_y;

  bool live;
} snake_t;

typedef struct game_state_t {
  unsigned int x_size;
  unsigned int y_size;

  char **board;

  unsigned int num_snakes;
  snake_t* snakes;
} game_state_t;

game_state_t* create_default_state();
void free_state(game_state_t* state);
void print_board(game_state_t* state, FILE* fp);
void save_board(game_state_t* state, char* filename);
void update_state(game_state_t* state, int (*add_food)(game_state_t* state));
game_state_t * initialize_snakes(game_state_t* state);
game_state_t* load_board(char* filename);

#endif
