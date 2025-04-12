#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "ui.h"
#include "tasks.h"
#include "database.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FONT_SIZE 16
#define MESSAGE_DURATION 3000  // 3 seconds

// Button positions and sizes
#define NEW_TASK_BUTTON_X 10
#define NEW_TASK_BUTTON_Y 50
#define NEW_TASK_BUTTON_WIDTH 100
#define NEW_TASK_BUTTON_HEIGHT 30

#define QUIT_BUTTON_X 10
#define QUIT_BUTTON_Y 90
#define QUIT_BUTTON_WIDTH 100
#define QUIT_BUTTON_HEIGHT 30

// Task dialog positions and sizes
#define TASK_DIALOG_X 200
#define TASK_DIALOG_Y 100
#define TASK_DIALOG_WIDTH 400
#define TASK_DIALOG_HEIGHT 400

#define TASK_TITLE_INPUT_X 220
#define TASK_TITLE_INPUT_Y 150
#define TASK_TITLE_INPUT_WIDTH 360
#define TASK_TITLE_INPUT_HEIGHT 30

#define TASK_DESC_INPUT_X 220
#define TASK_DESC_INPUT_Y 200
#define TASK_DESC_INPUT_WIDTH 360
#define TASK_DESC_INPUT_HEIGHT 60

#define TASK_TYPE_X 220
#define TASK_TYPE_Y 280
#define TASK_TYPE_WIDTH 360
#define TASK_TYPE_HEIGHT 30

#define TASK_DIFFICULTY_X 220
#define TASK_DIFFICULTY_Y 330
#define TASK_DIFFICULTY_WIDTH 360
#define TASK_DIFFICULTY_HEIGHT 30

#define TASK_SAVE_BUTTON_X 220
#define TASK_SAVE_BUTTON_Y 380
#define TASK_SAVE_BUTTON_WIDTH 100
#define TASK_SAVE_BUTTON_HEIGHT 30

#define TASK_CANCEL_BUTTON_X 480
#define TASK_CANCEL_BUTTON_Y 380
#define TASK_CANCEL_BUTTON_WIDTH 100
#define TASK_CANCEL_BUTTON_HEIGHT 30

#define TASK_ITEM_WIDTH 700
#define TASK_ITEM_HEIGHT 40
#define TASK_ITEM_SPACING 5
#define TASK_BUTTON_SIZE 30

typedef struct {
    char title[256];
    char description[512];
    int difficulty;
    int type;
    int editing_title;
    int editing_description;
    int cursor_position;
    time_t last_cursor_blink;
    int cursor_visible;
} TaskDialog;

// Task type names
static const char* task_type_names[] = {
    "Habit",
    "Daily",
    "To-Do"
};

// Task difficulty names
static const char* task_difficulty_names[] = {
    "Trivial",
    "Easy",
    "Medium",
    "Hard",
    "Very Hard"
};

typedef struct {
    char text[256];
    time_t show_time;
    int visible;
} Message;

typedef struct {
    Task* tasks;
    int count;
    int capacity;
} TaskList;

void init_task_list(TaskList* list) {
    list->tasks = NULL;
    list->count = 0;
    list->capacity = 0;
}

