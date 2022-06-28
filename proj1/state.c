#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snake_utils.h"
#include "state.h"

/* Helper function definitions */
static char get_board_at(game_state_t* state, int x, int y);
static void set_board_at(game_state_t* state, int x, int y, char ch);
static bool is_tail(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static int incr_x(char c);
static int incr_y(char c);
static void find_head(game_state_t* state, int snum);
static char next_square(game_state_t* state, int snum);
static void update_tail(game_state_t* state, int snum);
static void update_head(game_state_t* state, int snum);

/* Helper function to get a character from the board (already implemented for you). */
static char get_board_at(game_state_t* state, int x, int y) {
  return state->board[y][x];
}

/* Helper function to set a character on the board (already implemented for you). */
static void set_board_at(game_state_t* state, int x, int y, char ch) {
  state->board[y][x] = ch;
}

/* Task 1
 * Create the default game state 
 */
game_state_t* create_default_state() {
  game_state_t *gsp = (game_state_t *)malloc(sizeof(game_state_t));

  gsp->x_size = 14;
  gsp->y_size = 10;

  // create snakes
  gsp->num_snakes = 1;
  snake_t *sp = (snake_t *)malloc(sizeof(snake_t) * gsp->num_snakes);
  sp->tail_x = 4;
  sp->tail_y = 4;
  sp->head_x = 5; 
  sp->head_y = 4;
  sp->live = true;

  gsp->snakes = sp;

  // create game board
  char **bp = NULL;
  bp = (char **)malloc(sizeof(char *) * gsp->y_size);

  for (size_t i = 0; i < gsp->y_size; i++) {
    bp[i] = (char *)malloc(sizeof(char) * (gsp->x_size + 1));
    if (i == 0 || i == 9) {
      strcpy(bp[i], "##############");
      continue;
    }
    if (i == 2) {
      strcpy(bp[i], "#        *   #");
      continue;
    }
    if (i == 4) {
      strcpy(bp[i], "#   d>       #");
      continue;
    }
    strcpy(bp[i], "#            #");
  }

  gsp->board = bp;

  return gsp; 
}

/* Task 2 
 * free the space of current game state and null the pointers
 */
void free_state(game_state_t* state) {
  // free the snakes
  free(state->snakes);
  state->snakes = NULL;

  // free the game board
  for (size_t i = 0; i < state->y_size; i++) {
    free(state->board[i]);
    state->board[i] = NULL;
  }
  free(state->board);
  state->board = NULL;

  free(state);
  state = NULL;

  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  char **bp = state->board;
  for (size_t i = 0; i < state->y_size; i++) {
    fprintf(fp, "%s\n", bp[i]);
  }
  
  return;
}

/* Saves the current state into filename (already implemented for you). */
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 
 * Helper methods for update_state */
static bool is_tail(char c) {
  char tail[] = "wasd";
  for (size_t i = 0; i < strlen(tail); i++) {
    if (c == tail[i]) {
      return true;
    }
  }
  
  return false;
}

static bool is_snake(char c) {
  char snake[] = "wasd^<>vx";
  for (size_t i = 0; i < strlen(snake); i++) {
    if (c == snake[i]) {
      return true;
    }
  }
  
  return false;
}

static char body_to_tail(char c) {
  switch (c) {
    case '<':
      return 'a';
      break;
    case '>':
      return 'd';
      break;
    case '^':
      return 'w';
      break;
    case 'v':
      return 's';
      break;
    default:
      return '?';
      break;
  }
  
}

static int incr_x(char c) {
  if (c == '>' || c == 'd') {
    return 1;
  } else if (c == '<' || c == 'a') {
    return -1;
  }
  
  return 0;
}

static int incr_y(char c) {
  if (c == 'v' || c == 's') {
    return 1;
  } else if (c == '^' || c == 'w') {
    return -1;
  }

  return 0;
}

/* Task 4.2 
 * get the char the snake will meet
 */
static char next_square(game_state_t* state, int snum) {
  if (snum >= state->num_snakes) {
    return '?';
  }
  
  snake_t *snakes = state->snakes;
  int snake_x = snakes[snum].head_x;
  int snake_y = snakes[snum].head_y;
  int item_x = snake_x + incr_x(get_board_at(state, snake_x, snake_y));
  int item_y = snake_y + incr_y(get_board_at(state, snake_x, snake_y));
  char item = get_board_at(state, item_x, item_y);
  return item;
}

/* Task 4.3 */
static void update_head(game_state_t* state, int snum) {
  // update snake head
  snake_t *snakes = state->snakes;
  int snake_x = snakes[snum].head_x;
  int snake_y = snakes[snum].head_y;
  snakes[snum].head_x += incr_x(get_board_at(state, snake_x, snake_y));
  snakes[snum].head_y += incr_y(get_board_at(state, snake_x, snake_y));

  // update the board
  char new_head = get_board_at(state, snake_x, snake_y);
  set_board_at(state, snakes[snum].head_x, snakes[snum].head_y, new_head);

  return;
}

/* Task 4.4 */
static void update_tail(game_state_t* state, int snum) {
  // update the snake tail
  snake_t *snakes = state->snakes;
  int tail_x = snakes[snum].tail_x;
  int tail_y = snakes[snum].tail_y;
  snakes[snum].tail_x += incr_x(get_board_at(state, tail_x, tail_y));
  snakes[snum].tail_y += incr_y(get_board_at(state, tail_x, tail_y));

  // update the board
  char target = body_to_tail(get_board_at(state, snakes[snum].tail_x, snakes[snum].tail_y));
  set_board_at(state, snakes[snum].tail_x, snakes[snum].tail_y, target);
  set_board_at(state, tail_x, tail_y, ' ');

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  char next_item;
  snake_t *snakes = state->snakes;

  for (size_t i = 0; i < state->num_snakes; i++) {
    next_item = next_square(state, i);
    // if the destination is wall or snake, the snake die
    if (next_item == '#' || is_snake(next_item)) {
      snakes[i].live = false;
      set_board_at(state, snakes[i].head_x, snakes[i].head_y, 'x');
    } else if (next_item == '*') {
      // if the destination is fruit, then only update the head
      update_head(state, i);

      // generate a new fruit
      add_food(state);
    } else {
      update_head(state, i);
      update_tail(state, i);
    }
  }

  return;
}

/* Task 5 */
// board结尾并没有换行符，只有在打印到文件中时需要在末尾加上换行符
game_state_t* load_board(char* filename) {
  game_state_t *gsp = (game_state_t *)malloc(sizeof(game_state_t));
  gsp->board = (char **)malloc(sizeof(char *));

  FILE *fp = NULL;
  fp = fopen(filename, "r");
  char buffer[255];

  // get the length of the rectanagle
  fgets(buffer, 255, fp);
  // delete the \n
  buffer[strlen(buffer) - 1] = '\0';
  int length = strlen(buffer);
  gsp->x_size = length;
  gsp->board[0] = malloc(sizeof(char) * (length + 1));
  strcpy(gsp->board[0], buffer);
  
  size_t i = 1;
  for (; fgets(buffer, 255, fp); i++) {
    gsp->board = (char **)realloc(gsp->board, sizeof(char *) * (i + 1));
    gsp->board[i] = (char *)malloc(sizeof(char) * (length + 1));
    buffer[strlen(buffer) - 1] = '\0';
    strcpy(gsp->board[i], buffer);
  }
  gsp->y_size = i;

  fclose(fp);
  return gsp;
}

/* Task 6.1 */
// 思路：先找到尾巴，再移动到头
static void find_head(game_state_t* state, int snum) {
  char c = '\0';
  int count = 0, x = 0, y = 0;
  bool lable = true;

// find the snumth snake's tail
  for (size_t i = 0; i < state->y_size && lable; i++) {
    for (size_t j = 0; j < state->x_size; j++) {
      c = get_board_at(state, j, i);
      if (is_tail(c)) {
        count++;
      }
      if (snum == count - 1) {
        lable = false;
        x = j;
        y = i;
        break;
      }
    }
  }
  state->snakes[snum].tail_x = x;
  state->snakes[snum].tail_y = y;
  
  // find the head
  char next = c;
  int next_x = x, next_y = y;
  while (is_snake(next)) {
    x = next_x;
    y = next_y;
    c = get_board_at(state, x, y);

    next_x += incr_x(c);
    next_y += incr_y(c);
    next = get_board_at(state, next_x, next_y);
  }
  state->snakes[snum].head_x = x;
  state->snakes[snum].head_y = y;
  state->snakes[snum].live = (c != 'x') ? true : false;
  
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  char c = '\0';
  int count = 0;

  // count the number of snakes
  for (size_t i = 0; i < state->y_size; i++) {
    for (size_t j = 0; j < state->x_size; j++) {
      c = get_board_at(state, j, i);
      if (is_tail(c)) {
        count++;
      }
    }
  }
  state->num_snakes = count;

  // create the snakes
  state->snakes = malloc(sizeof(snake_t) * count);
  for (size_t i = 0; i < count; i++) {
    find_head(state, i);
  }

  return state;
}
