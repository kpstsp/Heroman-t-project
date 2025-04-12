#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sqlite3.h>

// Game states
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_INVENTORY,
    GAME_STATE_QUESTS,
    GAME_STATE_EXIT
} GameState;

// Player stats
typedef struct {
    int health;
    int experience;
    int level;
    int gold;
    int strength;
    int intelligence;
    int constitution;
    int perception;
} PlayerStats;

// Task structure
typedef struct {
    int id;
    char title[256];
    char description[512];
    int difficulty;  // 1-5 scale
    int type;        // 0: habit, 1: daily, 2: todo
    int completed;
    int streak;
    time_t last_completed;
} Task;

// Game structure
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    sqlite3* db;
    GameState state;
    PlayerStats player;
    Task* tasks;
    int task_count;
    int max_tasks;
} Game;

// Function declarations
Game* game_init(void);
void game_cleanup(Game* game);
void game_run(Game* game);
void game_render(Game* game);
void game_handle_input(Game* game, SDL_Event* event);

#endif // GAME_H 