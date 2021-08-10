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
#define MAX_OP_ON_BRICK 4

#define abs_diff(x, y) (x) < (y) ? ((y) - (x)) : ((x) - (y))


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


const char OP_NAMES[10] = "CLDUR)N";

const u_int8_t score_rate[5] = {0, 1, 3, 6, 10};


union row_t {
    // struct {
    //     u_int16_t c9: 1;
    //     u_int16_t c8: 1;
    //     u_int16_t c7: 1;
    //     u_int16_t c6: 1;
    //     u_int16_t c5: 1;
    //     u_int16_t c4: 1;
    //     u_int16_t c3: 1;
    //     u_int16_t c2: 1;
    //     u_int16_t c1: 1;
    //     u_int16_t c0: 1;
    // } cols;
    u_int16_t v;
};
typedef union row_t ROW_T;


struct game_t {
    ROW_T p_grids[Y_COUNT];
    u_int16_t brick_count;
    u_int8_t occupied;
    u_int8_t shape_index;
    u_int8_t brick_center_x;
    u_int8_t brick_center_y;
    int score;
};

typedef struct game_t GAME_T;


struct search_config_t {
    double aggregate_height;
    double line;
    double hole;
    double bumpiness;
    double occupied_below_cordon;
    double occupied_above_cordon;
    // double score_increase;
    int max_depth;
    u_int16_t round_cordon;
    u_int8_t occupied_cordon;
};

typedef struct search_config_t SEARCH_CONFIG_T;


static inline void normalize_weights(SEARCH_CONFIG_T *p_config) {
    double total = 0.0;
    total += p_config->line;
    total += p_config->bumpiness;
    total += p_config->hole;
    total += p_config->occupied_below_cordon;
    total += p_config->occupied_above_cordon;
    total += p_config->aggregate_height;
    p_config->line = p_config->line / total;
    p_config->bumpiness = p_config->bumpiness / total;
    p_config->hole = p_config->hole / total;
    p_config->occupied_below_cordon = p_config->occupied_below_cordon / total;
    p_config->occupied_above_cordon = p_config->occupied_above_cordon / total;
    p_config->aggregate_height = p_config->aggregate_height / total;
}


struct stats_t {
    u_int8_t heights[X_COUNT];
    u_int8_t aggregate_height;
    u_int8_t hole;
    u_int8_t bumpiness;
    u_int8_t line_cleared;
    int8_t occupied_increased;
};

typedef struct stats_t STATS_T;


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
            shape_index = shape_index * 4 + i % 4;
            shape_series[i] = shape_index;
        }
    }
    return shape_series;
}


static inline GAME_T init_game() {
    GAME_T game = {{GRID_EMPTY}, 0, 0, get_shape_series()[0], INIT_X, INIT_Y, 0};
    // game.p_grids = (ROW_T *) calloc(Y_COUNT, sizeof(ROW_T));
    // memset(game.p_grids, GRID_EMPTY, sizeof(ROW_T) * Y_COUNT);
    return game;
}

static inline void free_game(GAME_T *p_game) {
    // if (p_game->p_grids) {
    //     free(p_game->p_grids);
    //     p_game->p_grids = NULL;
    // }
}

static inline GAME_T copy_game(GAME_T * p_game) {
    GAME_T game = init_game();
    for (int y = 0; y < Y_COUNT; y ++) {
        game.p_grids[y] = p_game->p_grids[y];
    }
    game.shape_index = p_game->shape_index;
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
    printf("shape_series_index:%d\n", p_game->shape_index);
    printf("score:%d\n", p_game->score);
    printf("occupied:%d\n", p_game->occupied);
    // print_shape(p_game->shape_index);
    printf(" _ _ _ _ _ _ _ _ _ _ \n");
    for (int y = 0; y < Y_COUNT; y++) {
        printf("|");
        for (int x = 0; x < X_COUNT; x ++) {
            char c = ' ';
            if (p_game->p_grids[y].v & (1 << x)) {
                c = '#';
                int shape_idx = p_game->shape_index;
                for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
                    if (x == SHAPES[shape_idx][i][0] + p_game->brick_center_x && y == SHAPES[shape_idx][i][1] + p_game->brick_center_y) {
                        c = 'x';
                        break;
                    }
                }
            } else {
                int shape_idx = p_game->shape_index;
                for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
                    if (x == SHAPES[shape_idx][i][0] + p_game->brick_center_x && y == SHAPES[shape_idx][i][1] + p_game->brick_center_y) {
                        c = '*';
                        break;
                    }
                }
            }
            printf("%c|", c);
        }
        printf("\n");
    }
}


