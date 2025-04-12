#ifndef SPRITES_H
#define SPRITES_H

#include <SDL2/SDL.h>

// Sprite types
typedef enum {
    SPRITE_BACKGROUND,
    SPRITE_TASK_DIALOG,
    SPRITE_BUTTON_NORMAL,
    SPRITE_BUTTON_HOVER,
    SPRITE_BUTTON_PRESSED,
    SPRITE_CHECKBOX_UNCHECKED,
    SPRITE_CHECKBOX_CHECKED,
    SPRITE_ICON_HABIT,
    SPRITE_ICON_DAILY,
    SPRITE_ICON_TODO,
    SPRITE_ICON_DIFFICULTY,
    SPRITE_ICON_EDIT,
    SPRITE_ICON_DELETE,
    SPRITE_COUNT
} SpriteType;

// Sprite structure
typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} Sprite;

// Sprite manager structure
typedef struct {
    Sprite sprites[SPRITE_COUNT];
    SDL_Renderer* renderer;
} SpriteManager;

// Function declarations
int sprite_manager_init(SpriteManager* manager, SDL_Renderer* renderer);
void sprite_manager_cleanup(SpriteManager* manager);
int sprite_manager_load_sprite(SpriteManager* manager, SpriteType type, const char* filename);
void sprite_manager_draw_sprite(SpriteManager* manager, SpriteType type, int x, int y);
void sprite_manager_draw_sprite_scaled(SpriteManager* manager, SpriteType type, int x, int y, int width, int height);

#endif // SPRITES_H 