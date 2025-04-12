#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color text_color;
} UI;

void ui_init(UI* ui, SDL_Renderer* renderer, TTF_Font* font);
void ui_draw_text(UI* ui, const char* text, int x, int y);
void ui_draw_button(UI* ui, const char* text, int x, int y, int width, int height);
int ui_is_button_clicked(UI* ui, int x, int y, int width, int height, int mouse_x, int mouse_y);

#endif // UI_H 