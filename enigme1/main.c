#include "fonction.h"
#include <stdio.h>

int main() {
    if (!initSDL()) {
        printf("SDL initialization failed\n");
        return 1;
    }
    
    // Initialisation
    SDL_Surface *screen = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!screen) {
        SDL_Quit();
        return 1;
    }
    
    // Charger les polices de tailles différentes
    TTF_Font *questionFont = loadFont("arial.ttf", QUESTION_FONT_SIZE);
    TTF_Font *optionFont = loadFont("arial.ttf", OPTION_FONT_SIZE);
    TTF_Font *scoreFont = loadFont("arial.ttf", SCORE_FONT_SIZE);
    
    if (!questionFont || !optionFont || !scoreFont) {
        printf("Failed to load fonts: %s\n", TTF_GetError());
        cleanupAll(screen, NULL, NULL, (Button){0}, questionFont, optionFont, scoreFont, NULL);
        return 1;
    }
    
    SDL_Color white = {255, 255, 255};
    
    // Charger les énigmes
    Enigme* enigmes = NULL;
    int enigmeCount = loadEnigmesFromFile("enigme.txt", &enigmes);
    if (enigmeCount == 0) {
        cleanupAll(screen, NULL, NULL, (Button){0}, questionFont, optionFont, scoreFont, NULL);
        return 1;
    }
    
    // Initialiser les composants
    Button bg = createButton(0, 0, "eng_bg.jpeg", "x");
    Button *buttons = initializeMainMenuButtons();
    if (!buttons) {
        free(enigmes);
        cleanupAll(screen, NULL, enigmes, bg, questionFont, optionFont, scoreFont, NULL);
        return 1;
    }
    
    // Les boutons sont déjà initialisés avec les bonnes positions dans initializeMainMenuButtons()
    // Pas besoin de modifier les positions ici
    
    Mix_Chunk **sounds = initializeSounds();
    if (!sounds) {
        cleanupAll(screen, buttons, enigmes, bg, questionFont, optionFont, scoreFont, NULL);
        return 1;
    }
    
    // Initialize score
    Score gameScore;
    initializeScore(&gameScore);
    
    GameState state = ENIGME_SCREEN;
    int currentEnigme = 0;
    int showResult = 0;
    int result = 0;
    Uint32 resultTime = 0;
    Uint32 questionStartTime = SDL_GetTicks(); // Initialiser le temps de départ de la première question
    
    int quit = 0;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && state == ENIGME_SCREEN && !showResult) {
                int mouseX = e.button.x;
                int mouseY = e.button.y;
                for (int i = 0; i < 3; i++) {
                    if (isMouseOverButton(buttons[i], mouseX, mouseY)) {
                        Mix_PlayChannel(-1, sounds[0], 0);
                        result = checkAnswer(&enigmes[currentEnigme], i+1);
                        
                        // Update the score when the player answers
                        updateScore(&gameScore, result);
                        
                        showResult = 1;
                        resultTime = SDL_GetTicks();
                    }
                }
            }
        }
        
        // Vérifier si le temps est écoulé pour la question actuelle
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - questionStartTime > QUESTION_TIME_LIMIT && !showResult && state == ENIGME_SCREEN) {
            // Temps écoulé, traiter comme une réponse incorrecte
            result = 0;
            
            // Update the score for timeout (incorrect answer)
            updateScore(&gameScore, 0);
            
            showResult = 1;
            resultTime = currentTime;
        }
        
        // Effacer l'écran
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        
        // Afficher le fond
        if (bg.dark) SDL_BlitSurface(bg.dark, NULL, screen, &bg.pos);
        
        // Afficher selon l'état
        if (state == ENIGME_SCREEN) {
            // Afficher la barre de temps
            render_timer_bar(screen, questionStartTime);
            
            // Display the score with the bold font
            displayScore(screen, gameScore, scoreFont);
            
            // Afficher la question avec retour à la ligne si nécessaire
            displayCurrentEnigme(screen, &enigmes[currentEnigme], questionFont, optionFont, white, SCREEN_WIDTH);
            
            // Afficher les boutons avec leur texte de réponse
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            
            // Calculer la largeur maximale disponible pour les options
            int maxOptionWidth = SCREEN_WIDTH - buttons[0].pos.x - buttons[0].pos.w - TEXT_OFFSET_X - 50;
            
            for (int i = 0; i < 3; i++) {
                Button *btn = &buttons[i];
                int hovered = isMouseOverButton(*btn, mouseX, mouseY);
                if (hovered && !btn->hovered) {
                    Mix_PlayChannel(-1, sounds[1], 0);
                    btn->hovered = 1;
                } else if (!hovered) {
                    btn->hovered = 0;
                }
                
                // Afficher le bouton
                SDL_Surface *img = (hovered && btn->light) ? btn->light : btn->dark;
                if (img) SDL_BlitSurface(img, NULL, screen, &btn->pos);
                
                // Afficher le texte de la réponse à côté du bouton avec retour à la ligne si nécessaire
                SDL_Surface *textSurface = createTextWithWordWrap(
                    enigmes[currentEnigme].options[i], 
                    optionFont, 
                    white, 
                    maxOptionWidth
                );
                
                if (textSurface) {
                    SDL_Rect textRect = {
                        btn->pos.x + btn->pos.w + TEXT_OFFSET_X, // À droite du bouton
                        btn->pos.y + (btn->pos.h - textSurface->h) / 2, // Centré verticalement
                        textSurface->w,
                        textSurface->h
                    };
                    SDL_BlitSurface(textSurface, NULL, screen, &textRect);
                    SDL_FreeSurface(textSurface);
                }
            }
            
            // Afficher le résultat si nécessaire
            if (showResult) {
                renderResult(screen, result, questionFont);
                if (SDL_GetTicks() - resultTime > 2000) {
                    showResult = 0;
                    // Passer à la question suivante uniquement après l'affichage du résultat
                    currentEnigme = (currentEnigme + 1) % enigmeCount;
                    // Réinitialiser le timer pour la nouvelle question
                    questionStartTime = SDL_GetTicks();
                }
            }
        }
        
        SDL_Flip(screen);
    }
    
    // Nettoyage
    cleanupAll(screen, buttons, enigmes, bg, questionFont, optionFont, scoreFont, sounds);
    return 0;
}
