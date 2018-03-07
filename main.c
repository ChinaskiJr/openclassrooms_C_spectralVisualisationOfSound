/* SPECTRAL VISUALISATION OF CHINASKI'S MP3
 *
 * Proudly created by Chinaski      March 2018
 *
 * main.c
 *
 * USING FMOD & SDL LIBRARIES
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <FMOD/fmod.h>
#include <FMOD/fmod_errors.h>

#define WINDOW_WEIGHT         800
#define WINDOW_HEIGHT         600
#define SPECTRUM_ARRAY_SIZE   2048 

int main(int argc, char *argv[]) {

    int exitStatus = EXIT_FAILURE;
    int i = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Rect screen = {0, 0, WINDOW_WEIGHT, WINDOW_HEIGHT};
    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    int tempsPrecedent = 0, tempsActuel = 0;

    FMOD_RESULT result = 0;
    FMOD_SYSTEM *system = NULL;
    FMOD_SOUND *music = NULL;
    FMOD_CHANNEL *channel0 = NULL;
    FMOD_BOOL *isPlaying = NULL;
    FMOD_DSP *fftDSP = NULL;
    FMOD_DSP_PARAMETER_FFT *fftParam = NULL;

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


    // FMOD's object, channel & sound : creation & initialisation
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

    result = FMOD_System_GetChannel(system, 0, &channel0);
    if (result != FMOD_OK) {
        fprintf(stderr, "FMOD_System_GetChannel Error : %s\n", FMOD_ErrorString(result));
        goto QuitFMOD;
    }

    // DSP Initialization
    result = FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_FFT, &fftDSP);
    if (result != FMOD_OK) {
        fprintf(stderr, "FMOD_System_CreateDSPByType Error : %s\n", FMOD_ErrorString(result));
        goto QuitFMOD;
    }

    result = FMOD_DSP_SetParameterInt(fftDSP, FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_RECT);
    if (result != FMOD_OK) {
        fprintf(stderr, "FMOD_SetParameterInt Error : %s\n", FMOD_ErrorString(result));
        goto QuitFMOD;
    }

    result = FMOD_DSP_SetParameterInt(fftDSP, FMOD_DSP_FFT_WINDOWSIZE, SPECTRUM_ARRAY_SIZE * 2);
    if (result != FMOD_OK) {
        fprintf(stderr, "FMOD_SetParameterInt Error : %s\n", FMOD_ErrorString(result));
        goto QuitFMOD;
    }

    while(!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = SDL_TRUE;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            quit = SDL_TRUE;
                            break;
                        case SDLK_SPACE:
                            // Load, play and send the mp3 to the channel
                            result = FMOD_System_CreateSound(system, "LeavingEarth.mp3", FMOD_CREATESTREAM, NULL, &music);
                            if (result != FMOD_OK) {
                                fprintf(stderr, "FMOD_System_CreateSound Error : %s\n", FMOD_ErrorString(result));
                                goto QuitFMOD;
                            }

                            result = FMOD_System_PlaySound(system, music, NULL, 0, &channel0);
                            if (result != FMOD_OK) {
                                fprintf(stderr, "FMOD_System_PlaySound Error : %s\n", FMOD_ErrorString(result));
                                goto QuitFMOD;
                            }

                            result = FMOD_Channel_IsPlaying(channel0, isPlaying);
                            if (result != FMOD_OK) {
                                fprintf(stderr, "FMOD_Channel_IsPlaying Error : %s\n", FMOD_ErrorString(result));
                                goto QuitFMOD;
                            }

                            // Get the Spectrum
                            result = FMOD_Channel_AddDSP(channel0, 0, fftDSP);
                            if (result != FMOD_OK) {
                                fprintf(stderr, "FMOD_Channel_AddDSP : %s\n", FMOD_ErrorString(result));
                                goto QuitFMOD;
                            }

                            result = FMOD_DSP_SetActive(fftDSP, 1);
                            if (result != FMOD_OK) {
                                fprintf(stderr, "FMOD_DSP_SetActive : %s\n", FMOD_ErrorString(result));
                                goto QuitFMOD;
                            }

                            float spectrum[SPECTRUM_ARRAY_SIZE] = {0};

                            result = FMOD_DSP_GetParameterData(fftDSP, FMOD_DSP_FFT_SPECTRUMDATA, (void**)&fftParam, NULL, NULL, 0);
                            if (result != FMOD_OK) {
                                fprintf(stderr, "FMOD_DSP_GetParameterData : %s\n", FMOD_ErrorString(result));
                                goto QuitFMOD;
                            }
                            
                            if (*fftParam->spectrum != NULL) {
                                for (i = 0 ; i < SPECTRUM_ARRAY_SIZE ; i++) {
                                    spectrum[i] = (fftParam->spectrum[0][i] + fftParam->spectrum[1][i] / 2);
                                    printf("%f\n", spectrum[i]);
                                }
                            }      
                            for (i = 0 ; i < SPECTRUM_ARRAY_SIZE ; i++) 
                                fprintf(stderr, "%f\n", spectrum[i]);

                            break;
                    }
            }
        }
    }

    exitStatus = EXIT_SUCCESS;

QuitFMOD:
    FMOD_Sound_Release(music);
    FMOD_System_Close(system);
    FMOD_System_Release(system);

QuitSDL:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    fprintf(stderr, "%d\n", exitStatus);
    return exitStatus;
}

