#ifndef BACK_H
#define BACK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

// Structure pour les plateformes
typedef struct {
    SDL_Surface *surface;
    SDL_Rect     pos;
    int          type;      // 0: fixe (carton), 1: mobile (key), 2: destructible (etoile)
    int          actif;
    int          vitesseX, vitesseY;
    int          valeur;
} Plateforme;

// Structure Background
typedef struct {
    SDL_Surface *image;
    SDL_Rect     camera;        // mode mono
    SDL_Rect     camera1;       // joueur 1 (mode multi)
    SDL_Rect     camera2;       // joueur 2 (mode multi)
    SDL_Rect     posEcran1;     // position écran joueur 1
    SDL_Rect     posEcran2;     // position écran joueur 2
    Plateforme   plateformes[100];
    int          nbPlateformes;
    int          bg_width;
    int          bg_height;
    int          niveau;
} Background;

// Structure Joueur
typedef struct {
    SDL_Surface *surface;
    SDL_Rect     pos;           // position relative (écran)
    SDL_Rect     posAbs;        // position absolue (monde)
    int          vies;
    int          score;
    int          actif;
    int          numero;        // 1 ou 2
    int          invincible;
    int          invincibleTimer;
} Joueur;

// Structure Guide
typedef struct {
    SDL_Surface *guide1, *guide2, *guide3;
    SDL_Rect     posGuide1, posGuide2, posGuide3;
    int          afficher;
    int          index;
    Uint32       lastTime;
} Guide;

// ========== 6 FONCTIONS ==========
void initBackground(Background *B, int niveau, int screen_w, int screen_h);
void afficherBackground(SDL_Surface *ecran, Background *B, int screen_w, int screen_h, int mode);
void initTempsGuide(Guide *g, Uint32 *tempsDebut, int screen_w, int screen_h);
void afficherGuideTemps(SDL_Surface *ecran, TTF_Font *police, Guide *g, Uint32 tempsDebut, int x, int y);
void partageEcran(SDL_Surface *ecran, Background *B, int screen_w, int screen_h);
void scrolling(Background *B, int direction, int dx, int dy, int numCamera, int cameraWidth, int screen_h);

#endif