static inline bool valid(GAME_T *p_game) {
    for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
        int x = p_game->brick_center_x + SHAPES[p_game->shape_index][i][0];
        if (x < 0 || x >= X_COUNT) {
            return false;
        }
        int y = p_game->brick_center_y + SHAPES[p_game->shape_index][i][1];
        if (y >= Y_COUNT) {
            return false;
        }
        if ((p_game->p_grids[y].v & (1 << x)) == GRID_SET) {
            return false;
        }
    }
    return true;
}


const static u_int8_t COL_COUNT_MAPPING[1<<X_COUNT] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1,
        2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2,
        2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3,
        4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3,
        2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3,
        4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4,
        4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5,
        6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3,
        4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
        4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5,
        6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
        4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,
        6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6,
        6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7,
        8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3,
        4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4,
        4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5,
        6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5,
        4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5,
        6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6,
        6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7,
        8, 6, 7, 7, 8, 7, 8, 8, 9, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5,
        6, 6, 7, 6, 7, 7, 8, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6,
        6, 7, 6, 7, 7, 8, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7,
        8, 7, 8, 8, 9, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7,
        6, 7, 7, 8, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7,
        8, 8, 9, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8,
        8, 9, 5, 6, 6, 7, 6, 7, 7, 8, 6, 7, 7, 8, 7, 8, 8, 9, 6, 7, 7, 8, 7, 8, 8, 9, 7, 8, 8, 9, 8, 9, 9,
        10
};


static inline u_int8_t calc_occupied(GAME_T * p_game) {
    u_int8_t occupied = 0;
    for (int y = 0; y < Y_COUNT; y++) {
        occupied += COL_COUNT_MAPPING[p_game->p_grids[y].v];
    }
    return occupied;
}


static inline u_int8_t calc_x_height(GAME_T * p_game, int x) {
    u_int16_t b = 1 << x;
    for (int y = 0; y < Y_COUNT; y++) {
        if (p_game->p_grids[y].v & b) {
            return Y_COUNT - y;
        }
    }
    return 0;
}


static inline bool next_brick(GAME_T * p_game, STATS_T * p_stats) {
    u_int8_t cleared_y[4] = {255, 255, 255, 255};
    int8_t clear = 0;
    for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
        int x = p_game->brick_center_x + SHAPES[p_game->shape_index][i][0];
        int y = p_game->brick_center_y + SHAPES[p_game->shape_index][i][1];
        if (p_game->p_grids[y].v & (1 << x)) {
            return false;
        }
        p_game->p_grids[y].v |= 1 << x;
    }
    if (p_game->p_grids[0].v > 0) {
        return false;
    }
    for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
        int y = p_game->brick_center_y + SHAPES[p_game->shape_index][i][1];
        if (p_game->p_grids[y].v == (1 << X_COUNT) - 1) {
            int j = clear - 1;
            cleared_y[clear++] = y;
            while (j >= 0) {
                if (y < cleared_y[j]) {
                    cleared_y[j+1] = cleared_y[j];
                    cleared_y[j] = y;
                    j --;
                } else {
                    break;
                }
            }
            p_game->p_grids[y].v = GRID_EMPTY;
        }
    }
    for (int i = 0; i < clear; i++) {
        // printf("cleared: %d\n", cleared_y[i]);
        for (int y = cleared_y[i]; y > 0; y --) {
            p_game->p_grids[y].v = p_game->p_grids[y-1].v;
        }
        p_game->p_grids[0].v = GRID_EMPTY;
    }
    if (p_stats != NULL) {
        p_stats->line_cleared = clear;
        for (int x = 0; x < X_COUNT; x++) {
            p_stats->heights[x] = calc_x_height(p_game, x);
            p_stats->aggregate_height += p_stats->heights[x];
        }
        for (int x = 1; x < X_COUNT; x++) {
            p_stats->bumpiness += abs_diff(p_stats->heights[x-1], p_stats->heights[x]);
        }
        p_stats->occupied_increased = 4 - clear * X_COUNT;
    }

    p_game->score += score_rate[clear] * (p_game->occupied + 4);
    p_game->occupied -= clear * X_COUNT - 4;
    u_int8_t occupied = calc_occupied(p_game);
    if (p_game->occupied != occupied) {
        printf("something wrong for occupied %d != %d\n", p_game->occupied, occupied);
        print_game(p_game);
    }
    p_game->brick_count ++;
    p_game->shape_index = get_shape_series()[p_game->brick_count];
    p_game->brick_center_x = INIT_X;
    p_game->brick_center_y = INIT_Y;
    return valid(p_game);
}


