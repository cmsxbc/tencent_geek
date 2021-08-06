#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define Y_COUNT 20
#define X_COUNT 10
const int INIT_X = 4;
const int INIT_Y = 0;

const int MAX_BRICK_COUNT = 10000;
const int GRID_EMPTY = 0;
const int GRID_SET = 1;

const int RANDOM_A = 27073;
const int RANDOM_C = 17713;
const int RANDOM_M = 32749;
const int RANDOM_V = 12358;

#define SHAPE_TYPE_COUNT 7
#define SHAPE_STATE_COUNT 4
#define SHAPE_TOTAL_COUNT 28
#define SHAPE_GRID_COUNT 4


const int SHAPES[SHAPE_TOTAL_COUNT][SHAPE_GRID_COUNT][2] = {
    // I 型
    {
        {0, 0},
        {0, -1},
        {0, -2},
        {0, 1},
    },
    {
        {0, 0},
        {1, 0},
        {2, 0},
        {-1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {0, -2},
        {0, 1},
    },
    {
        {0, 0},
        {1, 0},
        {2, 0},
        {-1, 0},
    },
    // L 型
    {
        {0, 0},
        {0, -1},
        {0, -2},
        {1, 0},
    },
    {
        {0, 0},
        {1, 0},
        {2, 0},
        {0, 1},
    },
    {
        {0, 0},
        {-1, 0},
        {0, 1},
        {0, 2},
    },
    {
        {0, 0},
        {0, -1},
        {-1, 0},
        {-2, 0},
    },
    // J 型
    {
        {0, 0},
        {0, -1},
        {0, -2},
        {-1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {1, 0},
        {2, 0},
    },
    {
        {0, 0},
        {1, 0},
        {0, 1},
        {0, 2},
    },
    {
        {0, 0},
        {-1, 0},
        {-2, 0},
        {0, 1},
    },
    // T 型
    {
        {0, 0},
        {1, 0},
        {0, 1},
        {-1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {0, 1},
        {-1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {1, 0},
        {-1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {1, 0},
        {0, 1},
    },
    // O 型
    {
        {0, 0},
        {0, -1},
        {1, -1},
        {1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {1, -1},
        {1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {1, -1},
        {1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {1, -1},
        {1, 0},
    },
    // S 型
    {
        {0, 0},
        {0, -1},
        {1, -1},
        {-1, 0},
    },
    {
        {0, 0},
        {-1, 0},
        {-1, -1},
        {0, 1},
    },
    {
        {0, 0},
        {0, -1},
        {1, -1},
        {-1, 0},
    },
    {
        {0, 0},
        {-1, 0},
        {-1, -1},
        {0, 1},
    },
    // Z 型
    {
        {0, 0},
        {0, -1},
        {1, 0},
        {-1, -1},
    },
    {
        {0, 0},
        {0, -1},
        {-1, 1},
        {-1, 0},
    },
    {
        {0, 0},
        {0, -1},
        {1, 0},
        {-1, -1},
    },
    {
        {0, 0},
        {0, -1},
        {-1, 1},
        {-1, 0},
    },
};

union operation_t {
    struct {
        u_int8_t type:3;
        u_int8_t count:5;
    } s;
    u_int8_t v;
};

typedef union operation_t OPERATION_T;

enum OP_TYPE {
    OP_C = 0,
    OP_L = 1,
    OP_D = 2,
    OP_U = 3,
    OP_R = 4,
    OP_C_ = 5,
    OP_N = 6
};


struct game_t {
    int (*p_grids)[Y_COUNT][X_COUNT];
    int occupied;
    int brick_count;
    int shape_series_index;
    int brick_center_x;
    int brick_center_y;
    int score;
};

typedef struct game_t GAME_T;

static inline int get_next_random(int v) {
    return (v * RANDOM_A + RANDOM_C) % RANDOM_M;
}


static inline int* get_shape_series() {
    static int * shape_series = NULL;
    if (shape_series == NULL) {
        shape_series = (int *) calloc(MAX_BRICK_COUNT, sizeof(int));
        int v = RANDOM_V;
        int index_limit[7] = {1, 4, 7, 11, 16, 22, 29};
        for (int i = 0; i < MAX_BRICK_COUNT; i++) {
            v = get_next_random(v);
            int shape_index = 0;
            while (v % 29 > index_limit[shape_index]) {
                shape_index ++;
            }
            shape_index = shape_index * 4 + v % 4;
            shape_series[i] = shape_index;
        }
    }
    return shape_series;
}


static inline GAME_T init_game() {
    GAME_T game = {NULL, 0, 0, get_shape_series()[0], INIT_X, INIT_Y, 0};
    game.p_grids = (int (*)[Y_COUNT][X_COUNT])calloc(X_COUNT * Y_COUNT, sizeof(int));
    for (int y = 0; y < Y_COUNT; y ++) {
        for (int x = 0; x < X_COUNT; x ++) {
            (*game.p_grids)[y][x] = GRID_EMPTY;
        }
    }
    return game;
}

static inline void free_game(GAME_T *p_game) {
    if (p_game->p_grids) {
        free(p_game->p_grids);
    }
}

static inline GAME_T copy_game(GAME_T * p_game) {
    GAME_T game = init_game();
    for (int y = 0; y < Y_COUNT; y ++) {
        for (int x = 0; x < X_COUNT; x ++) {
            (*game.p_grids)[y][x] = (*p_game->p_grids)[y][x];
        }
    }
    game.shape_series_index = p_game->shape_series_index;
    game.brick_center_x = p_game->brick_center_x;
    game.brick_center_y = p_game->brick_center_y;
    game.brick_count = p_game->brick_count;
    game.occupied = p_game->occupied;
    game.score = p_game->score;
    return game;
}


static inline void print_shape(int shape_index) {
    char grids[5][5] = {};
    memset(grids, ' ', 25);
    for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
        grids[SHAPES[shape_index][i][1]+2][SHAPES[shape_index][i][0]+2] = '#';
    }
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%c", grids[i][j]);
        }
        printf("\n");
    }
}


static inline void print_game(GAME_T * p_game) {
    printf("brick_count:%d\n", p_game->brick_count);
    printf("shape_series_index:%d\n", p_game->shape_series_index);
    print_shape(p_game->shape_series_index);
    printf(" _ _ _ _ _ _ _ _ _ _ \n");
    for (int y = 0; y < Y_COUNT; y++) {
        printf("|");
        for (int x = 0; x < X_COUNT; x ++) {
            char c = ' ';
            if ((*p_game->p_grids)[y][x] == GRID_SET) {
                c = '#';
            } else {
                int shape_idx = p_game->shape_series_index;
                for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
                    if (x == SHAPES[shape_idx][i][0] + p_game->brick_center_x && y == SHAPES[shape_idx][i][1] + p_game->brick_center_y) {
                        c = '#';
                        break;
                    }
                }
            }
            printf("%c|", c);
        }
        printf("\n");
    }
}

static inline void next_brick(GAME_T * p_game) {
}


static inline void rotate(GAME_T *p_game, int n) {
    int shape_index = p_game->shape_series_index;
    n = n % 4;
    if (shape_index % 4 >= 3) {
        shape_index -= (4 - n) % 4;
    } else {
        shape_index += n;
    }
    p_game->shape_series_index = shape_index;
}


static inline void rotate_(GAME_T *p_game, int n) {
    n = 4 - (n % 4);
    rotate(p_game, n);
}


static inline void move_d(GAME_T *p_game, int n) {
    p_game->brick_center_y += n;
}


static inline void move_l(GAME_T *p_game, int n) {
    p_game->brick_center_x -= n;
}


static inline void move_r(GAME_T *p_game, int n) {
    p_game->brick_center_x += n;
}

static inline void move_u(GAME_T *p_game, int n) {
    p_game->brick_center_y -= n;
}


static inline void operate(GAME_T *p_game, OPERATION_T op) {
    static void (*funcs[OP_N])(GAME_T*, int) = {
            rotate,
            move_l,
            move_d,
            move_u,
            move_r,
            rotate_
    };
    funcs[op.s.type](p_game, op.s.count);
}

static inline void operate_(GAME_T *p_game, OPERATION_T op) {
    OPERATION_T inv_op = op;
    inv_op.s.type = OP_N - 1 - op.s.type;
    operate(p_game, inv_op);
}

static inline void print_all_shape_series() {
    int * shape_series = get_shape_series();
    for (int i = 0; i < MAX_BRICK_COUNT; i++) {
        printf("shape no: %d\n", i+1);
        print_shape(shape_series[i]);
    }
}

int main() {
    print_all_shape_series();
    GAME_T game = init_game();
    print_game(&game);
}
