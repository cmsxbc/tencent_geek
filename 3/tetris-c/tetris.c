#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


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


struct game_t {
    int grids[Y_COUNT][X_COUNT];
    int occupied;
    int brick_count;
    int brick_shape_index;
    int brick_center_x;
    int brick_center_y;
};

typedef struct game_t GAME_T;

inline int get_next_random(int v) {
    return (v * RANDOM_A + RANDOM_C) % RANDOM_M;
}


int* get_shape_series() {
    static int * shape_series = NULL;
    if (shape_series == NULL) {
        shape_series = (int *) calloc(MAX_BRICK_COUNT, sizeof(int));
        int v = RANDOM_V;
        int shape_index = 0;
        int index_limit[7] = {1, 4, 7, 11, 16, 22, 29};
        for (int i = 0; i < MAX_BRICK_COUNT; i++) {
            v = get_next_random(v);
            while (v % 29 <= index_limit[shape_index]) {
                shape_index ++;
            }
            shape_index = shape_index * 4 + v % 4;
            shape_series[i] = shape_index;
        }
    }
    return shape_series;
}


inline GAME_T init_game() {
    GAME_T game = {{GRID_EMPTY}, 0, 0, get_shape_series()[0], INIT_X, INIT_Y};
    return game;
}

inline GAME_T copy_game(GAME_T * p_game) {
    GAME_T game = init_game();
    for (int y = 0; y < Y_COUNT; y ++) {
        for (int x = 0; x < X_COUNT; x ++) {
            game.grids[y][x] = p_game->grids[y][x];
        }
    }
    return game;
}


inline void print_game(GAME_T * p_game) {
    printf("|----------|\n");
    for (int y = 0; y < Y_COUNT; y++) {
        printf("|");
        for (int x = 0; x < X_COUNT; x ++) {
            printf("%c", p_game->grids[y][x] ? '#' : ' ');
        }
        printf("|\n");
    }
}


int rotate(int shape_index) {
    if (shape_index % 4 >= 3) {
        shape_index -= 3;
    } else {
        shape_index += 1;
    }
    return shape_index;
}


void do_rotate(GAME_T *p_game) {
    p_game->brick_shape_index = rotate(p_game->brick_shape_index);
}


int main() {

}
