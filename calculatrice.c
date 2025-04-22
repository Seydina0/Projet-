#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WIDTH 400
#define HEIGHT 600

typedef struct {
    SDL_Rect rect;
    char label[4];
} Button;

const char* buttons[] = {
    "7", "8", "9", "/",
    "4", "5", "6", "*",
    "1", "2", "3", "-",
    "0", "C", "=", "+"
};

Button buttonGrid[16];
char expression[256] = "";

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        printf("Erreur TTF_RenderText_Solid : %s\n", TTF_GetError());
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Erreur SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

double eval(const char* expr) {
    double a, b;
    char op;
    sscanf(expr, "%lf %c %lf", &a, &op, &b);
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0;
        default: return 0;
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Calculatrice SDL2",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (!window) {
        printf("Erreur SDL_CreateWindow : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur SDL_CreateRenderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
    if (!font) {
        printf("Erreur chargement police : %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialisation des boutons
    int bw = WIDTH / 4;
    int bh = (HEIGHT - 100) / 4;
    for (int i = 0; i < 16; i++) {
        buttonGrid[i].rect.x = (i % 4) * bw;
        buttonGrid[i].rect.y = 100 + (i / 4) * bh;
        buttonGrid[i].rect.w = bw;
        buttonGrid[i].rect.h = bh;
        strncpy(buttonGrid[i].label, buttons[i], sizeof(buttonGrid[i].label) - 1);
        buttonGrid[i].label[sizeof(buttonGrid[i].label) - 1] = '\0'; // Assurez-vous que la chaîne est terminée
    }

    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;
                for (int i = 0; i < 16; i++) {
                    SDL_Rect r = buttonGrid[i].rect;
                    if (mx > r.x && mx < r.x + r.w && my > r.y && my < r.y + r.h) {
                        const char* label = buttonGrid[i].label;
                        if (strcmp(label, "C") == 0) {
                            expression[0] = '\0';
                        } else if (strcmp(label, "=") == 0) {
                            double res = eval(expression);
                            sprintf(expression, "%.2f", res);
                        } else {
                            if (strlen(expression) + strlen(label) + 2 < sizeof(expression)) {
                                strcat(expression, " ");
                                strcat(expression, label);
                            } else {
                                printf("Expression trop longue, impossible d'ajouter : %s\n", label);
                            }
                        }
                    }
                }
            }
        }

        // Rendu
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Zone d'affichage
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_Rect display = {0, 0, WIDTH, 100};
        SDL_RenderFillRect(renderer, &display);
        renderText(renderer, font, expression, 10, 40);

        // Dessin des boutons
        for (int i = 0; i < 16; i++) {
            SDL_SetRenderDrawColor(renderer, 70, 70, 200, 255);
            SDL_RenderFillRect(renderer, &buttonGrid[i].rect);
            renderText(renderer, font, buttonGrid[i].label,
                       buttonGrid[i].rect.x + 30, buttonGrid[i].rect.y + 30);
        }

        SDL_RenderPresent(renderer);
    }

    // Nettoyage
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
