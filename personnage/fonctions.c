#include <stdio.h>
#include <stdlib.h>
#include "fonctions.h"

// Fonction statique pour dessiner le background en tiling.
static void renderTiledBackground(SDL_Surface *dest, SDL_Surface *bg, SDL_Rect clip, int offsetX) {
    int bgWidth = bg->w;
    int off = offsetX % bgWidth;
    if (off < 0)
        off += bgWidth;
    int startX = clip.x - off;
    for (int x = startX; x < clip.x + clip.w; x += bgWidth) {
        SDL_Rect destRect = { x, clip.y, bgWidth, bg->h };
        SDL_BlitSurface(bg, NULL, dest, &destRect);
    }
}

int initSDL(Game *game) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        return -1;
    }
    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        return -1;
    }

    game->screen = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!game->screen) {
        fprintf(stderr, "Erreur SDL_SetVideoMode : %s\n", SDL_GetError());
        return -1;
    }

    SDL_WM_SetCaption("Jeu SDL - Mouvement, Jump & Attack", NULL);

    game->posX = 100.0f;
    game->posY = 100.0f;
    game->camX = 0;
    game->posPlayer.x = (int)game->posX;
    game->posPlayer.y = (int)game->posY;

    game->currentFrame = 0;
    game->nbFrames = NB_FRAMES_WALK_BACKWARD;
    game->animCounter = 0;

    game->isJumping = 0;
    game->jumpCount = 10;
    game->isAttacking = 0;
    game->lastDirection = 1;

    game->moveLeft = game->moveRight = game->moveUp = game->moveDown = 0;
    game->score = 0;

    game->font = TTF_OpenFont("arial.ttf", 28);
    if (!game->font) {
        fprintf(stderr, "Erreur chargement font : %s\n", TTF_GetError());
        return -1;
    }
    game->textColor = (SDL_Color){255, 255, 255};

    return 0;
}

