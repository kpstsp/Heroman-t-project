#include "ui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

void ui_init(UI* ui, SDL_Renderer* renderer, TTF_Font* font) {
    if (!ui || !renderer || !font) return;
    
    ui->renderer = renderer;
    ui->font = font;
}

void ui_draw_text(UI* ui, const char* text, int x, int y) {
    if (!ui || !text || !*text) return;  // Skip empty strings

    SDL_Color color = {0, 0, 0, 255};  // Black color
    SDL_Surface* surface = TTF_RenderText_Solid(ui->font, text, color);
    if (!surface) {
        printf("Failed to render text: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(ui->renderer, surface);
    if (!texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(ui->renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void ui_draw_button(UI* ui, const char* text, int x, int y, int width, int height) {
    if (!ui) return;

    // Draw button background
    SDL_Rect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(ui->renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(ui->renderer, &rect);
    SDL_SetRenderDrawColor(ui->renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(ui->renderer, &rect);

    // Draw button text
    if (text && *text) {  // Only draw if text is not empty
        SDL_Color color = {0, 0, 0, 255};  // Black color
        SDL_Surface* surface = TTF_RenderText_Solid(ui->font, text, color);
        if (!surface) {
            printf("Failed to render text: %s\n", TTF_GetError());
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(ui->renderer, surface);
        if (!texture) {
            printf("Failed to create texture: %s\n", SDL_GetError());
            SDL_FreeSurface(surface);
            return;
        }

        // Center text in button
        int text_x = x + (width - surface->w) / 2;
        int text_y = y + (height - surface->h) / 2;
        SDL_Rect text_rect = {text_x, text_y, surface->w, surface->h};
        SDL_RenderCopy(ui->renderer, texture, NULL, &text_rect);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}

int ui_is_button_clicked(UI* ui, int x, int y, int width, int height, int mouse_x, int mouse_y) {
    return (mouse_x >= x && mouse_x <= x + width &&
            mouse_y >= y && mouse_y <= y + height);
} 