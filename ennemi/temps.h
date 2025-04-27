#ifndef Temps_H_INCLUDED
#define Temps_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <string.h>



typedef struct  {
    SDL_Surface* textSurface;
    SDL_Rect positionText;
    char txt[20];
    SDL_Color couleurTxt;
    TTF_Font* police;
}Text;

typedef struct
{
    int tempsdebut;
    int mm; 
    int ss;
    Text temps;
    
} Time;



void inittemps(Time *t); 
void inittempsenig(Time *t);
void Timer(int *tempsdebut);
int initTexttime(Text* T);
int initTexttimeenig(Text* T);
int loadFonttime(Text* T, char* Lodstay);
void update_time(Time* T);
void displaytime(Time T,SDL_Surface *screen);
void freeTexttime(Text T);


#endif
