#include "fonction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        return 0;
    }
    if (TTF_Init() == -1) {
        printf("TTF Error: %s\n", TTF_GetError());
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        printf("Mixer Error: %s\n", Mix_GetError());
        return 0;
    }
    return 1;
}

SDL_Surface *createWindow(int width, int height) {
    return SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
}

Button *initializeMainMenuButtons() {
    Button *buttons = malloc(3 * sizeof(Button));
    if (!buttons) return NULL;
    
    // Initialize with specific positions as requested
    buttons[0] = createButton(291, 274, "eng_a.png", "eng_a_light.png");
    buttons[1] = createButton(291, 392, "eng_b.png", "eng_b_light.png"); 
    buttons[2] = createButton(291, 510, "eng_c.png", "eng_c_light.png");
    
    return buttons;
}

Mix_Chunk** initializeSounds() {
    Mix_Chunk** sounds = malloc(2 * sizeof(Mix_Chunk*));
    if (!sounds) return NULL;
    
    sounds[0] = Mix_LoadWAV("menuMusics/button_click.wav");
    sounds[1] = Mix_LoadWAV("menuMusics/button_hover.wav");
    
    if (!sounds[0] || !sounds[1]) {
        if (sounds[0]) Mix_FreeChunk(sounds[0]);
        if (sounds[1]) Mix_FreeChunk(sounds[1]);
        free(sounds);
        return NULL;
    }
    
    return sounds;
}

TTF_Font *loadFont(const char *path, int size) {
    TTF_Font *font = TTF_OpenFont(path, size);
    if (!font) {
        printf("Font Error: %s\n", TTF_GetError());
    }
    return font;
}

int loadEnigmesFromFile(const char* filename, Enigme** enigmes) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;
    // Compter les énigmes
    int count = 0;
    char buffer[MAX_QUESTION_LENGTH];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, "[Question]")) count++;
    }
    rewind(file);
    *enigmes = malloc(count * sizeof(Enigme));
    if (!*enigmes) {
        fclose(file);
        return 0;
    }
    // Lire les énigmes
    int current = 0;
    while (current < count && fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, "[Question]")) {
            fgets((*enigmes)[current].question, MAX_QUESTION_LENGTH, file);
            (*enigmes)[current].question[strcspn((*enigmes)[current].question, "\n")] = 0;
            fgets(buffer, sizeof(buffer), file); // [Options]
            
            for (int i = 0; i < 3; i++) {
                fgets((*enigmes)[current].options[i], MAX_OPTION_LENGTH, file);
                (*enigmes)[current].options[i][strcspn((*enigmes)[current].options[i], "\n")] = 0;
            }
            fgets(buffer, sizeof(buffer), file); // [Correct]
            fscanf(file, "%d\n\n", &(*enigmes)[current].correct_answer);
            current++;
        }
    }
    fclose(file);
    return count;
}

Button createButton(int x, int y, const char *dark_image, const char *light_image) {
    Button btn;
    btn.dark = loadImage(dark_image);
    btn.light = strcmp(light_image, "x") ? loadImage(light_image) : NULL;
    btn.pos.x = x;
    btn.pos.y = y;
    if (btn.dark) {
        btn.pos.w = btn.dark->w;
        btn.pos.h = btn.dark->h;
    }
    btn.hovered = 0;
    return btn;
}

SDL_Surface *loadImage(const char *path) {
    SDL_Surface *img = IMG_Load(path);
    if (!img) printf("Image Error: %s\n", IMG_GetError());
    return img;
}

SDL_Surface *renderText(const char *text, TTF_Font *font, SDL_Color color) {
    return TTF_RenderText_Solid(font, text, color);
}

SDL_Surface *renderBoldText(const char *text, TTF_Font *font, SDL_Color color) {
    // Utiliser la fonction standard au lieu de TTF_RenderText_Bold qui n'existe pas
    return TTF_RenderText_Solid(font, text, color);
    // Alternativement, pour une meilleure qualité:
    // return TTF_RenderText_Blended(font, text, color);
}

