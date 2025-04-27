#ifndef FONCTIONS_H
#define FONCTIONS_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>

// Nombre de frames pour les animations (pour cet exemple, 10 frames pour tout)
#define NB_FRAMES_WALK         10
#define NB_FRAMES_WALK_FORWARD  10
#define NB_FRAMES_WALK_BACKWARD 10
#define NB_FRAMES_JUMP_FORWARD  6
#define NB_FRAMES_JUMP_BACK     6
#define NB_FRAMES_ATTACK        10

typedef struct {
    SDL_Surface *screen;
    SDL_Surface *background;
    SDL_Surface *spriteSheetWalkForward;
    SDL_Surface *spriteSheetWalkBackward;
    SDL_Surface *spriteSheetJumpForward;
    SDL_Surface *spriteSheetJumpBackward;
    SDL_Surface *spriteSheetAttackForward;
    SDL_Surface *spriteSheetAttackBackward;
    SDL_Surface *spriteSheetStandForward;    // Sprite idle si dernier mouvement vers l'avant
    SDL_Surface *spriteSheetStandBackward;   // Sprite idle si dernier mouvement vers l'arrière
    SDL_Surface *currentSpriteSheet;
    SDL_Surface *heart;

    SDL_Rect frames[NB_FRAMES_WALK];  // utilisation de NB_FRAMES_WALK comme nombre max de frames
    int currentFrame;
    int nbFrames;
    int animCounter;
    int frameWidth;

    float posX;      // Position affichée du personnage (clampée dans sa zone)
    float posY;
    float camX;      // Offset pour défiler le background (mode normal et split)
    SDL_Rect posPlayer;

    int isJumping;
    int jumpCount;
    int isAttacking;
    int lastDirection;

    int moveLeft;
    int moveRight;
    int moveUp;
    int moveDown;

    int score;

    TTF_Font *font;
    SDL_Color textColor;
} Game;

// Prototypes des fonctions
int initSDL(Game *game);
void loadImages(Game *game);
void updateFrames(Game *game);
void movePlayer(Game *game, float dx, float dy);
void updateJump(Game *game);
void updateAttack(Game *game);
void renderGame(SDL_Surface *screen, Game *game);
void renderGameSplit(SDL_Surface *screen, Game *game, int hudOffsetX);
void renderMiniScreen(SDL_Surface *screen);
void cleanUp(Game *game);

#endif // FONCTIONS_H
