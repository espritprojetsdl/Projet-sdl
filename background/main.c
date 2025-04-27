#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PLAYER_WIDTH 550
#define PLAYER_HEIGHT 500
#define PLAYER_SPEED 4
#define LEVEL_WIDTH 1920

typedef struct {
    SDL_Rect pos;
    int velX;
} Player;

int main(int argc, char *argv[]) {
    // Initialize SDL and SDL_image
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    SDL_WM_SetCaption("Simple Platformer", NULL);

    // Load the player image and background using SDL_image
    SDL_Surface *playerSurface = IMG_Load("per.png");
    if (!playerSurface) {
        printf("Failed to load player image! SDL_image Error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface *background = IMG_Load("background.jpg");
    if (!background) {
        printf("Failed to load background image! SDL_image Error: %s\n", IMG_GetError());
        SDL_FreeSurface(playerSurface);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Set color key for transparency (magenta)
    SDL_SetColorKey(playerSurface, SDL_SRCCOLORKEY, SDL_MapRGB(playerSurface->format, 255, 0, 255));

    Player player1 = {{100, SCREEN_HEIGHT - PLAYER_HEIGHT - 50, PLAYER_WIDTH, PLAYER_HEIGHT}, 0};
    Player player2 = {{200, SCREEN_HEIGHT - PLAYER_HEIGHT - 50, PLAYER_WIDTH, PLAYER_HEIGHT}, 0};

    int camera1X = 0, camera2X = 0;
    bool running = true;
    bool splitMode = false;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    // Mouvements joueur 1
                    case SDLK_LEFT: player1.velX = -PLAYER_SPEED; break;
                    case SDLK_RIGHT: player1.velX = PLAYER_SPEED; break;

                    // Mouvements joueur 2
                    case SDLK_a: player2.velX = -PLAYER_SPEED; break;
                    case SDLK_d: player2.velX = PLAYER_SPEED; break;

                    // Changement de mode
                    case SDLK_p: splitMode = true; break;
                    case SDLK_m: splitMode = false; break;
                }
            }

            else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT)
                    player1.velX = 0;
                if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d)
                    player2.velX = 0;
            }
        }

        // Mise à jour des positions
        player1.pos.x += player1.velX;
        player2.pos.x += player2.velX;

        if (player1.pos.x < 0) player1.pos.x = 0;
        if (player1.pos.x > LEVEL_WIDTH - PLAYER_WIDTH) player1.pos.x = LEVEL_WIDTH - PLAYER_WIDTH;
        if (player2.pos.x < 0) player2.pos.x = 0;
        if (player2.pos.x > LEVEL_WIDTH - PLAYER_WIDTH) player2.pos.x = LEVEL_WIDTH - PLAYER_WIDTH;

        if (!splitMode) {
            // Mode normal : caméra suit le joueur 1
            camera1X = player1.pos.x + PLAYER_WIDTH / 2 - SCREEN_WIDTH / 2;
            if (camera1X < 0) camera1X = 0;
            if (camera1X > LEVEL_WIDTH - SCREEN_WIDTH) camera1X = LEVEL_WIDTH - SCREEN_WIDTH;

            SDL_Rect bgSrc = {camera1X, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_Rect bgDest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_BlitSurface(background, &bgSrc, screen, &bgDest);

            SDL_Rect playerScreenPos = {player1.pos.x - camera1X, player1.pos.y, PLAYER_WIDTH, PLAYER_HEIGHT};
            SDL_BlitSurface(playerSurface, NULL, screen, &playerScreenPos);
        } else {
            // Mode split : chaque moitié de l'écran pour un joueur
            int halfWidth = SCREEN_WIDTH / 2;

            // Caméra pour chaque joueur
            camera1X = player1.pos.x + PLAYER_WIDTH / 2 - halfWidth / 2;
            if (camera1X < 0) camera1X = 0;
            if (camera1X > LEVEL_WIDTH - halfWidth) camera1X = LEVEL_WIDTH - halfWidth;

            camera2X = player2.pos.x + PLAYER_WIDTH / 2 - halfWidth / 2;
            if (camera2X < 0) camera2X = 0;
            if (camera2X > LEVEL_WIDTH - halfWidth) camera2X = LEVEL_WIDTH - halfWidth;

            // Affichage Joueur 1 (gauche)
            SDL_Rect bgSrc1 = {camera1X, 0, halfWidth, SCREEN_HEIGHT};
            SDL_Rect bgDest1 = {0, 0, halfWidth, SCREEN_HEIGHT};
            SDL_BlitSurface(background, &bgSrc1, screen, &bgDest1);

            SDL_Rect playerScreenPos1 = {player1.pos.x - camera1X, player1.pos.y, PLAYER_WIDTH, PLAYER_HEIGHT};
            SDL_BlitSurface(playerSurface, NULL, screen, &playerScreenPos1);

            // Affichage Joueur 2 (droite)
            SDL_Rect bgSrc2 = {camera2X, 0, halfWidth, SCREEN_HEIGHT};
            SDL_Rect bgDest2 = {halfWidth, 0, halfWidth, SCREEN_HEIGHT};
            SDL_BlitSurface(background, &bgSrc2, screen, &bgDest2);

            SDL_Rect playerScreenPos2 = {player2.pos.x - camera2X + halfWidth, player2.pos.y, PLAYER_WIDTH, PLAYER_HEIGHT};
            SDL_BlitSurface(playerSurface, NULL, screen, &playerScreenPos2);
        }

        SDL_Flip(screen);
        SDL_Delay(16);
    }

    // Libération des surfaces et fermeture de SDL_image
    SDL_FreeSurface(playerSurface);
    SDL_FreeSurface(background);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

