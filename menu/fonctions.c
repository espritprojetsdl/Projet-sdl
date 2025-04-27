#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <string.h>
#include "fonction.h"
void initialiserBackg(image *im){
    im->img=IMG_Load("s.jpg");
    if(im->img==NULL){
        printf("Echec de chargement bg %s \n",SDL_GetError())
        return;
    }
    im->posAff.x=0;
    im->posAff.y=0;
    im->posAff.h=SCREEN_H;
    im->posAff.w=SCREEN_W;
    im->posEcr.x=0;
    im->posEcr.y=0;
}
void initialiserButtonOui(Oui *o) {
    o->Oui = IMG_Load("bt.png");
    if (o->Oui == NULL) {
        printf("Echec de chargement retour %s\n", SDL_GetError());
        return;
    }
void initialiserButtonQuitter(QuitButton *q) {
    q->quit = IMG_Load("quit_button.png");  // L'image pour le bouton Quitter
    if (q->quit == NULL) {
        printf("Échec du chargement du bouton Quitter: %s\n", SDL_GetError());
        return;
    }
    
    q->posQuit.x = SCREEN_W / 2 - q->quit->w / 2;  // Centrer le bouton horizontalement
    q->posQuit.y = SCREEN_H - q->quit->h - 50;     // Positionner le bouton en bas de l'écran
}


}