static inline u_int8_t get_max_move_d(GAME_T * p_game) {
    u_int8_t max_dy = Y_COUNT;
    for (int i = 0; i < SHAPE_GRID_COUNT; i++) {
        u_int8_t x = p_game->brick_center_x + SHAPES[p_game->shape_index][i][0];
        u_int8_t y = p_game->brick_center_y + SHAPES[p_game->shape_index][i][1];
        u_int8_t cur_dy = Y_COUNT - calc_x_height(p_game, x) - y - 1;
        max_dy = cur_dy < max_dy ? cur_dy : max_dy;
    }
    return max_dy;
}


static inline void rotate(GAME_T *p_game, int n) {
    u_int8_t shape_offset = p_game->shape_index % 4;
    u_int8_t shape_base = p_game->shape_index - shape_offset;
    u_int8_t shape_index = shape_base + (shape_offset + n) % 4;
    // printf("rotate (%d): %d -> %d\n", n, p_game->shape_index, shape_index);
    // print_shape(p_game->shape_index);
    // print_shape(shape_index);
    p_game->shape_index = shape_index;
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


static inline void print_op_series(OPERATION_T *ops, int op_count) {
    for (int i = 0; i < op_count; i++) {
        if (ops[i].s.type == OP_N) {
            printf("%c", OP_NAMES[OP_N]);
        } else {
            printf("%c%d", OP_NAMES[ops[i].s.type], ops[i].s.count);
        }
        if (i < op_count - 1) {
            printf(",");
        } else {
            printf("\n");
        }
    }
}


static inline bool is_game_over(GAME_T *p_game) {
    return p_game->p_grids[0].v > 0 || p_game->brick_count >= MAX_BRICK_COUNT;
}


static inline double calc_search_score(GAME_T *p_game, SEARCH_CONFIG_T *p_search_config, STATS_T *p_stats) {
    if (p_game->p_grids[0].v > 0) {
        if (p_game->brick_count < p_search_config->round_cordon) {
            return 0.0;
        }
    }
    double score = 0.0;
    score -= p_stats->bumpiness * p_search_config->bumpiness;
    if (p_game->occupied >= p_search_config->occupied_cordon) {
        score -= p_game->occupied * p_search_config->occupied_above_cordon;
    } else {
        score += p_game->occupied * p_search_config->occupied_below_cordon;
    }

    score -= p_stats->aggregate_height * p_search_config->aggregate_height;
    score -= p_stats->hole * p_search_config->hole;
    score += score_rate[p_stats->line_cleared] * p_search_config->line;

    return score;
}


static double df_game(SEARCH_CONFIG_T *p_search_config, GAME_T *p_game, int depth, OPERATION_T *ops, int *op_count) {
    double max_score = -2e100;
    double cur_score;
    OPERATION_T max_ops[MAX_OP_ON_BRICK];
    OPERATION_T cur_ops[MAX_OP_ON_BRICK] = {{0}};
    int max_op_count = 0;
    for (int rotate_n = 0; rotate_n < SHAPE_STATE_COUNT; rotate_n ++) {
        GAME_T game_s = copy_game(p_game);
        int cur_op_count = 0;
        cur_ops[cur_op_count].s.type = OP_C;
        cur_ops[cur_op_count].s.count = rotate_n;
        operate(&game_s, cur_ops[cur_op_count]);
        cur_op_count++;
        if (!valid(&game_s) || is_game_over(&game_s)) {
            free_game(&game_s);
            continue;
        }
        for (int x = 0; x < X_COUNT; x++) {
            cur_op_count = 1;
            GAME_T game = copy_game(&game_s);
            if (x < INIT_X) {
                cur_ops[cur_op_count].s.type = OP_L;
                cur_ops[cur_op_count].s.count = INIT_X - x;
                operate(&game, cur_ops[cur_op_count]);
                cur_op_count ++;
            } else if (x > INIT_X) {
                cur_ops[cur_op_count].s.type = OP_R;
                cur_ops[cur_op_count].s.count = x - INIT_X;
                operate(&game, cur_ops[cur_op_count]);
                cur_op_count ++;
            }
            if (!valid(&game) | is_game_over(&game)) {
                continue;
            }
            u_int8_t max_dy = get_max_move_d(&game);
            if (max_dy > 0) {
                cur_ops[cur_op_count].s.type = OP_D;
                cur_ops[cur_op_count].s.count = max_dy;
                operate(&game, cur_ops[cur_op_count]);
                cur_op_count ++;
            }
            STATS_T stats;
            bool validation = next_brick(&game, &stats);
            if (!validation) {
                cur_score = -2.222223e100;
            } else if (depth >= p_search_config->max_depth || p_game->brick_count >= MAX_BRICK_COUNT - 1) {
                cur_score = calc_search_score(&game, p_search_config, &stats);
            } else {
                cur_score = df_game(p_search_config, &game, depth+1, NULL, NULL);
            }
            if (cur_score > max_score) {
                memcpy(max_ops, cur_ops, sizeof(OPERATION_T) * cur_op_count);
                max_op_count = cur_op_count;
                max_score = cur_score;
            }
        }
    }
    if (ops != NULL && op_count != NULL) {
        *op_count = 0;
        for (*op_count; *op_count < max_op_count; (*op_count)++) {
            ops[*op_count].v = max_ops[*op_count].v;
        }
    }
    return max_score;
}

static inline void bf_game(SEARCH_CONFIG_T search_config) {
    GAME_T game = init_game();
    OPERATION_T cur_ops[MAX_OP_ON_BRICK];
    int op_count = 0;
    do {
        op_count = 0;
        memset(cur_ops, 0, sizeof(OPERATION_T) * MAX_OP_ON_BRICK);
        df_game(&search_config, &game, 0, cur_ops, &op_count);
        for (int i = 0; i < op_count; i++) {
            operate(&game, cur_ops[i]);
        }
        printf("current_step: %d, current_score: %d, ops: ", game.brick_count, game.score);
        print_op_series(cur_ops, op_count);
        if (!next_brick(&game, NULL)) {
            printf("[WARN] invalid while do next brick!!!");
            break;
        }
    } while(valid(&game) && !is_game_over(&game));
    print_game(&game);
    // free_game(&game);
}


static inline void run_game(GAME_T *p_game, OPERATION_T *operations, int op_count) {

}


static inline bool run_game_string(GAME_T *p_game, char *op_str, int len) {
    char op_name;
    int offset = 0;
    int count;
    OPERATION_T op;
    int op_count = 0;
    while (offset < len) {
        if (sscanf(op_str+offset, "%c", &op_name) <= 0) {
            printf("something wrong while read string at %d: %s\n", offset, op_str+offset);
            return false;
        }
        if (offset == 0 && op_name == 'N' && op_str[offset+1] == ',') {
            offset += 2;
            continue;
        }
        switch (op_name) {
            case 'N':
                op.s.type = OP_N;
                break;
            case 'C':
                op.s.type = OP_C;
                break;
            case 'L':
                op.s.type = OP_L;
                break;
            case 'R':
                op.s.type = OP_R;
                break;
            case 'D':
                op.s.type = OP_D;
                break;
            default:
                printf("unknown op type %c at %d: %s\n", op_name, offset, op_str+offset);
                return false;
        }
        offset ++;
        count = 0;
        while(op_str[offset+count] != ',' && offset + count < len) {
            count ++;
        }
        if (!((op_name == 'N') ^ (count > 0))) {
            printf("op:%c, count:%d at %d:%s\n", op_name, count, offset, op_str+offset);
            return false;
        }
        if (op_name == 'N') {
            if (next_brick(p_game, NULL) == false) {
                printf("next_brick not valid at %d\n", offset);
                return false;
            }
        } else {
            char * end = op_str + offset + count;
            op.s.count = strtol(op_str+offset, &end, 10);
            if (end != op_str + offset + count) {
                return false;
            }
            operate(p_game, op);
            if (valid(p_game) == false) {
                printf("op:%c%d not valid\n", OP_NAMES[op.s.type], op.s.count);
                return false;
            }
        }
        op_count ++;
        // printf("step %d\n", op_count++);
        // print_game(p_game);
        offset += count + 1;
    }
    printf("total steps: %d\n", op_count);
    return true;
}


int main() {
    // print_all_shape_series();
    GAME_T game = init_game();
    print_game(&game);
    char *ops = "N,D3,C1,L3,D15,N,D1,L1,D18,L1,N,D1,L2,D17,N,D1,R1,D17,N,D1,R1,C2,D1,C1,R2,D1,R1,D16,N,D1,R2,C1,D17,N,D1,C1,L2,R2,D1,R3,D14,N,D1,C1,R2,D1,R2,D14,N,C1,D1,L3,D15,N,D1,L2,D15,N,D1,C1,D1,R1,D13,N,D1,L1,D1,L1,R1,D12,N,D1,C1,L3,D13,N,D1,L3,D11,N,D1,R5,D17,N,R1,D1,C1,D1,L1,D1,L2,D1,R1,D13,N,D1,C1,L3,D13,N,C1,D1,C1,R2,D1,R1,D16,N,D1,R2,L1,D16,N,D1,C3,R1,D1,R4,D15,N,D1,R2,D1,C1,L3,D1,R1,D14,N,R4,D18,N,R1,D1,R2,D16,N,D1,C1,D1,C3,D1,C2,R3,C1,D1,C1,R2,D14,N,D1,C1,R1,D17,N,D1,R1,D1,R2,D15,N,D1,C1,D1,R1,D1,R1,C1,L2,D1,L1,D13,N,C1,D1,C2,D1,R1,D1,C2,D1,C2,D1,R3,D10,N,D1,R1,D1,R1,D14,N,D1,L3,D15,N,C1,L1,D15,N,C1,D1,L4,D13,N,D1,C1,L1,D12,N,D1,R2,D13,N,R1,C1,D1,R3,D1,R1,L1,C1,D12,N,C1,D1,C2,R1,D1,L1,D11,N,D1,R1,C1,D1,R3,D11,N,D13,N,D1,C1,L2,D12,N,D1,R2,C1,D1,L5,D11,N,D1,C1,R1,D1,R2,D11,N,D1,L2,D1,R2,D1,R1,D9,N,D1,C1,D11,N,L1,D1,L2,D10,N,L2,D11,N,D1,C1,R5,D11,N,D1,L1,D1,L1,R2,D11,N,L3,D12,N,C1,D1,C2,R2,D1,R3,D12,N,D1,R2,D13,N,D1,L2,D12,N,D1,C1,L2,D1,L2,D9,N,D1,R3,D1,L1,D1,L2,D9,N,D2,L1,D1,C3,L1,D1,L1,D7,N,D2,R1,L1,D1,R4,D1,L1,D9,N,R3,D1,R2,D11,N,D1,R2,D1,R1,D11,N,D1,R1,D11,N,R1,D1,C2,R1,D10,N,R2,D1,R2,D9,N,D1,L2,D11,N,D1,C1,R4,D1,R1,D11,N,D1,C1,D1,L2,D12,N,D2,L1,D13,N,D1,C1,L2,D1,L1,D12,N,D1,C1,D1,C3,D1,L2,D10,N,D1,C1,D1,L3,D10,N,D1,L3,D9,N,R2,D1,L1,D14,N,D1,R1,C1,R2,D1,R1,D1,L2,D1,L1,D9,N,D1,R5,D13,N,R1,D1,R1,C1,D1,R1,D14,N,D1,C1,D13,N,D1,R1,D13,N,D1,R2,D1,C1,D1,L3,D10,N,D12,N,R3,D1,C1,R2,D14,N,D1,R4,D13,N,D2,L1,C1,L2,D11,N,L1,D13,N,R1,D1,C1,R1,D1,R3,D11,N,D1,R3,D11,N,L1,D1,R1,D12,N,C1,D1,L1,C1,D1,C2,L1,D1,C1,R1,D1,L1,D8,N,C1,D1,C2,D10,N,D1,C2,L3,D1,L1,R2,D1,R2,C1,R1,D1,R1,D7,N,C1,D1,L1,C1,L3,D11,N,R2,D1,C1,L1,D1,L1,C1,L2,D8,N,R2,D1,C1,R3,D11,N,D1,R4,D1,R1,D10,N,D1,R1,C1,D1,R2,D11,N,D1,C2,D1,C2,D1,C2,D1,C2,L1,C1,L1,D1,L2,D8,N,D1,R1,D1,C2,D1,C1,L2,D10,N,R2,D2,L1,C1,L3,D10,N,D1,R1,D11,N,D1,R1,D1,C4,D1,R1,D10,N,R2,D1,R3,D14,N,D1,R3,D1,R2,D11,N,D14,N,C1,D1,C2,R1,D1,R2,D11,N,R1,D14,N,D1,C1,D1,C1,R5,D11,N,L2,D1,C1,L1,D13,N,D1,L1,D13,N,D1,C1,D1,L3,D12,N,D1,R2,D1,R1,D12,N,D1,R4,D1,R1,D10,N,D1,C1,L1,C1,D1,L2,R1,D11,N,D1,C1,R1,D13,N,D1,R1,C1,D1,R1,D11,N,D3,C1,L1,D11,N,C1,D14,N,D1,R1,C2,D1,C1,R1,D1,R1,D12,N,D1,L3,D14,N,R1,D16,N,D1,C3,L1,D13,N,D1,C1,R1,D1,R3,D13,N,D1,L1,D1,L2,D13,N,D1,R1,D14,N,C1,D1,C1,D1,C2,D1,L2,D11,N,R2,D1,R1,C1,D1,L1,D11,N,D1,R1,D1,L2,C1,D1,C1,L3,D10,N,L1,D13,N,C1,D1,L3,D10,N,R2,D1,C1,D1,R1,D1,R1,D1,L4,D7,N,R1,D1,C1,R3,D13,N,R1,D1,R4,D12,N,R1,D1,R3,D11,N,C1,D1,L1,D9,N,D1,L3,D1,L1,D9,N,L1,D1,L3,D8,N,R1,D1,C1,D1,C2,D1,C2,L1,D1,L2,R3,D1,R4,D5,N,D1,C1,R2,D1,R1,D11,N,R1,D1,R2,D1,C3,D1,L1,D1,C1,R1,D9,N,D1,C3,R1,D12,N,D2,R2,D1,R1,D10,N,C1,D1,C1,D1,C3,D11,N,D1,L1,D11,N,D1,R1,C1,D1,C2,R1,D11,N,D1,R1,D12,N,D1,R3,D1,R2,D11,N,D2,L4,D12,N,C1,D1,L1,D12,N,L1,D1,L3,D11,N,C1,R2,D1,R2,D13,N,D1,R1,D1,C1,D1,C1,L2,D10,N,R2,D15,N,D14,N,D2,R1,D12,N,R1,C1,D1,R2,C2,D1,R1,C1,D12,N,D1,L1,C2,D1,C1,L1,D13,N,D1,C1,D1,R3,D14,N,D1,L2,C2,D1,C1,L2,D1,R3,C1,D1,C2,R1,D11,N,D1,L3,D14,N,D1,C1,L2,D1,L1,D11,N,D1,C3,D1,C2,D1,L1,D11,N,D1,R2,D13,N,R1,D1,R1,L1,D1,L4,D9,N,R1,D1,C1,D1,C1,D2,C1,D1,L1,D8,N,D3,R1,D9,N,D2,L1,D9,N,D1,R1,D9,N,R1,D1,R1,C1,R3,D13,N,D1,C2,D1,C1,R3,D1,R2,D11,N,D1,C2,D1,C1,R3,D12,N,D1,R5,D12,N,R2,D1,L2,D1,L1,D10,N,D1,C2,D1,L1,D1,L1,D1,C2,D1,L2,D7,N,D1,R2,D1,R1,D9,N,D1,R1,D9,N,D1,R1,D2,L2,R1,D1,R2,D6,N,D1,R5,D9,N,D1,C2,D1,C1,D1,C1,R1,D1,R1,C3,R1,D1,R2,D3,N,D1,L1,C2,D1,C1,L1,C1,D9,N,D1,L2,C1,D1,R2,D8,N,D1,C2,D1,L1,C3,L1,D1,L1,D8,N,D1,C1,D1,R1,C1,R1,D8,N,C1,D1,L4,D8,N,D1,C1,L2,D9,N,D1,R4,D1,R1,D9,N,D1,L1,D1,R1,C1,R1,D1,R1,D8,N,D1,C1,D1,C2,L1,D10,N,D1,L2,D14,N,D1,R1,L1,D1,R1,D12,N,D2,C1,L2,D1,C1,L1,D12,N,L1,D13,N,D1,C2,D1,C2,D1,R3,C1,D1,R1,L1,D2,C3,R1,D8,N,D1,R1,C2,D1,C2,L3,D1,L2,D11,N,C1,D1,L1,C1,L2,D12,N,D1,L1,D2,L3,D8,N,L2,D1,L2,D8,N,D1,R1,C1,D1,C3,D1,L1,D1,C2,D1,R3,D9,N,D1,C1,R1,D12,N,R2,D1,L1,D2,R2,D9,N,R3,D1,R2,D12,N,D1,C2,D1,C1,L1,D13,N,D2,R4,D13,N,D1,L1,D13,N,D2,R2,D13,N,R1,D1,R3,D13,N,D1,C1,R1,D13,N,D1,R2,D1,R1,D13,N,D1,L2,D1,L2,D2,R3,C1,D11,N,D1,R1,C2,D1,C2,D1,C1,R2,D1,R1,D11,N,D1,C3,D1,L2,D1,C1,L2,D11,N,D1,R1,D1,R1,D12,N,D1,C1,D1,L1,D1,R1,D12,N,D1,L2,D13,N,D1,R1,C1,D1,R4,D11,N,C1,D1,R3,D12,N,D3,C1,D1,L1,D9,N,D1,C1,D1,C3,D1,R1,D10,N,D1,C1,D12,N,D1,C1,D1,L3,D12,N,D3,C1,R3,D1,R2,L2,D9,N,C1,D1,C2,D1,L1,C2,D11,N,R1,D1,R4,D13,N,C1,D2,R1,D12,N,D1,C1,D1,C4,D2,L2,D1,L2,D9,N,D1,C3,L1,D1,R2,D1,R3,D10,N,D1,C1,D1,C1,L2,D1,L1,D11,N,D1,R1,D12,N,R3,D13,N,D1,R1,D1,L2,D12,N,R1,D1,C1,D1,L2,R2,D1,R2,D11,N,D1,C2,D1,C2,D1,C2,L2,D11,N,D1,R3,C1,D1,C1,L3,D1,L4,D10,N,D1,R1,D12,N,L1,D2,C1,R1,D1,R5,D11,N,D1,C1,R1,D1,R1,D11,N,D1,L1,D1,C2,D1,C2,L2,D9,N,D1,L1,C1,D13,N,R1,D1,R2,C1,D1,R2,D12,N,D1,C1,R2,D1,R1,C1,R1,D1,L1,D11,N,D1,L1,C1,D14,N,D1,L1,D1,R1,D13,N,R2,D1,R2,D15,N,D1,R1,C1,D1,C2,R1,D1,R2,D11,N,D1,C2,D1,L3,D13,N,D1,L1,D14,N,D1,R1,C1,D1,R2,D13,N,D1,L1,R1,D14,N,D1,C1,L1,D13,N,D1,C1,L1,C1,D1,C1,R3,D1,R3,D9,N,D1,C1,R1,D1,R2,D12,N,D1,R1,D12,N,D1,C1,D11,N,D1,R1,D1,R2,D9,N,D1,C1,D1,C2,D1,R3,D1,L3,D6,N,C1,D1,R1,D8,N,D1,L4,D13,N,D1,L1,D1,L1,D12,N,D1,C1,D1,C2,R2,D1,C1,R2,D11,N,L2,D1,C2,D1,L2,D11,N,L1,D1,L1,R2,D1,R3,D9,N,D1,C1,D1,C1,L2,D10,N,R1,D1,C2,D1,C1,R1,D9,N,D1,C1,D1,R5,D8,N,D1,L1,D11,N,D1,L3,D1,L1,D9,N,D1,L2,D1,R2,D2,R4,D6,N,D1,R1,C1,D11,N,C1,D1,L1,D1,C1,L1,D9,N,D1,C2,D1,R3,D9,N,D1,L1,D1,C1,L3,D9,N,D1,C1,L1,C1,D1,C1,D1,C1,L2,D6,N,D11,N,D1,L1,D1,R1,C1,R2,D8,N,C1,D1,C1,D1,C1,L1,D1,C2,D2,C1,R3,D4,N,D1,C1,L1,D9,N,D1,R3,D1,R2,D10,N,D1,L1,R2,D1,C1,L1,C1,D1,L3,D8,N,D2,C1,D1,L1,D9,N,D2,R4,D1,L4,D1,L1,D6,N,D1,L2,D1,R2,D10,N,D1,R2,C1,D1,C1,L2,D1,L4,D6,N,D1,R1,D1,R2,C1,R2,D10,N,D1,R2,C2,D1,C1,L2,D9,N,R1,D1,R1,D1,C1,R2,D10,N,L1,D1,L3,D9,N,D1,R1,D1,C1,L1,C1,D1,L1,D7,N,D1,C1,R1,D1,R1,D10,N,D1,C1,D1,C1,D1,C1,R4,D1,R1,D8,N,D1,R1,D1,R1,C1,R1,D1,R1,D8,N,D1,R2,D1,R2,D7,N,D1,R1,D9,N,D1,R1,D1,R1,D8,N,C1,D1,L2,D9,N,D1,C1,R1,D10,N,D1,C2,D1,C2,D1,L2,D7,N,D1,C1,L1,D7,N,D1,L1,D1,L3,D8,N,D1,R1,C1,D1,L2,D5,N,D1,C1,R2,D1,C1,L1,D7,N,D1,R2,C1,D1,L1,D5,N,R1,D1,R1,C1,D1,C1,R1,C1,D8,L1,N,L1,D6,N,D1,R2,D1,R2,C1,D8,N,L1,D1,L3,D9,N,R1,D1,R1,L1,D1,L4,D5,N,R1,D1,R2,D1,R1,D1,C1,L2,D1,C1,L2,D3,N,R3,D10,N,D1,R2,C1,D1,R1,D7,N,D1,R3,D1,R2,D6,N,R3,D7,N,D1,C1,R1,D1,R1,D4,N,D1,C1,L1,D1,L2,D4,N,R1,D1,R1,D1,R1,D4,N,D1,R4,D1,R1,D4,N,D1,L1,D5,N,D2,R1,D5,N,D1,L1,D1,L2,D3,N,D1,C1,D1,R2,D3,N,C2,D1,L2,D4,N,R1,D1,R2,D5,N,D1,L2,D1,L1,D2,N,D1,R3,D1,R1,D2,N,D1,R2,D3,N,D5,N,D1,C3,D5,N,C2,D1,C1,L2,D4,N,D1,R1,D4,N,C1,D1,L3,D4,N,D2,R4,D1,L1,D3,N,D2,L1,C1,D1,L2,D1,N,D1,C1,L1,D3,N,R2,D1,R1,D1,C1,R2,D4,N,D1,C1,D1,R5,D4,N,D1,R1,C2,D1,R1,C1,D4,N,D1,R1,C1,D5,N,D1,C2,D1,C1,D3,N,D1,R3,D1,C1,D3,N,D1,C1,L2,D3,N,D1,L1,D2,N,D1,N";
    if (run_game_string(&game, ops, strlen(ops)) == false) {
        printf("run failed\n");
    } else {
        printf("run success\n");
    }
    print_game(&game);
    SEARCH_CONFIG_T config = {
            0.510066,
            0.760666,
            0.35663,
            0.184483,
            0.1,
            0.5,
            2,
            MAX_BRICK_COUNT - 10,
            120,
    };
    normalize_weights(&config);
    bf_game(config);
}
