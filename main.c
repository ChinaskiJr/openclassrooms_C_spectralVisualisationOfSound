#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <FMOD/fmod.h>
#include <FMOD/fmod_errors.h>

#define WINDOW_WEIGHT   800
#define WINDOW_HEIGHT   600

int main(int argc, char *argv[]) {

    int exitStatus = EXIT_FAILURE;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Rect screen = {0, 0, WINDOW_WEIGHT, WINDOW_HEIGHT};

    FMOD_RESULT result = 0;
    FMOD_SYSTEM *system = NULL;

    // SDL Initialization & renderer & window & black screen 
    if (0 != SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL_Init Error : %s\n", SDL_GetError());
        goto QuitSDL;
    }
    
    window = SDL_CreateWindow("Spectral Visualisation of Chinaski", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WEIGHT, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error : %s\n", SDL_GetError());
        goto QuitSDL;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer Error : %s\n", SDL_GetError());
        goto QuitSDL;
    }

    if (0 != SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255)) {
        fprintf(stderr, "SDL_SetRenderDrawColor Error : %s\n", SDL_GetError());
        goto QuitSDL;
    }
    
    if (0 != SDL_RenderClear(renderer)) {
        fprintf(stderr, "SDL_RenderClear Error : %s\n", SDL_GetError());
        goto QuitSDL;
    }


    SDL_RenderPresent(renderer);


    // FMOD's object creation & initialisation
    result = FMOD_System_Create(&system);
    if (result != FMOD_OK) {
        fprintf(stderr, "FMOD_System_Create Error : %s\n", FMOD_ErrorString(result));
        goto QuitFMOD;
    }

    result = FMOD_System_Init(system, 10, FMOD_INIT_NORMAL, NULL); 
    if (result != FMOD_OK) {
        fprintf(stderr, "FMOD_System_Init Error : %s\n", FMOD_ErrorString(result));
        goto QuitFMOD;
    }

    
    SDL_Delay(4000);
    
    exitStatus = EXIT_SUCCESS;

QuitFMOD:
    FMOD_System_Close(system);
    FMOD_System_Release(system);

QuitSDL:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    fprintf(stderr, "%d\n", exitStatus);
    return exitStatus;
}

