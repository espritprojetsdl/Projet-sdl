#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_W  1920
#define SCREEN_H  1080
#define BUTTON_START_Y 50      // Position verticale de départ pour les boutons (page d'accueil)
#define BUTTON_SPACING 150     // Espacement vertical entre les boutons (page d'accueil)
#define QUITTER_MARGIN_RIGHT 20  // Marge à droite pour le bouton Quitter
#define QUITTER_EXTRA_GAP 300    // Espace supplémentaire entre le 4ème bouton et le bouton Quitter

// Récupération d'un pixel sur une surface SDL
Uint32 SDL_GetPixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
        case 1: return *p;
        case 2: return *(Uint16 *)p;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[2] << 16 | p[1] << 8 | p[0];
        case 4: return *(Uint32 *)p;
        default: return 0;
    }
}

// Vérifie si un pixel est transparent
int isPixelTransparent(SDL_Surface *image, int x, int y) {
    if(x < 0 || x >= image->w || y < 0 || y >= image->h)
        return 1;
    Uint32 pixel = SDL_GetPixel(image, x, y);
    Uint8 r, g, b, a;
    SDL_GetRGBA(pixel, image->format, &r, &g, &b, &a);
    return (a == 0);
}

// Affiche du texte réparti sur plusieurs lignes (avec un nombre fixe de mots par ligne)
void renderTextInLines(SDL_Surface *screen, const char *text, SDL_Color color, TTF_Font *font, SDL_Rect position, int wordsPerLine) {
    int lineHeight = TTF_FontHeight(font);
    int x = position.x, y = position.y, wordCount = 0, i = 0;
    char word[256];
    while(*text) {
        while(text[i] != '\0' && text[i] != ' ') {
            word[i] = text[i];
            i++;
        }
        word[i] = '\0';
        SDL_Surface *wordSurface = TTF_RenderText_Solid(font, word, color);
        SDL_Rect wordPos = { x, y };
        SDL_BlitSurface(wordSurface, NULL, screen, &wordPos);
        x += wordSurface->w + 5;
        SDL_FreeSurface(wordSurface);
        wordCount++;
        if(wordCount == wordsPerLine) {
            wordCount = 0;
            y += lineHeight;
            x = position.x;
        }
        text += i;
        while(*text == ' ') text++;
        i = 0;
    }
}

