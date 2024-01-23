#include <SDL2/SDL.h>
#include <stdio.h>

SDL_Renderer* renderer;

float getYposition(float x1, float y1, float x2, float y2, float x) {
    float m = (y2-y1)/(x2-x1);
    float b = y2-m*x2;
    return m*x+b;
}

SDL_Point getLineCrossPoint(float line1x1, float line1y1, float line1x2, float line1y2, float line2x1, float line2y1, float line2x2, float line2y2) {
    float m1 = (line1y2-line1y1)/(line1x2-line1x1);
    float m2 = (line2y2-line2y1)/(line2x2-line2x1);
    float xCross = ((line2y1-line2x1*m2)-(line1y1-line1x1*m1))/(m1-m2);
    float yCross = m1*xCross + line1y1 - line1x1*m1;
    SDL_Point point;
    point.x = (int)xCross;
    point.y = (int)yCross;
    return point;
}

void plotRays(SDL_Point point1) {
    // parallel ray refracts to focal point
    SDL_RenderDrawLine(renderer, point1.x, point1.y, 320, point1.y);
    SDL_RenderDrawLine(renderer, 320, point1.y, 1279, getYposition(320, point1.y, 480, 240, 1279));
    // ray through focal point refracts parallel
    SDL_RenderDrawLine(renderer, point1.x, point1.y, 320, getYposition(point1.x, point1.y, 160, 240, 320));
    SDL_RenderDrawLine(renderer, 320, getYposition(point1.x, point1.y, 160, 240, 320), 1279, getYposition(point1.x, point1.y, 160, 240, 320));
    // ray through optical centre passes straight through
    SDL_RenderDrawLine(renderer, point1.x, point1.y, 1279, getYposition(point1.x, point1.y, 320, 240, 1279));
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialize\n");
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Lens simulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 480, 0);
    if (window == NULL) {
        fprintf(stderr, "SDL failed to create window\n");
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "SDL failed to create renderer\n");
        return 1;
    }

    SDL_Point point1;
    SDL_Point point2;
    point2.x = 10;
    point2.y = 10;

    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    point1.x = event.button.x;
                    point1.y = event.button.y;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    point2.x = event.button.x;
                    point2.y = event.button.y;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);
        SDL_RenderClear(renderer);
        // draw object
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, point1.x, point1.y, point2.x, point2.y);
        // principal and optical axis
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, 320, 0, 320, 479);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, 0, 240, 1279, 240);
        // rays
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        plotRays(point1);
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        plotRays(point2);
        // find points of convergence
        SDL_Point imagePoint1 = getLineCrossPoint(point1.x, point1.y, 320, 240, 320, point1.y, 480, 240);
        SDL_Point imagePoint2 = getLineCrossPoint(point2.x, point2.y, 320, 240, 320, point2.y, 480, 240);
        // draw image
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderDrawLine(renderer, imagePoint1.x, imagePoint1.y, imagePoint2.x, imagePoint2.y);
        SDL_RenderPresent(renderer);
    }
}