void free_task_list(TaskList* list) {
    if (list->tasks) {
        free(list->tasks);
        list->tasks = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}

void show_message(Message* msg, const char* text) {
    strncpy(msg->text, text, sizeof(msg->text) - 1);
    msg->text[sizeof(msg->text) - 1] = '\0';
    msg->show_time = time(NULL);
    msg->visible = 1;
}

void update_message(Message* msg) {
    if (msg->visible) {
        time_t now = time(NULL);
        if (now - msg->show_time >= MESSAGE_DURATION / 1000) {
            msg->visible = 0;
        }
    }
}

void draw_message(UI* ui, Message* msg) {
    if (msg->visible) {
        ui_draw_text(ui, msg->text, 10, WINDOW_HEIGHT - 30);
    }
}

void draw_task_list(UI* ui, TaskList* list) {
    if (!list || !list->tasks) return;

    // Draw task list background
    SDL_Rect list_rect = {10, 130, 780, 460};
    SDL_SetRenderDrawColor(ui->renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(ui->renderer, &list_rect);
    SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(ui->renderer, &list_rect);

    // Draw tasks
    for (int i = 0; i < list->count; i++) {
        Task* task = &list->tasks[i];
        int y = 140 + (i * (TASK_ITEM_HEIGHT + TASK_ITEM_SPACING));
        
        // Draw task background
        SDL_Rect task_rect = {15, y, TASK_ITEM_WIDTH, TASK_ITEM_HEIGHT};
        SDL_SetRenderDrawColor(ui->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(ui->renderer, &task_rect);
        SDL_SetRenderDrawColor(ui->renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(ui->renderer, &task_rect);

        // Draw task info
        char task_info[256];
        snprintf(task_info, sizeof(task_info), "%s%s (%s, %s)", 
                task->completed ? "[X] " : "[ ] ",
                task->title,
                task_type_names[task->type],
                task_difficulty_names[task->difficulty]);
        ui_draw_text(ui, task_info, 20, y + 10);

        // Draw edit button
        SDL_Rect edit_rect = {TASK_ITEM_WIDTH + 20, y, TASK_BUTTON_SIZE, TASK_BUTTON_SIZE};
        SDL_SetRenderDrawColor(ui->renderer, 200, 200, 255, 255);
        SDL_RenderFillRect(ui->renderer, &edit_rect);
        SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(ui->renderer, &edit_rect);
        ui_draw_text(ui, "E", TASK_ITEM_WIDTH + 25, y + 10);

        // Draw delete button
        SDL_Rect delete_rect = {TASK_ITEM_WIDTH + 55, y, TASK_BUTTON_SIZE, TASK_BUTTON_SIZE};
        SDL_SetRenderDrawColor(ui->renderer, 255, 200, 200, 255);
        SDL_RenderFillRect(ui->renderer, &delete_rect);
        SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(ui->renderer, &delete_rect);
        ui_draw_text(ui, "D", TASK_ITEM_WIDTH + 60, y + 10);
    }
}

void handle_text_input(TaskDialog* dialog, SDL_Event* event) {
    if (!dialog || !event) return;

    if (dialog->editing_title) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && dialog->cursor_position > 0) {
            dialog->title[--dialog->cursor_position] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_DELETE && dialog->cursor_position < strlen(dialog->title)) {
            memmove(&dialog->title[dialog->cursor_position],
                   &dialog->title[dialog->cursor_position + 1],
                   strlen(dialog->title) - dialog->cursor_position);
        }
        else if (event->key.keysym.sym == SDLK_LEFT && dialog->cursor_position > 0) {
            dialog->cursor_position--;
        }
        else if (event->key.keysym.sym == SDLK_RIGHT && dialog->cursor_position < strlen(dialog->title)) {
            dialog->cursor_position++;
        }
        else if (event->key.keysym.sym == SDLK_HOME) {
            dialog->cursor_position = 0;
        }
        else if (event->key.keysym.sym == SDLK_END) {
            dialog->cursor_position = strlen(dialog->title);
        }
        else if (event->text.text[0] >= 32 && event->text.text[0] <= 126 &&
                 strlen(dialog->title) < 255) {
            memmove(&dialog->title[dialog->cursor_position + 1],
                   &dialog->title[dialog->cursor_position],
                   strlen(dialog->title) - dialog->cursor_position + 1);
            dialog->title[dialog->cursor_position++] = event->text.text[0];
        }
    }
    else if (dialog->editing_description) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && dialog->cursor_position > 0) {
            dialog->description[--dialog->cursor_position] = '\0';
        }
        else if (event->key.keysym.sym == SDLK_DELETE && dialog->cursor_position < strlen(dialog->description)) {
            memmove(&dialog->description[dialog->cursor_position],
                   &dialog->description[dialog->cursor_position + 1],
                   strlen(dialog->description) - dialog->cursor_position);
        }
        else if (event->key.keysym.sym == SDLK_LEFT && dialog->cursor_position > 0) {
            dialog->cursor_position--;
        }
        else if (event->key.keysym.sym == SDLK_RIGHT && dialog->cursor_position < strlen(dialog->description)) {
            dialog->cursor_position++;
        }
        else if (event->key.keysym.sym == SDLK_HOME) {
            dialog->cursor_position = 0;
        }
        else if (event->key.keysym.sym == SDLK_END) {
            dialog->cursor_position = strlen(dialog->description);
        }
        else if (event->text.text[0] >= 32 && event->text.text[0] <= 126 &&
                 strlen(dialog->description) < 511) {
            memmove(&dialog->description[dialog->cursor_position + 1],
                   &dialog->description[dialog->cursor_position],
                   strlen(dialog->description) - dialog->cursor_position + 1);
            dialog->description[dialog->cursor_position++] = event->text.text[0];
        }
    }
}

