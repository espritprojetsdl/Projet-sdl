#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "fonctions.h"

int main(int argc, char *argv[]) {
    Game game1, game2;
    int active2 = 0;      // Mode split activé par la touche "p"
    int shiftActive = 0;  // Boost pour le joueur 1 avec Shift
    int ctrlActive = 0;   // Boost pour le joueur 2 avec Ctrl (en mode split)
    const float normalSpeed = 2.5f;

    // Initialisation et configuration pour le joueur 1 (mode normal ou partie gauche en mode split)
    if (initSDL(&game1) != 0) {
        fprintf(stderr, "Erreur d'initialisation pour game1.\n");
        return EXIT_FAILURE;
    }
    loadImages(&game1);
    game1.posX = 0;
    game1.posY = game1.screen->h - game1.currentSpriteSheet->h - 80;
    game1.camX = 0;
    game1.posPlayer.x = (int)game1.posX;
    game1.posPlayer.y = (int)game1.posY;

    // Initialisation et configuration pour le joueur 2 (zone droite en mode split)
    game2.screen = game1.screen;  // Même écran
    if (initSDL(&game2) != 0) {
        fprintf(stderr, "Erreur d'initialisation pour game2.\n");
        return EXIT_FAILURE;
    }
    loadImages(&game2);
    game2.posX = game2.screen->w / 2;
    game2.posY = game2.screen->h - game2.currentSpriteSheet->h - 80;
    game2.camX = 0;
    game2.posPlayer.x = (int)game2.posX;
    game2.posPlayer.y = (int)game2.posY;
    game2.currentFrame = 0;
    game2.nbFrames = NB_FRAMES_WALK_FORWARD;
    game2.animCounter = 0;
    game2.isJumping = 0;
    game2.jumpCount = 10;
    game2.isAttacking = 0;
    game2.lastDirection = 1;
    game2.moveLeft = game2.moveRight = game2.moveUp = game2.moveDown = 0;
    game2.score = 0;

    int running = 1;
    SDL_Event event;
    Uint32 startTime;

    while (running) {
        startTime = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    // Activation des boosts
                    if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
                        shiftActive = 1;
                    if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
                        ctrlActive = 1;
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = 0;
                            break;
                        case SDLK_d:
                            game1.moveRight = 1;
                            game1.lastDirection = 1;
                            break;
                        case SDLK_q:
                            game1.moveLeft = 1;
                            game1.lastDirection = -1;
                            break;
                        case SDLK_SPACE:
                            if (!game1.isJumping && !game1.isAttacking) {
                                game1.isJumping = 1;
                                game1.jumpCount = 10;
                                if (game1.lastDirection == 1) {
                                    game1.currentSpriteSheet = game1.spriteSheetJumpForward;
                                    game1.nbFrames = NB_FRAMES_JUMP_FORWARD;
                                } else {
                                    game1.currentSpriteSheet = game1.spriteSheetJumpBackward;
                                    game1.nbFrames = NB_FRAMES_JUMP_BACK;
                                }
                                game1.frameWidth = game1.currentSpriteSheet->w / game1.nbFrames;
                                updateFrames(&game1);
                            }
                            break;
                        case SDLK_e:
                            if (!game1.isAttacking) {
                                game1.isAttacking = 1;
                                game1.currentFrame = 0;
                                if (game1.lastDirection == 1) {
                                    game1.currentSpriteSheet = game1.spriteSheetAttackForward;
                                    game1.nbFrames = NB_FRAMES_ATTACK;
                                } else {
                                    game1.currentSpriteSheet = game1.spriteSheetAttackBackward;
                                    game1.nbFrames = NB_FRAMES_ATTACK;
                                }
                                game1.frameWidth = game1.currentSpriteSheet->w / game1.nbFrames;
                                updateFrames(&game1);
                            }
                            break;
                        case SDLK_p:
                            active2 = 1;
                            break;
                        default:
                            break;
                    }
                    if (active2) {
                        switch (event.key.keysym.sym) {
                            case SDLK_RIGHT:
                                game2.moveRight = 1;
                                game2.lastDirection = 1;
                                break;
                            case SDLK_LEFT:
                                game2.moveLeft = 1;
                                game2.lastDirection = -1;
                                break;
                            case SDLK_UP:
                                if (!game2.isJumping && !game2.isAttacking) {
                                    game2.isJumping = 1;
                                    game2.jumpCount = 10;
                                    if (game2.lastDirection == 1) {
                                        game2.currentSpriteSheet = game2.spriteSheetJumpForward;
                                        game2.nbFrames = NB_FRAMES_JUMP_FORWARD;
                                    } else {
                                        game2.currentSpriteSheet = game2.spriteSheetJumpBackward;
                                        game2.nbFrames = NB_FRAMES_JUMP_BACK;
                                    }
                                    game2.frameWidth = game2.currentSpriteSheet->w / game2.nbFrames;
                                    updateFrames(&game2);
                                }
                                break;
                            case SDLK_DOWN:
                                if (!game2.isAttacking) {
                                    game2.isAttacking = 1;
                                    game2.currentFrame = 0;
                                    if (game2.lastDirection == 1) {
                                        game2.currentSpriteSheet = game2.spriteSheetAttackForward;
                                        game2.nbFrames = NB_FRAMES_ATTACK;
                                    } else {
                                        game2.currentSpriteSheet = game2.spriteSheetAttackBackward;
                                        game2.nbFrames = NB_FRAMES_ATTACK;
                                    }
                                    game2.frameWidth = game2.currentSpriteSheet->w / game2.nbFrames;
                                    updateFrames(&game2);
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
                        shiftActive = 0;
                    if (event.key.keysym.sym == SDLK_LCTRL || event.key.keysym.sym == SDLK_RCTRL)
                        ctrlActive = 0;
                    switch (event.key.keysym.sym) {
                        case SDLK_d:
                            game1.moveRight = 0;
                            break;
                        case SDLK_q:
                            game1.moveLeft = 0;
                            break;
                        default:
                            break;
                    }
                    if (active2) {
                        switch (event.key.keysym.sym) {
                            case SDLK_RIGHT:
                                game2.moveRight = 0;
                                break;
                            case SDLK_LEFT:
                                game2.moveLeft = 0;
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // Calcul de la vitesse selon les boosts
        float speed1 = normalSpeed;
        if (shiftActive)
            speed1 *= 2.2f;  // Boost pour le joueur 1 avec Shift
        float speed2 = normalSpeed;
        if (ctrlActive)
            speed2 *= 2.2f;  // Boost pour le joueur 2 avec Ctrl

        float dx1 = 0.0f, dx2 = 0.0f, dy = 0.0f;
        if (game1.moveLeft)
            dx1 -= speed1;
        if (game1.moveRight)
            dx1 += speed1;
        if (active2) {
            if (game2.moveLeft)
                dx2 -= speed2;
            if (game2.moveRight)
                dx2 += speed2;
        }

        // Mise à jour des positions et animations
        if (!active2) {
            // Mode normal pour le joueur 1
            movePlayer(&game1, dx1, dy);
        } else {
            // Mode split : joueur 1 (zone gauche)
            float newPos1 = game1.posX + dx1;
            game1.posX = newPos1;
            if (game1.posX < 0) {
                game1.camX -= dx1;
                game1.posX = 0;
            }
            if (game1.posX > (game1.screen->w / 2 - game1.frameWidth)) {
                game1.camX += dx1;
                game1.posX = game1.screen->w / 2 - game1.frameWidth;
            }
            if (!game1.isAttacking && !game1.isJumping) {
                if (dx1 > 0) {
                    game1.currentSpriteSheet = game1.spriteSheetWalkForward;
                    game1.nbFrames = NB_FRAMES_WALK_FORWARD;
                } else if (dx1 < 0) {
                    game1.currentSpriteSheet = game1.spriteSheetWalkBackward;
                    game1.nbFrames = NB_FRAMES_WALK_BACKWARD;
                }
                game1.frameWidth = game1.currentSpriteSheet->w / game1.nbFrames;
                updateFrames(&game1);
                if (dx1 != 0) {
                    game1.animCounter++;
                    if (game1.animCounter >= 3) {
                        game1.currentFrame = (game1.currentFrame + 1) % game1.nbFrames;
                        game1.animCounter = 0;
                        game1.score++;
                    }
                }
            }
            game1.posPlayer.x = (int)game1.posX;

            // Mode split : joueur 2 (zone droite)
            float newPos2 = game2.posX + dx2;
            game2.posX = newPos2;
            if (game2.posX < game2.screen->w / 2) {
                game2.camX += dx2;
                game2.posX = game2.screen->w / 2;
            }
            if (game2.posX > (game2.screen->w - game2.frameWidth)) {
                game2.camX += dx2;
                game2.posX = game2.screen->w - game2.frameWidth;
            }
            if (!game2.isAttacking && !game2.isJumping) {
                if (dx2 > 0) {
                    game2.currentSpriteSheet = game2.spriteSheetWalkForward;
                    game2.nbFrames = NB_FRAMES_WALK_FORWARD;
                } else if (dx2 < 0) {
                    game2.currentSpriteSheet = game2.spriteSheetWalkBackward;
                    game2.nbFrames = NB_FRAMES_WALK_BACKWARD;
                }
                game2.frameWidth = game2.currentSpriteSheet->w / game2.nbFrames;
                updateFrames(&game2);
                if (dx2 != 0) {
                    game2.animCounter++;
                    if (game2.animCounter >= 3) {
                        game2.currentFrame = (game2.currentFrame + 1) % game2.nbFrames;
                        game2.animCounter = 0;
                        game2.score++;
                    }
                }
            }
            game2.posPlayer.x = (int)game2.posX;
        }

        // Mise à jour du saut et de l'attaque
        if (game1.isJumping)
            updateJump(&game1);
        if (game1.isAttacking)
            updateAttack(&game1);
        if (active2) {
            if (game2.isJumping)
                updateJump(&game2);
            if (game2.isAttacking)
                updateAttack(&game2);
        }

        // Rendu
        if (!active2) {
            renderGame(game1.screen, &game1);
        } else {
            SDL_Rect leftClip = {0, 0, game1.screen->w / 2, game1.screen->h};
            SDL_Rect rightClip = {game1.screen->w / 2, 0, game1.screen->w / 2, game1.screen->h};

            SDL_SetClipRect(game1.screen, &leftClip);
            renderGameSplit(game1.screen, &game1, 10);
            SDL_SetClipRect(game1.screen, &rightClip);
            renderGameSplit(game1.screen, &game2, game1.screen->w / 2 + 210);
            SDL_SetClipRect(game1.screen, NULL);

            SDL_Rect lineRect = {game1.screen->w / 2 - 1, 0, 2, game1.screen->h};
            SDL_FillRect(game1.screen, &lineRect, SDL_MapRGB(game1.screen->format, 0, 0, 0));

            renderMiniScreen(game1.screen);
            SDL_Flip(game1.screen);
        }

        int frameTime = SDL_GetTicks() - startTime;
        if (frameTime < 16)
            SDL_Delay(16 - frameTime);
    }

    cleanUp(&game1);
    if (active2)
        cleanUp(&game2);

    return 0;
}