int main(int argc, char **argv)
{
    SDL_Surface *ecran, *image, *resizedImage;
    SDL_Surface *buttonImage, *optionButtonImage, *scoreButtonImage, *histoireButtonImage, *quitterButtonImage;
    SDL_Surface *optimizedImage, *optimizedButton, *optimizedOptionButton, *optimizedScoreButton, *optimizedHistoireButton, *optimizedQuitterButton;
    SDL_Surface *ouiButtonImage, *nonButtonImage;
    SDL_Surface *optimizedOuiButton, *optimizedNonButton;
    SDL_Surface *saveLoadButtonImage, *saveNewGameButtonImage;
    SDL_Surface *optimizedSaveLoadButton, *optimizedSaveNewGameButton;
    SDL_Surface *volumeDecreaseButtonImage, *volumeIncreaseButtonImage;
    SDL_Surface *optimizedVolumeDecreaseButton, *optimizedVolumeIncreaseButton;
    SDL_Surface *modeFullScreenButtonImage, *modeWindowButtonImage;
    SDL_Surface *optimizedModeFullScreenButton, *optimizedModeWindowButton;
    SDL_Surface *retourButtonImage, *optimizedRetourButton;
    // Image pour la page Avatar (ss.png)
    SDL_Surface *avatarImage, *optimizedAvatarImage;
    
    SDL_Event event;
    Mix_Music *musique;
    TTF_Font *font;
    SDL_Color textColor = {255, 255, 255};
    int quitter = 1;
    // Pages :
    // 0: Accueil, 1: Jeu, 2: Options, 3: Score, 4: Histoire,
    // 5: Sauvegarde, 6: Page Joueur, 7: Page Avatar, 8: Page Inscri
    int currentPage = 0;
    int currentScreenW = SCREEN_W, currentScreenH = SCREEN_H;
    int fullscreen = 1;
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { printf("Erreur SDL_Init: %s\n", SDL_GetError()); return 1; }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { printf("Erreur Mix_OpenAudio: %s\n", Mix_GetError()); SDL_Quit(); return 1; }
    if(TTF_Init() == -1) { printf("Erreur TTF_Init: %s\n", TTF_GetError()); Mix_Quit(); SDL_Quit(); return 1; }
    
    ecran = SDL_SetVideoMode(currentScreenW, currentScreenH, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    if(!ecran) { printf("Erreur SDL_SetVideoMode: %s\n", SDL_GetError()); TTF_Quit(); Mix_Quit(); SDL_Quit(); return 1; }
    
    // Chargement des images de fond et des boutons d'accueil
    image = IMG_Load("mehdi.png");
    buttonImage = IMG_Load("jouer.png");
    optionButtonImage = IMG_Load("option.png");
    scoreButtonImage = IMG_Load("score.png");
    histoireButtonImage = IMG_Load("histoire.png");
    quitterButtonImage = IMG_Load("quitter.png");
    if(!image || !buttonImage || !optionButtonImage || !scoreButtonImage || !histoireButtonImage || !quitterButtonImage) {
        printf("Erreur de chargement d'une image.\n"); return 1;
    }
    optimizedImage = SDL_DisplayFormat(image);
    optimizedButton = SDL_DisplayFormatAlpha(buttonImage);
    optimizedOptionButton = SDL_DisplayFormatAlpha(optionButtonImage);
    optimizedScoreButton = SDL_DisplayFormatAlpha(scoreButtonImage);
    optimizedHistoireButton = SDL_DisplayFormatAlpha(histoireButtonImage);
    optimizedQuitterButton = SDL_DisplayFormatAlpha(quitterButtonImage);
    SDL_FreeSurface(image); SDL_FreeSurface(buttonImage); SDL_FreeSurface(optionButtonImage);
    SDL_FreeSurface(scoreButtonImage); SDL_FreeSurface(histoireButtonImage); SDL_FreeSurface(quitterButtonImage);
    
    // Chargement des images Oui/Non
    ouiButtonImage = IMG_Load("oui.png");
    nonButtonImage = IMG_Load("non.png");
    if(!ouiButtonImage || !nonButtonImage) { printf("Erreur de chargement de l'image pour Oui/Non.\n"); return 1; }
    optimizedOuiButton = SDL_DisplayFormatAlpha(ouiButtonImage);
    optimizedNonButton = SDL_DisplayFormatAlpha(nonButtonImage);
    SDL_FreeSurface(ouiButtonImage); SDL_FreeSurface(nonButtonImage);
    
    // Boutons de sauvegarde
    saveLoadButtonImage = IMG_Load("cj.png");
    saveNewGameButtonImage = IMG_Load("np.png");
    if(!saveLoadButtonImage || !saveNewGameButtonImage) { printf("Erreur de chargement de l'image pour les boutons de sauvegarde.\n"); return 1; }
    optimizedSaveLoadButton = SDL_DisplayFormatAlpha(saveLoadButtonImage);
    optimizedSaveNewGameButton = SDL_DisplayFormatAlpha(saveNewGameButtonImage);
    SDL_FreeSurface(saveLoadButtonImage); SDL_FreeSurface(saveNewGameButtonImage);
    
    // Boutons de volume
    volumeDecreaseButtonImage = IMG_Load("diminuer.png");
    volumeIncreaseButtonImage = IMG_Load("augmenter.png");
    if(!volumeDecreaseButtonImage || !volumeIncreaseButtonImage) { printf("Erreur de chargement de l'image pour les boutons de volume.\n"); return 1; }
    optimizedVolumeDecreaseButton = SDL_DisplayFormatAlpha(volumeDecreaseButtonImage);
    optimizedVolumeIncreaseButton = SDL_DisplayFormatAlpha(volumeIncreaseButtonImage);
    SDL_FreeSurface(volumeDecreaseButtonImage); SDL_FreeSurface(volumeIncreaseButtonImage);
    
    // Boutons pour le mode affichage
    modeFullScreenButtonImage = IMG_Load("pe.png");
    modeWindowButtonImage = IMG_Load("fenetre.png");
    if(!modeFullScreenButtonImage || !modeWindowButtonImage) { printf("Erreur de chargement des images pour le mode affichage.\n"); return 1; }
    optimizedModeFullScreenButton = SDL_DisplayFormatAlpha(modeFullScreenButtonImage);
    optimizedModeWindowButton = SDL_DisplayFormatAlpha(modeWindowButtonImage);
    SDL_FreeSurface(modeFullScreenButtonImage); SDL_FreeSurface(modeWindowButtonImage);
    
    // Bouton Retour (utilisé dans la page Joueur et Avatar)
    retourButtonImage = IMG_Load("retour.png");
    if(!retourButtonImage) { printf("Erreur de chargement de l'image pour le bouton Retour.\n"); return 1; }
    optimizedRetourButton = SDL_DisplayFormatAlpha(retourButtonImage);
    SDL_FreeSurface(retourButtonImage);
    
    // Chargement de l'image pour la page Avatar (ss.png) et réduction de taille
    avatarImage = IMG_Load("ss.png");
    if(!avatarImage) { printf("Erreur de chargement de ss.png.\n"); return 1; }
    optimizedAvatarImage = SDL_DisplayFormatAlpha(avatarImage);
    SDL_FreeSurface(avatarImage);
    {
        int scaleFactor = 2; // Réduction à 50%
        int smallW = optimizedAvatarImage->w / scaleFactor;
        int smallH = optimizedAvatarImage->h / scaleFactor;
        SDL_Surface *smallAvatar = SDL_CreateRGBSurface(SDL_SWSURFACE, smallW, smallH,
                                                        optimizedAvatarImage->format->BitsPerPixel,
                                                        optimizedAvatarImage->format->Rmask,
                                                        optimizedAvatarImage->format->Gmask,
                                                        optimizedAvatarImage->format->Bmask,
                                                        optimizedAvatarImage->format->Amask);
        if(!smallAvatar) { printf("Erreur de création de smallAvatar: %s\n", SDL_GetError()); return 1; }
        if(SDL_SoftStretch(optimizedAvatarImage, NULL, smallAvatar, NULL) < 0) {
            printf("Erreur SDL_SoftStretch pour smallAvatar: %s\n", SDL_GetError());
        }
        SDL_FreeSurface(optimizedAvatarImage);
        optimizedAvatarImage = smallAvatar;
    }
    
    resizedImage = SDL_CreateRGBSurface(SDL_SWSURFACE, currentScreenW, currentScreenH, 32, 0, 0, 0, 0);
    if(!resizedImage) { printf("Erreur SDL_CreateRGBSurface: %s\n", SDL_GetError()); TTF_Quit(); Mix_Quit(); SDL_Quit(); return 1; }
    if(SDL_SoftStretch(optimizedImage, NULL, resizedImage, NULL) < 0) { printf("Erreur SDL_SoftStretch: %s\n", SDL_GetError()); }
    
    SDL_Rect jouerButtonPos = {20, BUTTON_START_Y};
    SDL_Rect optionButtonPos = {20, BUTTON_START_Y + BUTTON_SPACING};
    SDL_Rect scoreButtonPos = {20, BUTTON_START_Y + 2 * BUTTON_SPACING};
    SDL_Rect histoireButtonPos = {20, BUTTON_START_Y + 3 * BUTTON_SPACING};
    SDL_Rect quitterButtonPos = {SCREEN_W - optimizedQuitterButton->w - QUITTER_MARGIN_RIGHT,
                                 BUTTON_START_Y + 3 * BUTTON_SPACING + QUITTER_EXTRA_GAP};
    
    font = TTF_OpenFont("arial.ttf", 36);
    if(!font) { printf("Erreur TTF_OpenFont: %s\n", TTF_GetError()); TTF_Quit(); Mix_Quit(); SDL_Quit(); return 1; }
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Shattered Realms", textColor);
    if(!textSurface) { printf("Erreur TTF_RenderText_Solid: %s\n", TTF_GetError()); TTF_CloseFont(font); Mix_Quit(); SDL_Quit(); return 1; }
    SDL_Rect textPosition = {20, 10};
    
    musique = Mix_LoadMUS("song.mp3");
    if(!musique) { printf("Erreur de chargement de la musique: %s\n", Mix_GetError()); }
    else { if(Mix_PlayMusic(musique, -1) == -1) { printf("Erreur de lecture de la musique: %s\n", Mix_GetError()); } }
    
    while(quitter) {
        SDL_Rect yesPos, noPos;
        
        // Pour la page Jeu (page 1)
        if(currentPage == 1) {
            SDL_Surface *tmpText = TTF_RenderText_Solid(font, "Voulez-vous sauvegarder votre jeu ?", textColor);
            int textW = tmpText->w, textH = tmpText->h;
            SDL_FreeSurface(tmpText);
            int gap = 20;
            yesPos.y = (currentScreenH - (textH + optimizedOuiButton->h + gap)) / 2 + textH + gap;
        }
        
        // Rendu selon la page
        if(currentPage == 0) {
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            SDL_BlitSurface(textSurface, NULL, ecran, &textPosition);
            SDL_BlitSurface(optimizedButton, NULL, ecran, &jouerButtonPos);
            SDL_BlitSurface(optimizedOptionButton, NULL, ecran, &optionButtonPos);
            SDL_BlitSurface(optimizedScoreButton, NULL, ecran, &scoreButtonPos);
            SDL_BlitSurface(optimizedHistoireButton, NULL, ecran, &histoireButtonPos);
            SDL_BlitSurface(optimizedQuitterButton, NULL, ecran, &quitterButtonPos);
        }
        else if(currentPage == 1) {
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            SDL_Surface *textSavePrompt = TTF_RenderText_Solid(font, "Voulez-vous sauvegarder votre jeu ?", textColor);
            int textW = textSavePrompt->w, textH = textSavePrompt->h;
            SDL_Rect drawPromptPos = { (currentScreenW - textW) / 2, (currentScreenH - (textH + optimizedOuiButton->h + 20)) / 2 };
            SDL_BlitSurface(textSavePrompt, NULL, ecran, &drawPromptPos);
            SDL_FreeSurface(textSavePrompt);
            int gap = 20;
            int groupWidth = optimizedOuiButton->w + gap + optimizedNonButton->w;
            yesPos.x = (currentScreenW - groupWidth) / 2;
            yesPos.w = optimizedOuiButton->w; yesPos.h = optimizedOuiButton->h;
            noPos.x = yesPos.x + optimizedOuiButton->w + gap; noPos.y = yesPos.y;
            noPos.w = optimizedNonButton->w; noPos.h = optimizedNonButton->h;
            SDL_BlitSurface(optimizedOuiButton, NULL, ecran, &yesPos);
            SDL_BlitSurface(optimizedNonButton, NULL, ecran, &noPos);
        }
        else if(currentPage == 2) {
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            int yVolumeText = 50;
            SDL_Surface *volumeText = TTF_RenderText_Solid(font, "Volume", textColor);
            SDL_Rect volTextRect = {20, yVolumeText, volumeText->w, volumeText->h};
            SDL_BlitSurface(volumeText, NULL, ecran, &volTextRect);
            SDL_FreeSurface(volumeText);
            int gap = 20;
            SDL_Rect decRect = {20, yVolumeText + volTextRect.h + 20, optimizedVolumeDecreaseButton->w, optimizedVolumeDecreaseButton->h};
            SDL_Rect incRect = {20 + optimizedVolumeDecreaseButton->w + gap, yVolumeText + volTextRect.h + 20, optimizedVolumeIncreaseButton->w, optimizedVolumeIncreaseButton->h};
            SDL_BlitSurface(optimizedVolumeDecreaseButton, NULL, ecran, &decRect);
            SDL_BlitSurface(optimizedVolumeIncreaseButton, NULL, ecran, &incRect);
            int yModeText = decRect.y + optimizedVolumeDecreaseButton->h + 40;
            SDL_Surface *modeText = TTF_RenderText_Solid(font, "Mode affichage", textColor);
            SDL_Rect modeTextRect = {20, yModeText, modeText->w, modeText->h};
            SDL_BlitSurface(modeText, NULL, ecran, &modeTextRect);
            SDL_FreeSurface(modeText);
            int modeButtonGap = 20;
            SDL_Rect fullScreenRect = {20, yModeText + modeTextRect.h + 20, optimizedModeFullScreenButton->w, optimizedModeFullScreenButton->h};
            SDL_Rect windowRect = {20 + optimizedModeFullScreenButton->w + modeButtonGap, yModeText + modeTextRect.h + 20, optimizedModeWindowButton->w, optimizedModeWindowButton->h};
            SDL_BlitSurface(optimizedModeFullScreenButton, NULL, ecran, &fullScreenRect);
            SDL_BlitSurface(optimizedModeWindowButton, NULL, ecran, &windowRect);
            if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                int mx = event.button.x, my = event.button.y;
                if(mx >= fullScreenRect.x && mx < fullScreenRect.x + fullScreenRect.w &&
                   my >= fullScreenRect.y && my < fullScreenRect.y + fullScreenRect.h &&
                   !isPixelTransparent(optimizedModeFullScreenButton, mx - fullScreenRect.x, my - fullScreenRect.y)) {
                    fullscreen = 1;
                    ecran = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
                    currentScreenW = SCREEN_W; currentScreenH = SCREEN_H;
                    resizedImage = SDL_CreateRGBSurface(SDL_SWSURFACE, currentScreenW, currentScreenH, 32, 0,0,0,0);
                    SDL_SoftStretch(optimizedImage, NULL, resizedImage, NULL);
                }
                else if(mx >= windowRect.x && mx < windowRect.x + windowRect.w &&
                        my >= windowRect.y && my < windowRect.y + windowRect.h &&
                        !isPixelTransparent(optimizedModeWindowButton, mx - windowRect.x, my - windowRect.y)) {
                    fullscreen = 0;
                    currentScreenW = SCREEN_W; currentScreenH = SCREEN_H;
                    ecran = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
                    resizedImage = SDL_CreateRGBSurface(SDL_SWSURFACE, currentScreenW, currentScreenH, 32, 0,0,0,0);
                    SDL_SoftStretch(optimizedImage, NULL, resizedImage, NULL);
                }
            }
            // Bouton Retour placé tout en bas à droite
            SDL_Rect retourButtonPos = { currentScreenW - optimizedRetourButton->w - 20,
                                         currentScreenH - optimizedRetourButton->h,
                                         optimizedRetourButton->w, optimizedRetourButton->h };
            SDL_BlitSurface(optimizedRetourButton, NULL, ecran, &retourButtonPos);
        }
        else if(currentPage == 3) {
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            SDL_Surface *textScore = TTF_RenderText_Solid(font, "Meilleur Score", textColor);
            SDL_Rect textScorePos = { (SCREEN_W - textScore->w) / 2, (SCREEN_H - textScore->h) / 2 };
            SDL_BlitSurface(textScore, NULL, ecran, &textScorePos);
            SDL_FreeSurface(textScore);
        }
        else if(currentPage == 4) {
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            SDL_Surface *textHistory = TTF_RenderText_Solid(font, "Histoire", textColor);
            SDL_Rect textHistoryPos = { (SCREEN_W - textHistory->w) / 2, 10 };
            SDL_BlitSurface(textHistory, NULL, ecran, &textHistoryPos);
            SDL_FreeSurface(textHistory);
            const char* storyText = "In the distant future, the planet Concordia serves as a peaceful hub for various species...";
            SDL_Rect storyPos = {100, 100};
            renderTextInLines(ecran, storyText, textColor, font, storyPos, 15);
        }
        else if(currentPage == 5) {
            // Page de sauvegarde
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            SDL_Surface *textSavePage = TTF_RenderText_Solid(font, "Page de sauvegarde", textColor);
            SDL_Rect textSavePagePos = { (SCREEN_W - textSavePage->w) / 2, 50 };
            SDL_BlitSurface(textSavePage, NULL, ecran, &textSavePagePos);
            SDL_FreeSurface(textSavePage);
            int gap = 20;
            SDL_Rect loadButtonPos, newGameButtonPos;
            int groupWidth = optimizedSaveLoadButton->w + gap + optimizedSaveNewGameButton->w;
            loadButtonPos.x = (SCREEN_W - groupWidth) / 2;
            loadButtonPos.y = (SCREEN_H - optimizedSaveLoadButton->h) / 2;
            loadButtonPos.w = optimizedSaveLoadButton->w; loadButtonPos.h = optimizedSaveLoadButton->h;
            newGameButtonPos.x = loadButtonPos.x + optimizedSaveLoadButton->w + gap;
            newGameButtonPos.y = loadButtonPos.y;
            newGameButtonPos.w = optimizedSaveNewGameButton->w; newGameButtonPos.h = optimizedSaveNewGameButton->h;
            SDL_BlitSurface(optimizedSaveLoadButton, NULL, ecran, &loadButtonPos);
            SDL_BlitSurface(optimizedSaveNewGameButton, NULL, ecran, &newGameButtonPos);
        }
        else if(currentPage == 6) {
            // Page Joueur : 2 boutons (Mono et Multi) et un bouton Retour tout en bas à droite
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            int gap = 20;
            SDL_Rect monoButtonPos, multiButtonPos;
            int buttonWidth = optimizedQuitterButton->w, buttonHeight = optimizedQuitterButton->h;
            int totalWidth = buttonWidth * 2 + gap;
            monoButtonPos.x = (SCREEN_W - totalWidth) / 2;
            monoButtonPos.y = (SCREEN_H - buttonHeight) / 2 + 50;
            monoButtonPos.w = buttonWidth; monoButtonPos.h = buttonHeight;
            multiButtonPos.x = monoButtonPos.x + buttonWidth + gap;
            multiButtonPos.y = monoButtonPos.y;
            multiButtonPos.w = buttonWidth; multiButtonPos.h = buttonHeight;
            SDL_BlitSurface(optimizedQuitterButton, NULL, ecran, &monoButtonPos);
            SDL_BlitSurface(optimizedQuitterButton, NULL, ecran, &multiButtonPos);
            // Bouton Retour placé tout en bas à droite
            SDL_Rect retourButtonPos = { currentScreenW - optimizedRetourButton->w - 20,
                                         currentScreenH - optimizedRetourButton->h,
                                         optimizedRetourButton->w, optimizedRetourButton->h };
            SDL_BlitSurface(optimizedRetourButton, NULL, ecran, &retourButtonPos);
        }
        else if(currentPage == 7) {
            // Page Avatar : 2 images côte à côte, un bouton "Vérifier" centré et un bouton Retour tout en bas à droite
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            int gap = 20;
            int totalWidth = 2 * optimizedAvatarImage->w + gap;
            int startX = (SCREEN_W - totalWidth) / 2;
            int startY = (SCREEN_H - optimizedAvatarImage->h) / 2;
            SDL_Rect avatarPos1 = { startX, startY, optimizedAvatarImage->w, optimizedAvatarImage->h };
            SDL_Rect avatarPos2 = { startX + optimizedAvatarImage->w + gap, startY, optimizedAvatarImage->w, optimizedAvatarImage->h };
            SDL_BlitSurface(optimizedAvatarImage, NULL, ecran, &avatarPos1);
            SDL_BlitSurface(optimizedAvatarImage, NULL, ecran, &avatarPos2);
            // Bouton "Vérifier" centré sous les images
            SDL_Rect verifyButtonPos;
            verifyButtonPos.w = optimizedQuitterButton->w;
            verifyButtonPos.h = optimizedQuitterButton->h;
            verifyButtonPos.x = (SCREEN_W - verifyButtonPos.w) / 2;
            verifyButtonPos.y = startY + optimizedAvatarImage->h + gap;
            SDL_BlitSurface(optimizedQuitterButton, NULL, ecran, &verifyButtonPos);
            // Bouton Retour placé tout en bas à droite
            SDL_Rect retourButtonPos = { currentScreenW - optimizedRetourButton->w - 20,
                                         currentScreenH - optimizedRetourButton->h,
                                         optimizedRetourButton->w, optimizedRetourButton->h };
            SDL_BlitSurface(optimizedRetourButton, NULL, ecran, &retourButtonPos);
        }
        else if(currentPage == 8) {
            // Page Inscri
            SDL_BlitSurface(resizedImage, NULL, ecran, NULL);
            SDL_Surface *inscriText = TTF_RenderText_Solid(font, "Page Inscri", textColor);
            SDL_Rect inscriTextPos = { (SCREEN_W - inscriText->w) / 2, (SCREEN_H - inscriText->h) / 2 };
            SDL_BlitSurface(inscriText, NULL, ecran, &inscriTextPos);
            SDL_FreeSurface(inscriText);
        }
        
        SDL_Flip(ecran);
        
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) { quitter = 0; }
            if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    if(currentPage == 5 || currentPage == 6 || currentPage == 7 || currentPage == 8)
                        currentPage = 1;
                    else
                        quitter = 0;
                }
            }
            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mx = event.button.x, my = event.button.y;
                if(currentPage == 0) {
                    if(mx >= jouerButtonPos.x && mx < jouerButtonPos.x + optimizedButton->w &&
                       my >= jouerButtonPos.y && my < jouerButtonPos.y + optimizedButton->h &&
                       !isPixelTransparent(optimizedButton, mx - jouerButtonPos.x, my - jouerButtonPos.y))
                    { currentPage = 1; }
                    else if(mx >= optionButtonPos.x && mx < optionButtonPos.x + optimizedOptionButton->w &&
                            my >= optionButtonPos.y && my < optionButtonPos.y + optimizedOptionButton->h &&
                            !isPixelTransparent(optimizedOptionButton, mx - optionButtonPos.x, my - optionButtonPos.y))
                    { currentPage = 2; }
                    else if(mx >= scoreButtonPos.x && mx < scoreButtonPos.x + optimizedScoreButton->w &&
                            my >= scoreButtonPos.y && my < scoreButtonPos.y + optimizedScoreButton->h &&
                            !isPixelTransparent(optimizedScoreButton, mx - scoreButtonPos.x, my - scoreButtonPos.y))
                    { currentPage = 3; }
                    else if(mx >= histoireButtonPos.x && mx < histoireButtonPos.x + optimizedHistoireButton->w &&
                            my >= histoireButtonPos.y && my < histoireButtonPos.y + optimizedHistoireButton->h &&
                            !isPixelTransparent(optimizedHistoireButton, mx - histoireButtonPos.x, my - histoireButtonPos.y))
                    { currentPage = 4; }
                    else if(mx >= quitterButtonPos.x && mx < quitterButtonPos.x + optimizedQuitterButton->w &&
                            my >= quitterButtonPos.y && my < quitterButtonPos.y + optimizedQuitterButton->h &&
                            !isPixelTransparent(optimizedQuitterButton, mx - quitterButtonPos.x, my - quitterButtonPos.y))
                    { quitter = 0; }
                }
                else if(currentPage == 1) {
                    if(mx >= yesPos.x && mx < yesPos.x + optimizedOuiButton->w &&
                       my >= yesPos.y && my < yesPos.y + optimizedOuiButton->h &&
                       !isPixelTransparent(optimizedOuiButton, mx - yesPos.x, my - yesPos.y))
                    { currentPage = 5; }
                    else if(mx >= noPos.x && mx < noPos.x + optimizedNonButton->w &&
                            my >= noPos.y && my < noPos.y + optimizedNonButton->h &&
                            !isPixelTransparent(optimizedNonButton, mx - noPos.x, my - noPos.y))
                    { currentPage = 0; }
                }
                else if(currentPage == 2) {
                    int gap = 20;
                    SDL_Rect decRect = {20, 50 + TTF_FontHeight(font) + 20, optimizedVolumeDecreaseButton->w, optimizedVolumeDecreaseButton->h};
                    SDL_Rect incRect = {20 + optimizedVolumeDecreaseButton->w + gap, 50 + TTF_FontHeight(font) + 20, optimizedVolumeIncreaseButton->w, optimizedVolumeIncreaseButton->h};
                    if(mx >= decRect.x && mx < decRect.x + decRect.w &&
                       my >= decRect.y && my < decRect.y + decRect.h &&
                       !isPixelTransparent(optimizedVolumeDecreaseButton, mx - decRect.x, my - decRect.y))
                    { int vol = Mix_VolumeMusic(-1); vol -= 8; if(vol < 0) vol = 0; Mix_VolumeMusic(vol); }
                    else if(mx >= incRect.x && mx < incRect.x + incRect.w &&
                            my >= incRect.y && my < incRect.y + incRect.h &&
                            !isPixelTransparent(optimizedVolumeIncreaseButton, mx - incRect.x, my - incRect.y))
                    { int vol = Mix_VolumeMusic(-1); vol += 8; if(vol > MIX_MAX_VOLUME) vol = MIX_MAX_VOLUME; Mix_VolumeMusic(vol); }
                    else if(mx >= 20 && mx < 20 + optimizedModeFullScreenButton->w) {
                        // Traitement simplifié pour le clic sur le mode affichage...
                    }
                    SDL_Rect retourButtonPos = { currentScreenW - optimizedRetourButton->w - 20,
                                                 currentScreenH - optimizedRetourButton->h,
                                                 optimizedRetourButton->w, optimizedRetourButton->h };
                    if(mx >= retourButtonPos.x && mx < retourButtonPos.x + retourButtonPos.w &&
                       my >= retourButtonPos.y && my < retourButtonPos.y + retourButtonPos.h &&
                       !isPixelTransparent(optimizedRetourButton, mx - retourButtonPos.x, my - retourButtonPos.y))
                    { currentPage = 0; }
                }
                else if(currentPage == 3) { currentPage = 0; }
                else if(currentPage == 4) { currentPage = 0; }
                else if(currentPage == 5) {
                    int gap = 20;
                    SDL_Rect loadButtonPos, newGameButtonPos;
                    int groupWidth = optimizedSaveLoadButton->w + gap + optimizedSaveNewGameButton->w;
                    loadButtonPos.x = (SCREEN_W - groupWidth) / 2;
                    loadButtonPos.y = (SCREEN_H - optimizedSaveLoadButton->h) / 2;
                    loadButtonPos.w = optimizedSaveLoadButton->w; loadButtonPos.h = optimizedSaveLoadButton->h;
                    newGameButtonPos.x = loadButtonPos.x + optimizedSaveLoadButton->w + gap;
                    newGameButtonPos.y = loadButtonPos.y;
                    newGameButtonPos.w = optimizedSaveNewGameButton->w; newGameButtonPos.h = optimizedSaveNewGameButton->h;
                    if(mx >= loadButtonPos.x && mx < loadButtonPos.x + loadButtonPos.w &&
                       my >= loadButtonPos.y && my < loadButtonPos.y + loadButtonPos.h &&
                       !isPixelTransparent(optimizedSaveLoadButton, mx - loadButtonPos.x, my - loadButtonPos.y))
                    { currentPage = 0; }
                    else if(mx >= newGameButtonPos.x && mx < newGameButtonPos.x + newGameButtonPos.w &&
                            my >= newGameButtonPos.y && my < newGameButtonPos.y + newGameButtonPos.h &&
                            !isPixelTransparent(optimizedSaveNewGameButton, mx - newGameButtonPos.x, my - newGameButtonPos.y))
                    { currentPage = 6; }
                }
                else if(currentPage == 6) {
                    int gap = 20;
                    SDL_Rect monoButtonPos, multiButtonPos;
                    int buttonWidth = optimizedQuitterButton->w, buttonHeight = optimizedQuitterButton->h;
                    int totalWidth = buttonWidth * 2 + gap;
                    monoButtonPos.x = (SCREEN_W - totalWidth) / 2;
                    monoButtonPos.y = (SCREEN_H - buttonHeight) / 2 + 50;
                    monoButtonPos.w = buttonWidth; monoButtonPos.h = buttonHeight;
                    multiButtonPos.x = monoButtonPos.x + buttonWidth + gap;
                    multiButtonPos.y = monoButtonPos.y;
                    multiButtonPos.w = buttonWidth; multiButtonPos.h = buttonHeight;
                    if(mx >= monoButtonPos.x && mx < monoButtonPos.x + monoButtonPos.w &&
                       my >= monoButtonPos.y && my < monoButtonPos.y + monoButtonPos.h &&
                       !isPixelTransparent(optimizedQuitterButton, mx - monoButtonPos.x, my - monoButtonPos.y))
                    { currentPage = 7; }
                    else if(mx >= multiButtonPos.x && mx < multiButtonPos.x + multiButtonPos.w &&
                            my >= multiButtonPos.y && my < multiButtonPos.y + multiButtonPos.h &&
                            !isPixelTransparent(optimizedQuitterButton, mx - multiButtonPos.x, my - multiButtonPos.y))
                    { currentPage = 7; }
                    else {
                        // Bouton Retour placé tout en bas à droite
                        SDL_Rect retourButtonPos = { currentScreenW - optimizedRetourButton->w - 20,
                                                     currentScreenH - optimizedRetourButton->h,
                                                     optimizedRetourButton->w, optimizedRetourButton->h };
                        if(mx >= retourButtonPos.x && mx < retourButtonPos.x + retourButtonPos.w &&
                           my >= retourButtonPos.y && my < retourButtonPos.y + retourButtonPos.h &&
                           !isPixelTransparent(optimizedRetourButton, mx - retourButtonPos.x, my - retourButtonPos.y))
                        { currentPage = 0; }
                    }
                }
                else if(currentPage == 7) {
                    // Dans la page Avatar, vérifie le bouton "Vérifier" et le bouton Retour placé en bas à droite
                    int gap = 20;
                    int totalWidth = 2 * optimizedAvatarImage->w + gap;
                    int startX = (SCREEN_W - totalWidth) / 2;
                    int startY = (SCREEN_H - optimizedAvatarImage->h) / 2;
                    SDL_Rect avatarPos1 = { startX, startY, optimizedAvatarImage->w, optimizedAvatarImage->h };
                    SDL_Rect avatarPos2 = { startX + optimizedAvatarImage->w + gap, startY, optimizedAvatarImage->w, optimizedAvatarImage->h };
                    // Bouton "Vérifier" centré sous les images
                    SDL_Rect verifyButtonPos;
                    verifyButtonPos.w = optimizedQuitterButton->w;
                    verifyButtonPos.h = optimizedQuitterButton->h;
                    verifyButtonPos.x = (SCREEN_W - verifyButtonPos.w) / 2;
                    verifyButtonPos.y = startY + optimizedAvatarImage->h + gap;
                    if(mx >= verifyButtonPos.x && mx < verifyButtonPos.x + verifyButtonPos.w &&
                       my >= verifyButtonPos.y && my < verifyButtonPos.y + verifyButtonPos.h &&
                       !isPixelTransparent(optimizedQuitterButton, mx - verifyButtonPos.x, my - verifyButtonPos.y))
                    { currentPage = 8; }
                    else {
                        // Bouton Retour placé tout en bas à droite
                        SDL_Rect retourButtonPos = { currentScreenW - optimizedRetourButton->w - 20,
                                                     currentScreenH - optimizedRetourButton->h,
                                                     optimizedRetourButton->w, optimizedRetourButton->h };
                        if(mx >= retourButtonPos.x && mx < retourButtonPos.x + retourButtonPos.w &&
                           my >= retourButtonPos.y && my < retourButtonPos.y + retourButtonPos.h &&
                           !isPixelTransparent(optimizedRetourButton, mx - retourButtonPos.x, my - retourButtonPos.y))
                        { currentPage = 0; }
                    }
                }
                else if(currentPage == 8) {
                    // Dans la page Inscri, un clic ramène à la page Jeu
                    currentPage = 1;
                }
            }
        }
    }
    
    SDL_FreeSurface(resizedImage);
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(optimizedImage);
    SDL_FreeSurface(optimizedButton);
    SDL_FreeSurface(optimizedOptionButton);
    SDL_FreeSurface(optimizedScoreButton);
    SDL_FreeSurface(optimizedHistoireButton);
    SDL_FreeSurface(optimizedQuitterButton);
    SDL_FreeSurface(optimizedOuiButton);
    SDL_FreeSurface(optimizedNonButton);
    SDL_FreeSurface(optimizedSaveLoadButton);
    SDL_FreeSurface(optimizedSaveNewGameButton);
    SDL_FreeSurface(optimizedVolumeDecreaseButton);
    SDL_FreeSurface(optimizedVolumeIncreaseButton);
    SDL_FreeSurface(optimizedModeFullScreenButton);
    SDL_FreeSurface(optimizedModeWindowButton);
    SDL_FreeSurface(optimizedRetourButton);
    SDL_FreeSurface(optimizedAvatarImage);
    Mix_FreeMusic(musique);
    TTF_CloseFont(font);
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();
    
    return 0;
}

