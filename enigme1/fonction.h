#ifndef FONCTION_H
#define FONCTION_H
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

// Constants
#define QUESTION_TIME_LIMIT 30000  // 30 secondes en millisecondes
#define MAX_QUESTION_LENGTH 256
#define MAX_OPTION_LENGTH 150     // Limiter la longueur des options
#define TEXT_MARGIN 20            // Marge pour éviter les bords
#define CONTENT_AREA_WIDTH 600    // Width of the central content area
#define TEXT_OFFSET_X 20          // Space between button and text
#define SCREEN_WIDTH 1270
#define SCREEN_HEIGHT 800
#define QUESTION_FONT_SIZE 30     // Taille de police pour les questions
#define OPTION_FONT_SIZE 30       // Taille de police pour les options
#define SCORE_FONT_SIZE 34        // Taille de police pour le score (plus grande pour plus de visibilité)
#define QUESTION_Y_POSITION 200   // Position Y de la question

// Structures
typedef struct {
    SDL_Surface *dark;
    SDL_Surface *light;
    SDL_Rect pos;
    int hovered;
} Button;

typedef enum {
    MAIN_MENU,
    ENIGME_SCREEN,
    RESULT_SCREEN
} GameState;

typedef struct {
    char question[MAX_QUESTION_LENGTH];
    char options[3][MAX_OPTION_LENGTH];
    int correct_answer;
} Enigme;

typedef struct {
    int current;
    int total;
} Score;

// Initialisation
int initSDL();
SDL_Surface *createWindow(int width, int height);
Button *initializeMainMenuButtons();
Mix_Chunk **initializeSounds();
int loadEnigmesFromFile(const char* filename, Enigme** enigmes);
TTF_Font *loadFont(const char *path, int size);

// Creation
Button createButton(int x, int y, const char *dark_image, const char *light_image);
SDL_Surface *loadImage(const char *path);
SDL_Surface *renderText(const char *text, TTF_Font *font, SDL_Color color);
SDL_Surface *renderBoldText(const char *text, TTF_Font *font, SDL_Color color); // Modifié
SDL_Surface* createTextWithWordWrap(const char* text, TTF_Font* font, SDL_Color color, int maxWidth);

// Interaction
int isMouseOverButton(Button button, int mouseX, int mouseY);
int checkAnswer(Enigme* enigme, int answer);

// Affichage
void displayCurrentEnigme(SDL_Surface* screen, Enigme* enigme, TTF_Font* questionFont, TTF_Font* optionFont, SDL_Color color, int screenWidth);
void renderResult(SDL_Surface* screen, int isCorrect, TTF_Font* font);
void render_timer_bar(SDL_Surface *screen, Uint32 startTime);

// Score management
void initializeScore(Score* score);
void updateScore(Score* score, int isCorrect);
void displayScore(SDL_Surface* screen, Score score, TTF_Font* scoreFont);

// Nettoyage
void cleanup(SDL_Surface *surf);
void cleanupFonts(TTF_Font *questionFont, TTF_Font *optionFont);
void cleanupAll(SDL_Surface *screen, Button *buttons, Enigme *enigmes, Button bg, TTF_Font *questionFont, TTF_Font *optionFont, TTF_Font *scoreFont, Mix_Chunk **sounds);

#endif