void loadImages(Game *game) {
    // Chargement du background. Redimensionnement pour tiling horizontal.
    game->background = IMG_Load("a.jpg");
    if (!game->background) {
        fprintf(stderr, "Erreur chargement background : %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    double scaleY = (double)game->screen->h / game->background->h;
    SDL_Surface *temp = game->background;
    game->background = zoomSurface(game->background, 1.0, scaleY, 1);
    SDL_FreeSurface(temp);

    // Chargement des sprites pour déplacement, saut et attaque
    game->spriteSheetWalkForward = IMG_Load("b.png");
    game->spriteSheetWalkBackward = IMG_Load("c.png");
    game->spriteSheetJumpForward = IMG_Load("d.png");
    game->spriteSheetJumpBackward = IMG_Load("e.png");
    game->spriteSheetAttackForward = IMG_Load("attack1.png");
    game->spriteSheetAttackBackward = IMG_Load("attack2.png");

    if (!game->spriteSheetWalkForward || !game->spriteSheetWalkBackward ||
        !game->spriteSheetJumpForward || !game->spriteSheetJumpBackward ||
        !game->spriteSheetAttackForward || !game->spriteSheetAttackBackward) {
        fprintf(stderr, "Erreur chargement sprites\n");
        exit(EXIT_FAILURE);
    }

    // Mise à l'échelle des sprites de déplacement
    game->spriteSheetWalkForward = zoomSurface(game->spriteSheetWalkForward, 0.5, 0.5, 1);
    game->spriteSheetWalkBackward = zoomSurface(game->spriteSheetWalkBackward, 0.5, 0.5, 1);
    game->spriteSheetJumpForward = zoomSurface(game->spriteSheetJumpForward, 0.5, 0.5, 1);
    game->spriteSheetJumpBackward = zoomSurface(game->spriteSheetJumpBackward, 0.5, 0.5, 1);
    game->spriteSheetAttackForward = zoomSurface(game->spriteSheetAttackForward, 0.5, 0.5, 1);
    game->spriteSheetAttackBackward = zoomSurface(game->spriteSheetAttackBackward, 0.5, 0.5, 1);

    // Chargement des sprites idle (stand)
    game->spriteSheetStandForward = IMG_Load("s.png");
    game->spriteSheetStandBackward = IMG_Load("s2.png");
    if (!game->spriteSheetStandForward || !game->spriteSheetStandBackward) {
        fprintf(stderr, "Erreur chargement sprites stand\n");
        exit(EXIT_FAILURE);
    }
    game->spriteSheetStandForward = zoomSurface(game->spriteSheetStandForward, 0.5, 0.5, 1);
    game->spriteSheetStandBackward = zoomSurface(game->spriteSheetStandBackward, 0.5, 0.5, 1);

    // Par défaut, utiliser le spritesheet de marche avant
    game->currentSpriteSheet = game->spriteSheetWalkForward;
    game->nbFrames = NB_FRAMES_WALK_FORWARD;
    game->frameWidth = game->currentSpriteSheet->w / game->nbFrames;
    updateFrames(game);

    // Chargement de l'image "coeur"
    SDL_Surface *heartTemp = IMG_Load("coeur.png");
    if (!heartTemp) {
        fprintf(stderr, "Erreur chargement coeur.png : %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    game->heart = zoomSurface(heartTemp, 0.05, 0.05, 1);
    SDL_FreeSurface(heartTemp);
}

void updateFrames(Game *game) {
    for (int i = 0; i < game->nbFrames; i++) {
        game->frames[i].x = i * game->frameWidth;
        game->frames[i].y = 0;
        game->frames[i].w = game->frameWidth;
        game->frames[i].h = game->currentSpriteSheet->h;
    }
}

// Fonction movePlayer pour mettre à jour la position et le décalage (tiling)
void movePlayer(Game *game, float dx, float dy) {
    float newPosX = game->posX + dx;
    float newPosY = game->posY + dy;

    if (newPosX < 0) {
        game->camX -= dx;
        newPosX = 0;
    }
    if (newPosX > game->screen->w - game->frameWidth) {
        game->camX += dx;
        newPosX = game->screen->w - game->frameWidth;
    }
    game->posX = newPosX;

    if (newPosY < 0)
        newPosY = 0;
    else if (newPosY > game->screen->h - game->currentSpriteSheet->h)
        newPosY = game->screen->h - game->currentSpriteSheet->h;
    game->posY = newPosY;

    game->posPlayer.x = (int)game->posX;
    game->posPlayer.y = (int)game->posY;

    if (!game->isJumping && !game->isAttacking) {
        if (dx > 0) {
            game->currentSpriteSheet = game->spriteSheetWalkForward;
            game->nbFrames = NB_FRAMES_WALK_FORWARD;
        } else if (dx < 0) {
            game->currentSpriteSheet = game->spriteSheetWalkBackward;
            game->nbFrames = NB_FRAMES_WALK_BACKWARD;
        }
        game->frameWidth = game->currentSpriteSheet->w / game->nbFrames;
        updateFrames(game);
        if (dx != 0.0f || dy != 0.0f) {
            game->animCounter++;
            if (game->animCounter >= 3) {
                game->currentFrame = (game->currentFrame + 1) % game->nbFrames;
                game->animCounter = 0;
                game->score++;
            }
        }
    }
}

// Mise à jour du saut avec translation horizontale
void updateJump(Game *game) {
    if (game->jumpCount >= -10) {
        int neg = (game->jumpCount < 0) ? -1 : 1;
        game->posY -= (int)((game->jumpCount * game->jumpCount) * 0.5 * neg);
        if (game->moveRight)
            game->posX += 10;
        else if (game->moveLeft)
            game->posX -= 10;
        game->jumpCount--;

        game->animCounter++;
        if (game->animCounter >= 3) {
            game->currentFrame = (game->currentFrame + 1) % game->nbFrames;
            game->animCounter = 0;
            game->score++;
        }
    } else {
        game->isJumping = 0;
        game->jumpCount = 10;
        if (game->lastDirection == 1)
            game->currentSpriteSheet = game->spriteSheetWalkForward;
        else
            game->currentSpriteSheet = game->spriteSheetWalkBackward;
        game->nbFrames = (game->lastDirection == 1) ? NB_FRAMES_WALK_FORWARD : NB_FRAMES_WALK_BACKWARD;
        game->frameWidth = game->currentSpriteSheet->w / game->nbFrames;
        updateFrames(game);
        game->currentFrame = 0;
    }

    game->posPlayer.x = (int)game->posX;
    game->posPlayer.y = (int)game->posY;
}

void updateAttack(Game *game) {
    game->animCounter++;
    if (game->animCounter >= 3) {
        game->currentFrame++;
        game->animCounter = 0;
        if (game->currentFrame >= game->nbFrames) {
            game->isAttacking = 0;
            game->currentFrame = 0;
            if (game->lastDirection == 1)
                game->currentSpriteSheet = game->spriteSheetWalkForward;
            else
                game->currentSpriteSheet = game->spriteSheetWalkBackward;
            game->nbFrames = (game->lastDirection == 1) ? NB_FRAMES_WALK_FORWARD : NB_FRAMES_WALK_BACKWARD;
            game->frameWidth = game->currentSpriteSheet->w / game->nbFrames;
            updateFrames(game);
        }
    }
}

void renderGameSplit(SDL_Surface *screen, Game *game, int hudOffsetX) {
    SDL_Rect clip;
    SDL_GetClipRect(screen, &clip);
    int offset = (int)game->camX;
    renderTiledBackground(screen, game->background, clip, offset);

    // Si le personnage est inactif, afficher le sprite idle en ne gardant que sa première frame
    if (!game->moveLeft && !game->moveRight && !game->isJumping && !game->isAttacking) {
        if (game->lastDirection == 1) {
            game->currentSpriteSheet = game->spriteSheetStandForward;
        } else {
            game->currentSpriteSheet = game->spriteSheetStandBackward;
        }
        game->nbFrames = 1; // On force l'animation idle à une seule frame
        // IMPORTANT : Si votre spritesheet idle comporte 10 images, vous voulez extraire une frame :
        game->frameWidth = game->currentSpriteSheet->w / 10;
        game->currentFrame = 0; // Forcer la première frame à être affichée
        updateFrames(game);
    }

    // Affichage du personnage
    SDL_BlitSurface(game->currentSpriteSheet, &game->frames[game->currentFrame], screen, &game->posPlayer);

    // Affichage du HUD (score et coeurs)
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", game->score);
    SDL_Surface *textSurface = TTF_RenderText_Solid(game->font, scoreText, game->textColor);
    if (textSurface) {
        SDL_Rect textLocation = { hudOffsetX, 10, textSurface->w, textSurface->h };
        SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
        int spacing = 10;
        int heartsStartX = textLocation.x + textSurface->w + spacing;
        SDL_Rect heartLocation = { heartsStartX, 10, game->heart->w, game->heart->h };
        for (int i = 0; i < 3; i++) {
            heartLocation.x = heartsStartX + i * (game->heart->w + 2);
            SDL_BlitSurface(game->heart, NULL, screen, &heartLocation);
        }
        SDL_FreeSurface(textSurface);
    }
}

void renderMiniScreen(SDL_Surface *screen) {
    double scale = 0.2;
    SDL_Surface *mini = zoomSurface(screen, scale, scale, 1);
    if (mini) {
        int posX = (screen->w - mini->w) / 2;
        int posY = 0;
        SDL_Rect destRect = { posX, posY, mini->w, mini->h };
        SDL_BlitSurface(mini, NULL, screen, &destRect);
        SDL_FreeSurface(mini);
    }
}

void renderGame(SDL_Surface *screen, Game *game) {
    renderGameSplit(screen, game, 10);
    renderMiniScreen(screen);
    SDL_Flip(screen);
}

void cleanUp(Game *game) {
    if (game->background) SDL_FreeSurface(game->background);
    if (game->spriteSheetWalkForward) SDL_FreeSurface(game->spriteSheetWalkForward);
    if (game->spriteSheetWalkBackward) SDL_FreeSurface(game->spriteSheetWalkBackward);
    if (game->spriteSheetJumpForward) SDL_FreeSurface(game->spriteSheetJumpForward);
    if (game->spriteSheetJumpBackward) SDL_FreeSurface(game->spriteSheetJumpBackward);
    if (game->spriteSheetAttackForward) SDL_FreeSurface(game->spriteSheetAttackForward);
    if (game->spriteSheetAttackBackward) SDL_FreeSurface(game->spriteSheetAttackBackward);
    if (game->spriteSheetStandForward) SDL_FreeSurface(game->spriteSheetStandForward);
    if (game->spriteSheetStandBackward) SDL_FreeSurface(game->spriteSheetStandBackward);
    if (game->heart) SDL_FreeSurface(game->heart);
    if (game->font) TTF_CloseFont(game->font);
    TTF_Quit();
    SDL_Quit();
}
