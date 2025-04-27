#ifndef ENNEMIE_H_INCLUDED
#define ENNEMIE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include<time.h>
#include "entity.h"

typedef struct {
	SDL_Surface *ennemie ;  
	SDL_Rect pos_ennemie; 
	SDL_Rect anim_ennemie[70]; 
	int frame_ennemie;
	int direction;
	int col ;
	int type;
	int maxi_up,maxi_down;
	int show;

}Ennemie;


void init_tab_anim_ennemie (SDL_Rect* clip,Ennemie *e); 
void initEnnemi  (Ennemie *e) ;
void afficherEnnemi (Ennemie e, SDL_Surface *screen);
void animerEnnemi (Ennemie *e);
void deplacerIA (Ennemie *e,personnage *p);
void update_ennemie (Ennemie *e,personnage *p);

int detect_collision_ennemie(personnage *p, Ennemie *e);

int gerer(Ennemie *e) ;
void affiche_time(SDL_Surface* screen, TTF_Font* font, Uint32 start_time); 


#endif 