// Fonction pour créer du texte avec retour à la ligne automatique
SDL_Surface* createTextWithWordWrap(const char* text, TTF_Font* font, SDL_Color color, int maxWidth) {
    if (!text || !font || maxWidth <= 0) return NULL;
    
    // Pour les textes courts qui ne nécessitent pas de wrapping
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return NULL;
    
    if (surface->w <= maxWidth) {
        return surface; // Le texte s'adapte sans besoin de wrapping
    }
    
    // Libérer la surface test
    SDL_FreeSurface(surface);
    
    // Créer une surface vide pour le résultat final
    SDL_Surface* result = NULL;
    
    // Copier le texte pour le modifier
    char* textCopy = strdup(text);
    if (!textCopy) return NULL;
    
    // Variables pour les lignes
    char* lines[20];  // Maximum 20 lignes
    int lineCount = 0;
    
    // Diviser le texte en mots
    char* word = strtok(textCopy, " ");
    char currentLine[MAX_QUESTION_LENGTH] = "";
    
    while (word && lineCount < 20) {
        char testLine[MAX_QUESTION_LENGTH];
        sprintf(testLine, "%s %s", currentLine, word);
        
        // Tester si cette ligne s'adapte
        surface = TTF_RenderText_Solid(font, testLine[0] == ' ' ? testLine + 1 : testLine, color);
        if (!surface) {
            free(textCopy);
            return NULL;
        }
        
        if (surface->w > maxWidth && strlen(currentLine) > 0) {
            // La ligne ne s'adapte pas, sauvegarder la ligne actuelle
            lines[lineCount++] = strdup(currentLine);
            strcpy(currentLine, word);
        } else {
            // La ligne s'adapte encore, ajouter le mot
            if (strlen(currentLine) == 0) {
                strcpy(currentLine, word);
            } else {
                strcpy(currentLine, testLine);
            }
        }
        
        SDL_FreeSurface(surface);
        word = strtok(NULL, " ");
    }
    
    // Ajouter la dernière ligne si elle n'est pas vide
    if (strlen(currentLine) > 0 && lineCount < 20) {
        lines[lineCount++] = strdup(currentLine);
    }
    
    // Créer la surface finale
    int lineHeight = TTF_FontLineSkip(font);
    result = SDL_CreateRGBSurface(SDL_SWSURFACE, maxWidth, lineHeight * lineCount, 32, 0, 0, 0, 0);
    if (!result) {
        free(textCopy);
        for (int i = 0; i < lineCount; i++) {
            free(lines[i]);
        }
        return NULL;
    }
    
    // Définir la couleur de fond comme transparent
    SDL_FillRect(result, NULL, SDL_MapRGBA(result->format, 0, 0, 0, 0));
    
    // Rendre chaque ligne
    for (int i = 0; i < lineCount; i++) {
        surface = TTF_RenderText_Solid(font, lines[i], color);
        if (surface) {
            SDL_Rect pos = {0, i * lineHeight, surface->w, surface->h};
            SDL_BlitSurface(surface, NULL, result, &pos);
            SDL_FreeSurface(surface);
        }
        free(lines[i]);
    }
    
    free(textCopy);
    return result;
}

int isMouseOverButton(Button button, int mouseX, int mouseY) {
    return (button.dark && 
            mouseX >= button.pos.x && mouseX <= button.pos.x + button.pos.w &&
            mouseY >= button.pos.y && mouseY <= button.pos.y + button.pos.h);
}

int checkAnswer(Enigme* enigme, int answer) {
    return (answer == enigme->correct_answer);
}

void displayCurrentEnigme(SDL_Surface* screen, Enigme* enigme, TTF_Font* questionFont, TTF_Font* optionFont, SDL_Color color, int screenWidth) {
    // Afficher la question avec retour à la ligne automatique
    int questionMaxWidth = screenWidth * 0.75; // 75% de la largeur de l'écran pour centrage
    SDL_Surface* questionSurface = createTextWithWordWrap(enigme->question, questionFont, color, questionMaxWidth);
    
    if (questionSurface) {
        // Position exactement au centre horizontalement et plus bas selon la constante
        SDL_Rect questionRect = {
            (screenWidth - questionSurface->w) / 2,  // Center horizontally
            QUESTION_Y_POSITION,                     // Position définie dans les constantes
            questionSurface->w, 
            questionSurface->h
        };
        SDL_BlitSurface(questionSurface, NULL, screen, &questionRect);
        SDL_FreeSurface(questionSurface);
    }
}

