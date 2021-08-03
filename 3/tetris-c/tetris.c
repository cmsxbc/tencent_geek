#include <stdio.h>


const int Y_COUNT = 20;
const int X_COUNT = 10;
const int INIT_X = 4;
const int INIT_Y = 0;

const int MAX_BRICK_COUNT = 10000;
const int GRID_EMPTY = 0;
const int GRID_SETTED = 1;

const int RANDOM_A = 27073;
const int RANDOM_C = 17713;
const int RANDOM_M = 32749;
const int RANDOM_V = 12358;

const int SHAPE_TYPE_COUNT = 7;
const int SHAPE_STATE_COUNT = 4;
const int SHAPE_GRID_COUNT = 4;

const int SHAPES[SHAPE_TYPE_COUNT][SHAPE_STATE_COUNT][SHAPE_GRID_COUNT][2] = {
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    },
    {
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
    }
};

struct game_t {
    int grids[Y_COUNT][X_COUNT];
    int occupied;
    int v;
    int brick_count;
};

typedef struct game_t GAME_T;

struct brick_t {
    int center_x;
    int center_y;
};

typedef struct brick_t BRICK_T;

inline BRICK_T init_brick() {
    BRICK_T brick = {INIT_X, INIT_Y};
    return brick;
}

inline int get_next_random(int v) {
    return (v * RANDOM_A + RANDOM_C) % RANDOM_M;
}


inline GAME_T init_game() {
    GAME_T game = {{GRID_EMPTY}, 0, RANDOM_V, 0};
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

int main() {

}
