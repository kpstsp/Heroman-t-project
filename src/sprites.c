#include "sprites.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

int sprite_manager_init(SpriteManager* manager, SDL_Renderer* renderer) {
    if (!manager || !renderer) return 1;
    
    memset(manager, 0, sizeof(SpriteManager));
    manager->renderer = renderer;
    return 0;
}

void sprite_manager_cleanup(SpriteManager* manager) {
    if (!manager) return;
    
    for (int i = 0; i < SPRITE_COUNT; i++) {
        if (manager->sprites[i].texture) {
            SDL_DestroyTexture(manager->sprites[i].texture);
            manager->sprites[i].texture = NULL;
        }
    }
}

int sprite_manager_load_sprite(SpriteManager* manager, SpriteType type, const char* filename) {
    if (!manager || !filename) return 1;
    
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        fprintf(stderr, "Failed to load sprite %s: %s\n", filename, IMG_GetError());
        return 1;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(manager->renderer, surface);
    if (!texture) {
        fprintf(stderr, "Failed to create texture from %s: %s\n", filename, SDL_GetError());
        SDL_FreeSurface(surface);
        return 1;
    }
    
    manager->sprites[type].texture = texture;
    manager->sprites[type].width = surface->w;
    manager->sprites[type].height = surface->h;
    
    SDL_FreeSurface(surface);
    return 0;
}

void sprite_manager_draw_sprite(SpriteManager* manager, SpriteType type, int x, int y) {
    if (!manager || !manager->sprites[type].texture) return;
    
    SDL_Rect dest = {
        .x = x,
        .y = y,
        .w = manager->sprites[type].width,
        .h = manager->sprites[type].height
    };
    
    SDL_RenderCopy(manager->renderer, manager->sprites[type].texture, NULL, &dest);
}

void sprite_manager_draw_sprite_scaled(SpriteManager* manager, SpriteType type, int x, int y, int width, int height) {
    if (!manager || !manager->sprites[type].texture) return;
    
    SDL_Rect dest = {
        .x = x,
        .y = y,
        .w = width,
        .h = height
    };
    
    SDL_RenderCopy(manager->renderer, manager->sprites[type].texture, NULL, &dest);
} 