void renderResult(SDL_Surface* screen, int isCorrect, TTF_Font* font) {
    SDL_Color color = isCorrect ? (SDL_Color){0, 255, 0} : (SDL_Color){255, 0, 0};
    const char* text = isCorrect ? "Correct!" : "Incorrect!";
    
    SDL_Surface* textSurface = renderBoldText(text, font, color);
    if (textSurface) {
        // Centrer le texte horizontalement et le placer en bas
        SDL_Rect textRect = {
            (screen->w - textSurface->w) / 2,
            screen->h - textSurface->h - 80,  // Adjusted for better positioning
            textSurface->w,
            textSurface->h
        };
        SDL_BlitSurface(textSurface, NULL, screen, &textRect);
        SDL_FreeSurface(textSurface);
    }
}

void render_timer_bar(SDL_Surface *screen, Uint32 startTime) {
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - startTime;
    
    // Empêcher le temps écoulé de dépasser la durée
    if (elapsedTime > QUESTION_TIME_LIMIT) elapsedTime = QUESTION_TIME_LIMIT;
    
    // Calculer le ratio de temps restant (0.0 à 1.0)
    float timeRatio = 1.0f - ((float)elapsedTime / QUESTION_TIME_LIMIT);
    
    // Position et taille de la barre - moved to left-top
    SDL_Rect barBg = {20, 20, 200, 20};  // Fond de la barre - left position
    SDL_Rect barFg = barBg;              // Premier plan (partie colorée)
    barFg.w = (int)(barBg.w * timeRatio);  // Ajuster la largeur selon le temps restant
    
    // Couleur dynamique : Vert (100%) -> Jaune (50%) -> Rouge (0%)
    Uint8 r, g, b = 0;  // Pas de bleu
    if (timeRatio > 0.5f) {  // De Vert à Jaune
        g = 255;
        r = (Uint8)(255 * (1.0f - (timeRatio - 0.5f) * 2.0f));
    } else {  // De Jaune à Rouge
        r = 255;
        g = (Uint8)(255 * (timeRatio * 2.0f));
    }
    
    // Dessiner les rectangles
    SDL_FillRect(screen, &barBg, SDL_MapRGB(screen->format, 50, 50, 50));  // Fond gris foncé
    if (barFg.w > 0) {  // Ne dessiner que si la largeur est positive
        SDL_FillRect(screen, &barFg, SDL_MapRGB(screen->format, r, g, b));
    }
}

// Score management functions
void initializeScore(Score* score) {
    score->current = 0;
    score->total = 0;
}

void updateScore(Score* score, int isCorrect) {
    score->total++;
    if (isCorrect) {
        score->current++;
    }
}

void displayScore(SDL_Surface* screen, Score score, TTF_Font* scoreFont) {
    // Using black color for score
    SDL_Color black = {0, 0, 0};
    
    char scoreText[50];
    sprintf(scoreText, "Score: %d/%d", score.current, score.total);
    
    // Utiliser le rendu en gras pour que le score soit plus visible
    SDL_Surface* scoreSurface = renderBoldText(scoreText, scoreFont, black);
    if (scoreSurface) {
        SDL_Rect scoreRect = {
            screen->w - scoreSurface->w - 20,  // Position in top-right corner with small margin
            20,                                // Small margin from top
            scoreSurface->w,
            scoreSurface->h
        };
        SDL_BlitSurface(scoreSurface, NULL, screen, &scoreRect);
        SDL_FreeSurface(scoreSurface);
    }
}

void cleanup(SDL_Surface *surf) {
    if (surf) SDL_FreeSurface(surf);
}

void cleanupFonts(TTF_Font *questionFont, TTF_Font *optionFont) {
    if (questionFont) TTF_CloseFont(questionFont);
    if (optionFont) TTF_CloseFont(optionFont);
}

void cleanupAll(SDL_Surface *screen, Button *buttons, Enigme *enigmes, Button bg, TTF_Font *questionFont, TTF_Font *optionFont, TTF_Font *scoreFont, Mix_Chunk **sounds) {
    if (buttons) {
        for (int i = 0; i < 3; i++) {
            cleanup(buttons[i].dark);
            cleanup(buttons[i].light);
        }
        free(buttons);
    }
    if (enigmes) free(enigmes);
    cleanup(bg.dark);
    
    // Cleanup fonts
    if (questionFont) TTF_CloseFont(questionFont);
    if (optionFont) TTF_CloseFont(optionFont);
    if (scoreFont) TTF_CloseFont(scoreFont);
    
    if (sounds) {
        if (sounds[0]) Mix_FreeChunk(sounds[0]);
        if (sounds[1]) Mix_FreeChunk(sounds[1]);
        free(sounds);
    }
    if (screen) SDL_FreeSurface(screen);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}
