#ifndef entite_H_INCLUDED 
#define entite_H_INCLUDED

#include <stdio.h>     
#include <stdlib.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include<time.h>

typedef struct {   
	  

	SDL_Surface *entity; 
	SDL_Rect pos_entity; 
	SDL_Rect anim_entity[70];  
	int frame_entity;
	int direction;
	int col ;
	int maxi_up,maxi_down;
	int show;

}entity ;


typedef struct
{
SDL_Surface *perso; 
SDL_Rect perso_pos ;
}personnage ;

void init_tab_anim_entity(SDL_Rect* clip,entity *e);   
void initentity (entity *e) ;
void afficherentity(entity e , SDL_Surface *screen);
void animerentity(entity *e);
void deplacer(entity *e);
void update_entity(entity *e);

int detect_collision(personnage *p, entity* e);  
int gestion(entity* e);
void initialiser_perso(personnage *p);
void afficher_perso(personnage p , SDL_Surface *screen);


#endif 