void draw_cursor(UI* ui, int x, int y, int height, int visible) {
    if (!ui || !visible) return;
    
    SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(ui->renderer, x, y, x, y + height);
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        SDL_Log("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize database
    sqlite3* db;
    if (db_init("habitica.db", &db) != 0) {
        SDL_Log("Failed to initialize database\n");
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Habitica Clone",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         WINDOW_WIDTH,
                                         WINDOW_HEIGHT,
                                         SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        db_close(db);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        db_close(db);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("assets/font.ttf", FONT_SIZE);
    if (!font) {
        SDL_Log("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        db_close(db);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize UI
    UI ui;
    ui_init(&ui, renderer, font);

    // Initialize task dialog
    TaskDialog task_dialog = {0};
    task_dialog.difficulty = 2; // Default to medium difficulty
    task_dialog.type = 0; // Default to habit
    int showing_task_dialog = 0;

    // Initialize message system
    Message message = {0};
    
    // Initialize task list
    TaskList task_list;
    init_task_list(&task_list);

    // Load existing tasks
    if (db_get_all_tasks(db, &task_list.tasks, &task_list.count) == 0) {
        task_list.capacity = task_list.count;
    }

    // Main game loop
    SDL_Event event;
    int running = 1;
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                if (!showing_task_dialog) {
                    // Check main menu buttons
                    if (ui_is_button_clicked(&ui, NEW_TASK_BUTTON_X, NEW_TASK_BUTTON_Y,
                                           NEW_TASK_BUTTON_WIDTH, NEW_TASK_BUTTON_HEIGHT,
                                           mouse_x, mouse_y)) {
                        showing_task_dialog = 1;
                        memset(&task_dialog, 0, sizeof(TaskDialog));
                        task_dialog.difficulty = 2;
                        task_dialog.type = 0;
                    }
                    else if (ui_is_button_clicked(&ui, QUIT_BUTTON_X, QUIT_BUTTON_Y,
                                                QUIT_BUTTON_WIDTH, QUIT_BUTTON_HEIGHT,
                                                mouse_x, mouse_y)) {
                        running = 0;
                    }
                }
                else {
                    // Check task dialog buttons and input fields
                    if (ui_is_button_clicked(&ui, TASK_SAVE_BUTTON_X, TASK_SAVE_BUTTON_Y,
                                           TASK_SAVE_BUTTON_WIDTH, TASK_SAVE_BUTTON_HEIGHT,
                                           mouse_x, mouse_y)) {
                        // Save the task
                        Task task;
                        task_init(&task, task_dialog.title, task_dialog.description,
                                task_dialog.difficulty, task_dialog.type);
                        
                        if (db_create_task(db, &task) == 0) {
                            // Add task to list
                            if (task_list.count >= task_list.capacity) {
                                int new_capacity = task_list.capacity == 0 ? 10 : task_list.capacity * 2;
                                Task* new_tasks = realloc(task_list.tasks, new_capacity * sizeof(Task));
                                if (new_tasks) {
                                    task_list.tasks = new_tasks;
                                    task_list.capacity = new_capacity;
                                }
                            }
                            
                            if (task_list.count < task_list.capacity) {
                                task_list.tasks[task_list.count++] = task;
                                show_message(&message, "Task created successfully!");
                            }
                            else {
                                show_message(&message, "Failed to add task to list!");
                            }
                            
                            showing_task_dialog = 0;
                        }
                        else {
                            show_message(&message, "Failed to save task to database!");
                        }
                    }
                    else if (ui_is_button_clicked(&ui, TASK_CANCEL_BUTTON_X, TASK_CANCEL_BUTTON_Y,
                                                TASK_CANCEL_BUTTON_WIDTH, TASK_CANCEL_BUTTON_HEIGHT,
                                                mouse_x, mouse_y)) {
                        showing_task_dialog = 0;
                    }
                    else if (mouse_x >= TASK_TITLE_INPUT_X && mouse_x <= TASK_TITLE_INPUT_X + TASK_TITLE_INPUT_WIDTH &&
                             mouse_y >= TASK_TITLE_INPUT_Y && mouse_y <= TASK_TITLE_INPUT_Y + TASK_TITLE_INPUT_HEIGHT) {
                        task_dialog.editing_title = 1;
                        task_dialog.editing_description = 0;
                        task_dialog.cursor_position = strlen(task_dialog.title);
                    }
                    else if (mouse_x >= TASK_DESC_INPUT_X && mouse_x <= TASK_DESC_INPUT_X + TASK_DESC_INPUT_WIDTH &&
                             mouse_y >= TASK_DESC_INPUT_Y && mouse_y <= TASK_DESC_INPUT_Y + TASK_DESC_INPUT_HEIGHT) {
                        task_dialog.editing_title = 0;
                        task_dialog.editing_description = 1;
                        task_dialog.cursor_position = strlen(task_dialog.description);
                    }
                    else if (mouse_x >= TASK_TYPE_X && mouse_x <= TASK_TYPE_X + TASK_TYPE_WIDTH &&
                             mouse_y >= TASK_TYPE_Y && mouse_y <= TASK_TYPE_Y + TASK_TYPE_HEIGHT) {
                        task_dialog.type = (task_dialog.type + 1) % 3;
                    }
                    else if (mouse_x >= TASK_DIFFICULTY_X && mouse_x <= TASK_DIFFICULTY_X + TASK_DIFFICULTY_WIDTH &&
                             mouse_y >= TASK_DIFFICULTY_Y && mouse_y <= TASK_DIFFICULTY_Y + TASK_DIFFICULTY_HEIGHT) {
                        task_dialog.difficulty = (task_dialog.difficulty + 1) % 5;
                    }
                    else {
                        task_dialog.editing_title = 0;
                        task_dialog.editing_description = 0;
                    }
                }
            }
            else if (event.type == SDL_TEXTINPUT && (task_dialog.editing_title || task_dialog.editing_description)) {
                handle_text_input(&task_dialog, &event);
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_TAB) {
                    if (task_dialog.editing_title) {
                        task_dialog.editing_title = 0;
                        task_dialog.editing_description = 1;
                        task_dialog.cursor_position = strlen(task_dialog.description);
                    }
                    else if (task_dialog.editing_description) {
                        task_dialog.editing_title = 1;
                        task_dialog.editing_description = 0;
                        task_dialog.cursor_position = strlen(task_dialog.title);
                    }
                }
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    if (task_dialog.editing_title) {
                        task_dialog.editing_title = 0;
                        task_dialog.editing_description = 1;
                        task_dialog.cursor_position = strlen(task_dialog.description);
                    }
                    else if (task_dialog.editing_description) {
                        task_dialog.editing_description = 0;
                    }
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (task_dialog.editing_title || task_dialog.editing_description) {
                        task_dialog.editing_title = 0;
                        task_dialog.editing_description = 0;
                    }
                    else {
                        showing_task_dialog = 0;
                    }
                }
            }

            if (!showing_task_dialog) {
                // Check task list interactions
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mouse_x, mouse_y;
                    SDL_GetMouseState(&mouse_x, &mouse_y);

                    // Check if clicking on a task
                    for (int i = 0; i < task_list.count; i++) {
                        int y = 140 + (i * (TASK_ITEM_HEIGHT + TASK_ITEM_SPACING));
                        SDL_Rect task_rect = {15, y, TASK_ITEM_WIDTH, TASK_ITEM_HEIGHT};
                        
                        if (mouse_x >= task_rect.x && mouse_x <= task_rect.x + task_rect.w &&
                            mouse_y >= task_rect.y && mouse_y <= task_rect.y + task_rect.h) {
                            // Toggle task completion
                            task_list.tasks[i].completed = !task_list.tasks[i].completed;
                            if (db_update_task(db, &task_list.tasks[i]) == 0) {
                                show_message(&message, task_list.tasks[i].completed ? 
                                           "Task completed!" : "Task uncompleted!");
                            }
                            else {
                                show_message(&message, "Failed to update task!");
                            }
                        }

                        // Check edit button
                        SDL_Rect edit_rect = {TASK_ITEM_WIDTH + 20, y, TASK_BUTTON_SIZE, TASK_BUTTON_SIZE};
                        if (mouse_x >= edit_rect.x && mouse_x <= edit_rect.x + edit_rect.w &&
                            mouse_y >= edit_rect.y && mouse_y <= edit_rect.y + edit_rect.h) {
                            // Open edit dialog
                            showing_task_dialog = 1;
                            strncpy(task_dialog.title, task_list.tasks[i].title, sizeof(task_dialog.title) - 1);
                            strncpy(task_dialog.description, task_list.tasks[i].description, sizeof(task_dialog.description) - 1);
                            task_dialog.difficulty = task_list.tasks[i].difficulty;
                            task_dialog.type = task_list.tasks[i].type;
                            task_dialog.editing_title = 0;
                            task_dialog.editing_description = 0;
                            task_dialog.cursor_position = 0;
                        }

                        // Check delete button
                        SDL_Rect delete_rect = {TASK_ITEM_WIDTH + 55, y, TASK_BUTTON_SIZE, TASK_BUTTON_SIZE};
                        if (mouse_x >= delete_rect.x && mouse_x <= delete_rect.x + delete_rect.w &&
                            mouse_y >= delete_rect.y && mouse_y <= delete_rect.y + delete_rect.h) {
                            // Delete task
                            if (db_delete_task(db, task_list.tasks[i].id) == 0) {
                                // Remove from list
                                memmove(&task_list.tasks[i], &task_list.tasks[i + 1],
                                       (task_list.count - i - 1) * sizeof(Task));
                                task_list.count--;
                                show_message(&message, "Task deleted!");
                            }
                            else {
                                show_message(&message, "Failed to delete task!");
                            }
                        }
                    }
                }
            }
        }

        // Update cursor blink
        time_t now = time(NULL);
        if (now - task_dialog.last_cursor_blink >= 500) {
            task_dialog.cursor_visible = !task_dialog.cursor_visible;
            task_dialog.last_cursor_blink = now;
        }

        // Update message visibility
        update_message(&message);

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw UI elements
        ui_draw_text(&ui, "Habitica Clone", 10, 10);
        
        if (!showing_task_dialog) {
            // Draw main menu
            ui_draw_button(&ui, "New Task", NEW_TASK_BUTTON_X, NEW_TASK_BUTTON_Y,
                          NEW_TASK_BUTTON_WIDTH, NEW_TASK_BUTTON_HEIGHT);
            ui_draw_button(&ui, "Quit", QUIT_BUTTON_X, QUIT_BUTTON_Y,
                          QUIT_BUTTON_WIDTH, QUIT_BUTTON_HEIGHT);

            // Draw task list
            draw_task_list(&ui, &task_list);
        }
        else {
            // Draw task dialog
            SDL_Rect dialog_rect = {TASK_DIALOG_X, TASK_DIALOG_Y,
                                  TASK_DIALOG_WIDTH, TASK_DIALOG_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &dialog_rect);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &dialog_rect);

            ui_draw_text(&ui, "New Task", TASK_DIALOG_X + 10, TASK_DIALOG_Y + 10);
            
            // Draw title input
            ui_draw_text(&ui, "Title:", TASK_TITLE_INPUT_X, TASK_TITLE_INPUT_Y - 20);
            SDL_Rect title_rect = {TASK_TITLE_INPUT_X, TASK_TITLE_INPUT_Y,
                                 TASK_TITLE_INPUT_WIDTH, TASK_TITLE_INPUT_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &title_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &title_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            ui_draw_text(&ui, task_dialog.title, TASK_TITLE_INPUT_X + 5, TASK_TITLE_INPUT_Y + 5);
            if (task_dialog.editing_title) {
                int cursor_x = TASK_TITLE_INPUT_X + 5;
                if (task_dialog.cursor_position > 0) {
                    char temp[256] = {0};
                    strncpy(temp, task_dialog.title, task_dialog.cursor_position);
                    int w, h;
                    TTF_SizeText(font, temp, &w, &h);
                    cursor_x += w;
                }
                draw_cursor(&ui, cursor_x, TASK_TITLE_INPUT_Y + 5, TASK_TITLE_INPUT_HEIGHT - 10,
                           task_dialog.cursor_visible);
            }

            // Draw description input
            ui_draw_text(&ui, "Description:", TASK_DESC_INPUT_X, TASK_DESC_INPUT_Y - 20);
            SDL_Rect desc_rect = {TASK_DESC_INPUT_X, TASK_DESC_INPUT_Y,
                                TASK_DESC_INPUT_WIDTH, TASK_DESC_INPUT_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &desc_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &desc_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            ui_draw_text(&ui, task_dialog.description, TASK_DESC_INPUT_X + 5, TASK_DESC_INPUT_Y + 5);
            if (task_dialog.editing_description) {
                int cursor_x = TASK_DESC_INPUT_X + 5;
                if (task_dialog.cursor_position > 0) {
                    char temp[512] = {0};
                    strncpy(temp, task_dialog.description, task_dialog.cursor_position);
                    int w, h;
                    TTF_SizeText(font, temp, &w, &h);
                    cursor_x += w;
                }
                draw_cursor(&ui, cursor_x, TASK_DESC_INPUT_Y + 5, TASK_DESC_INPUT_HEIGHT - 10,
                           task_dialog.cursor_visible);
            }

            // Draw type selection
            ui_draw_text(&ui, "Type:", TASK_TYPE_X, TASK_TYPE_Y - 20);
            SDL_Rect type_rect = {TASK_TYPE_X, TASK_TYPE_Y,
                                TASK_TYPE_WIDTH, TASK_TYPE_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &type_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &type_rect);
            ui_draw_text(&ui, task_type_names[task_dialog.type], TASK_TYPE_X + 5, TASK_TYPE_Y + 5);

            // Draw difficulty selection
            ui_draw_text(&ui, "Difficulty:", TASK_DIFFICULTY_X, TASK_DIFFICULTY_Y - 20);
            SDL_Rect diff_rect = {TASK_DIFFICULTY_X, TASK_DIFFICULTY_Y,
                                TASK_DIFFICULTY_WIDTH, TASK_DIFFICULTY_HEIGHT};
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &diff_rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &diff_rect);
            ui_draw_text(&ui, task_difficulty_names[task_dialog.difficulty],
                        TASK_DIFFICULTY_X + 5, TASK_DIFFICULTY_Y + 5);

            // Draw buttons
            ui_draw_button(&ui, "Save", TASK_SAVE_BUTTON_X, TASK_SAVE_BUTTON_Y,
                          TASK_SAVE_BUTTON_WIDTH, TASK_SAVE_BUTTON_HEIGHT);
            ui_draw_button(&ui, "Cancel", TASK_CANCEL_BUTTON_X, TASK_CANCEL_BUTTON_Y,
                          TASK_CANCEL_BUTTON_WIDTH, TASK_CANCEL_BUTTON_HEIGHT);
        }

        // Draw message
        draw_message(&ui, &message);

        // Update screen
        SDL_RenderPresent(renderer);

        // Cap at 60 FPS
        SDL_Delay(16);
    }

    // Cleanup
    free_task_list(&task_list);
    db_close(db);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
} 