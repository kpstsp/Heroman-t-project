#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "database.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FONT_SIZE 16

Game* game_init(void) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) {
        return NULL;
    }

    // Initialize SDL window and renderer
    game->window = SDL_CreateWindow("Heroman project",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WINDOW_WIDTH,
                                   WINDOW_HEIGHT,
                                   SDL_WINDOW_SHOWN);
    if (!game->window) {
        free(game);
        return NULL;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        SDL_DestroyWindow(game->window);
        free(game);
        return NULL;
    }

    // Initialize font
    game->font = TTF_OpenFont("assets/font.ttf", FONT_SIZE);
    if (!game->font) {
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        free(game);
        return NULL;
    }

    // Initialize database
    if (db_init("heroman.db", &game->db) != 0) {
        TTF_CloseFont(game->font);
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        free(game);
        return NULL;
    }

    // Create database schema
    if (db_create_schema(game->db) != 0) {
        db_close(game->db);
        TTF_CloseFont(game->font);
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        free(game);
        return NULL;
    }

    // Initialize game state
    game->state = GAME_STATE_MENU;
    game->task_count = 0;
    game->max_tasks = 10;
    game->tasks = (Task*)malloc(sizeof(Task) * game->max_tasks);
    if (!game->tasks) {
        db_close(game->db);
        TTF_CloseFont(game->font);
        SDL_DestroyRenderer(game->renderer);
        SDL_DestroyWindow(game->window);
        free(game);
        return NULL;
    }

    // Load player data
    if (db_load_player(game->db, &game->player) != 0) {
        // Initialize default player stats if no save exists
        game->player.health = 50;
        game->player.experience = 0;
        game->player.level = 1;
        game->player.gold = 0;
        game->player.strength = 1;
        game->player.intelligence = 1;
        game->player.constitution = 1;
        game->player.perception = 1;
        db_save_player(game->db, &game->player);
    }

    return game;
}

void game_cleanup(Game* game) {
    if (!game) return;

    free(game->tasks);
    db_close(game->db);
    TTF_CloseFont(game->font);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    free(game);
}

void game_render(Game* game) {
    if (!game) return;

    // Clear screen
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    // Render based on game state
    switch (game->state) {
        case GAME_STATE_MENU:
            // TODO: Render menu
            break;
        case GAME_STATE_PLAYING:
            // TODO: Render main game screen
            break;
        case GAME_STATE_INVENTORY:
            // TODO: Render inventory
            break;
        case GAME_STATE_QUESTS:
            // TODO: Render quests
            break;
        default:
            break;
    }

    SDL_RenderPresent(game->renderer);
}

void game_handle_input(Game* game, SDL_Event* event) {
    if (!game || !event) return;

    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    if (game->state == GAME_STATE_PLAYING) {
                        game->state = GAME_STATE_MENU;
                    }
                    break;
                // Add more key handling as needed
            }
            break;
        // Add more event handling as needed
    }
}

void game_filter_tasks(Game* game, TaskFilter filter) {
    if (!game) return;
    
    game->current_filter = filter;
    
    // Reload all tasks from database
    Task* all_tasks = NULL;
    int all_count = 0;
    if (db_get_all_tasks(game->db, &all_tasks, &all_count) != 0) {
        return;
    }
    
    // Filter tasks
    int filtered_count = 0;
    for (int i = 0; i < all_count; i++) {
        if (filter == TASK_FILTER_ALL ||
            (filter == TASK_FILTER_COMPLETED && all_tasks[i].completed) ||
            (filter == TASK_FILTER_UNCOMPLETED && !all_tasks[i].completed)) {
            if (filtered_count < game->max_tasks) {
                game->tasks[filtered_count++] = all_tasks[i];
            }
        }
    }
    
    game->task_count = filtered_count;
    free(all_tasks);
    
    // Re-sort the filtered tasks
    game_sort_tasks(game, game->current_sort);
}

void game_sort_tasks(Game* game, TaskSort sort) {
    if (!game) return;
    
    game->current_sort = sort;
    
    // Simple bubble sort for demonstration
    for (int i = 0; i < game->task_count - 1; i++) {
        for (int j = 0; j < game->task_count - i - 1; j++) {
            int should_swap = 0;
            
            switch (sort) {
                case TASK_SORT_TYPE:
                    should_swap = game->tasks[j].type > game->tasks[j + 1].type;
                    break;
                case TASK_SORT_DIFFICULTY:
                    should_swap = game->tasks[j].difficulty > game->tasks[j + 1].difficulty;
                    break;
                case TASK_SORT_COMPLETION:
                    should_swap = game->tasks[j].completed && !game->tasks[j + 1].completed;
                    break;
            }
            
            if (should_swap) {
                Task temp = game->tasks[j];
                game->tasks[j] = game->tasks[j + 1];
                game->tasks[j + 1] = temp;
            }
        }
    }
} 