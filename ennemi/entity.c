
#include "entity.h"




int 
nb_frame=12 ,
entity_h=100,
entity_w=100,
entity_y=100,
entity_x=100;

								 

void init_tab_anim_entity(SDL_Rect *clip,entity *e)
{		
	for (int j=0;j<nb_frame; j++)
	{
	clip[j].h=entity_h;
	clip[j].w=entity_w;
	clip[j].x = j*entity_x;
	clip[j].y = 0;
        }  
}



void initentity(entity *e)
{


    e->entity = IMG_Load("es.png"); 

    e->pos_entity.x = 400;
    e->pos_entity.y = 100;
    init_tab_anim_entity(e->anim_entity,e); 
    e->frame_entity=0;
    e->direction=0;  
    e->col=0;
    e->show=1; 
      
}



void afficherentity(entity e , SDL_Surface *screen)
{
   
   SDL_BlitSurface(e.entity,&e.anim_entity[e.frame_entity], screen, &e.pos_entity);

}


void deplacer(entity *e)
{	
	srand (time(0));

	int maxi_down=300+rand()%300;  
	int maxi_up=50+rand()%300; 

  	


if(e->pos_entity.y>= maxi_down)
{
e->direction =1;
}
if(e->pos_entity.y<=maxi_up)
{
e->direction =0;
}

if(e->direction==1)
{   
	e->pos_entity.y-=5;

}
if(e->direction==0)
{
        
	e->pos_entity.y+=7;

}
}





void animerentity(entity *e)
{   
	if (e->frame_entity >=0 && e->frame_entity <(nb_frame-1)) 
	e->frame_entity++;
	
	if ( e->frame_entity ==(nb_frame-1)) 
	e->frame_entity=0;
}


void update_entity(entity *e)
{ 	
	deplacer(e);
	animerentity(e);	
}




void initialiser_perso(personnage *p)
{
    p->perso= IMG_Load("per.png");
    p->perso_pos.x = 100;
    p->perso_pos.y = 100;
    p->perso_pos.h = 281;
    p->perso_pos.w = 217;
    
    
}

void afficher_perso(personnage p , SDL_Surface *screen)
{
   SDL_BlitSurface(p.perso,NULL, screen, &p.perso_pos);

}


int detect_collision(personnage *p, entity* e)
{
	
if ((p->perso_pos.x + p->perso_pos. w< e->pos_entity. x) || (p->perso_pos.x> e->pos_entity. x + e->pos_entity. w) ||
(p->perso_pos.y + p->perso_pos.h< e->pos_entity. y) || (p->perso_pos.y> e->pos_entity. y + e->pos_entity. h ))
return 0;
else 
return 1;
 
}

int gestion(entity* e)
{
e->show=0; 
return 0;

}
