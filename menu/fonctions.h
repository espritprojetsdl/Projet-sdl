#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_W 1920  
#define SCREEN_H 1080    

typedef struct {
    SDL_Surface *img;
    SDL_Rect posAff;   
    SDL_Rect posEcr;   
} image;

typedef struct {
    SDL_Surface *quit;
    SDL_Rect posQuit;   
} quit;
typedef struct {
    SDL_Surface *quit;   // Image du bouton
    SDL_Rect posQuit;    // Position du bouton
} QuitButton;

