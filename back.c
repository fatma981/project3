#include "back.h"

/* ============================================================
 *  1. INIT BACKGROUND
 * ============================================================ */
void initBackground(Background *B, int niveau, int screen_w, int screen_h) {
    B->niveau = niveau;

    B->image = IMG_Load("background.png");
    if (!B->image) { 
        B->image = SDL_CreateRGBSurface(0, 3100, 2000, 32, 0, 0, 0, 0); 
        SDL_FillRect(B->image, NULL, SDL_MapRGB(B->image->format, 100, 150, 200)); 
    }
    B->bg_width = B->image->w; 
    B->bg_height = B->image->h;

    B->camera  = (SDL_Rect){0, 0, screen_w, screen_h};
    B->camera1 = (SDL_Rect){0, 0, screen_w/2, screen_h};
    B->camera2 = (SDL_Rect){0, 0, screen_w/2, screen_h};
    B->posEcran1 = (SDL_Rect){0, 0, screen_w/2, screen_h};
    B->posEcran2 = (SDL_Rect){screen_w/2, 0, screen_w/2, screen_h};

    SDL_Surface *carton = IMG_Load("carton.png");
    if (!carton) { 
        carton = SDL_CreateRGBSurface(0, 100, 20, 32, 0, 0, 0, 0); 
        SDL_FillRect(carton, NULL, SDL_MapRGB(carton->format, 139, 69, 19)); 
    }
    SDL_Surface *key = IMG_Load("key.png");
    if (!key) { 
        key = SDL_CreateRGBSurface(0, 50, 30, 32, 0, 0, 0, 0);  
        SDL_FillRect(key, NULL, SDL_MapRGB(key->format, 255, 215, 0)); 
    }
    SDL_Surface *etoile = IMG_Load("etoile.png");
    if (!etoile) { 
        etoile = SDL_CreateRGBSurface(0, 40, 40, 32, 0, 0, 0, 0);  
        SDL_FillRect(etoile, NULL, SDL_MapRGB(etoile->format, 255, 0, 0)); 
    }

    B->nbPlateformes = 0;
    int n = niveau;

    if (n == 1 || n == 2) {
        B->plateformes[B->nbPlateformes++] = (Plateforme){carton, {0, 850, 3100, 50}, 0, 1, 0, 0, 0};
    } else {
        B->plateformes[B->nbPlateformes++] = (Plateforme){carton, {0, 850, 1500, 50}, 0, 1, 0, 0, 0};
        B->plateformes[B->nbPlateformes++] = (Plateforme){carton, {2000, 850, 1100, 50}, 0, 1, 0, 0, 0};
    }

    if (n == 1) {
        for (int i = 0; i < 4; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){carton, {400 + i * 500, 700, 120, 20}, 0, 1, 0, 0, 0};
        for (int i = 0; i < 3; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){key, {800 + i * 600, 550, 50, 30}, 1, 1, 3, 0, 10};
        for (int i = 0; i < 3; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){etoile, {2000 + i * 200, 650, 40, 40}, 2, 1, 0, 0, 20};
    }
    else if (n == 2) {
        for (int i = 0; i < 6; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){carton, {300 + i * 350, 750 - i * 50, 100, 20}, 0, 1, 0, 0, 0};
        for (int i = 0; i < 3; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){key, {1500 + i * 400, 500, 50, 30}, 1, 1, 0, 3, 15};
        for (int i = 0; i < 5; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){etoile, {2200 + i * 150, 600, 40, 40}, 2, 1, 0, 0, 25};
    }
    else {
        for (int i = 0; i < 8; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){etoile, {1600 + i * 150, 700, 40, 40}, 2, 1, 0, 0, 30};
        for (int i = 0; i < 4; i++) 
            B->plateformes[B->nbPlateformes++] = (Plateforme){key, {500 + i * 300, 600, 50, 30}, 1, 1, 4, 0, 20};
    }
}

/* ============================================================
 *  2. AFFICHAGE BACKGROUND (BLIT TILED)
 * ============================================================ */
void blitTiled(SDL_Surface *ecran, SDL_Surface *img, int camX, int camY,
               int dstOffX, int dstW, int screen_h) {
    int bgW = img->w, bgH = img->h;
    int ox = camX % bgW; 
    if (ox < 0) ox += bgW;
    int oy = camY % bgH; 
    if (oy < 0) oy += bgH;
    
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 2; col++) {
            int sx = (col == 0) ? ox : 0;
            int sy = (row == 0) ? oy : 0;
            int sw = (col == 0) ? bgW - ox : ox;
            int sh = (row == 0) ? bgH - oy : oy;
            int dx = dstOffX + ((col == 0) ? 0 : bgW - ox);
            int dy = (row == 0) ? 0 : bgH - oy;
            
            if (sw > 0 && sh > 0 && dx < dstOffX + dstW && dy < screen_h) {
                SDL_Rect src = {sx, sy, sw, sh};
                SDL_Rect dst = {dx, dy, sw, sh};
                SDL_BlitSurface(img, &src, ecran, &dst);
            }
        }
    }
}

void afficherBackground(SDL_Surface *ecran, Background *B, int screen_w, int screen_h, int mode) {
    if (!B->image) return;
    int demi = screen_w / 2;
    
    if (mode == 1) {
        blitTiled(ecran, B->image, B->camera.x, B->camera.y, 0, screen_w, screen_h);
        for (int i = 0; i < B->nbPlateformes; i++) {
            if (!B->plateformes[i].actif) continue;
            SDL_Rect d = {B->plateformes[i].pos.x - B->camera.x, 
                          B->plateformes[i].pos.y - B->camera.y,
                          B->plateformes[i].pos.w, 
                          B->plateformes[i].pos.h};
            if (d.x + d.w > 0 && d.x < screen_w && d.y + d.h > 0 && d.y < screen_h)
                SDL_BlitSurface(B->plateformes[i].surface, NULL, ecran, &d);
        }
    } else {
        blitTiled(ecran, B->image, B->camera1.x, B->camera1.y, 0, demi, screen_h);
        blitTiled(ecran, B->image, B->camera2.x, B->camera2.y, demi, demi, screen_h);
        for (int i = 0; i < B->nbPlateformes; i++) {
            if (!B->plateformes[i].actif) continue;
            SDL_Rect p = B->plateformes[i].pos;
            SDL_Rect d1 = {p.x - B->camera1.x, p.y - B->camera1.y, p.w, p.h};
            SDL_Rect d2 = {demi + p.x - B->camera2.x, p.y - B->camera2.y, p.w, p.h};
            if (d1.x + d1.w > 0 && d1.x < demi && d1.y + d1.h > 0 && d1.y < screen_h)
                SDL_BlitSurface(B->plateformes[i].surface, NULL, ecran, &d1);
            if (d2.x + d2.w > demi && d2.x < screen_w && d2.y + d2.h > 0 && d2.y < screen_h)
                SDL_BlitSurface(B->plateformes[i].surface, NULL, ecran, &d2);
        }
    }
}

/* ============================================================
 *  3. INIT GUIDE & TEMPS
 * ============================================================ */
void initTempsGuide(Guide *g, Uint32 *tempsDebut, int screen_w, int screen_h) {
    *tempsDebut = SDL_GetTicks();
    int gw = 400, gh = 300;
    
    g->guide1 = IMG_Load("guide.png");
    if (!g->guide1) { 
        g->guide1 = SDL_CreateRGBSurface(0, gw, gh, 32, 0, 0, 0, 0); 
        SDL_FillRect(g->guide1, NULL, SDL_MapRGB(g->guide1->format, 255, 255, 0)); 
    }
    g->guide2 = IMG_Load("guide1.png");
    if (!g->guide2) { 
        g->guide2 = SDL_CreateRGBSurface(0, gw, gh, 32, 0, 0, 0, 0); 
        SDL_FillRect(g->guide2, NULL, SDL_MapRGB(g->guide2->format, 0, 255, 255)); 
    }
    g->guide3 = IMG_Load("guide2.png");
    if (!g->guide3) { 
        g->guide3 = SDL_CreateRGBSurface(0, gw, gh, 32, 0, 0, 0, 0); 
        SDL_FillRect(g->guide3, NULL, SDL_MapRGB(g->guide3->format, 255, 0, 255)); 
    }
    
    g->posGuide1 = (SDL_Rect){100, 50, gw, gh};
    g->posGuide2 = (SDL_Rect){100, 50, gw, gh};
    g->posGuide3 = (SDL_Rect){100, 50, gw, gh};
    g->afficher = 1;
    g->index = 0;
    g->lastTime = SDL_GetTicks();
}

/* ============================================================
 *  4. AFFICHAGE GUIDE & TEMPS
 * ============================================================ */
void afficherGuideTemps(SDL_Surface *ecran, TTF_Font *police, Guide *g, Uint32 tempsDebut, int x, int y) {
    if (police) {
        char txt[50]; 
        Uint32 e = (SDL_GetTicks() - tempsDebut) / 1000;
        sprintf(txt, "Temps: %02d:%02d", (int)(e / 60), (int)(e % 60));
        SDL_Surface *t = TTF_RenderText_Solid(police, txt, (SDL_Color){255, 255, 255});
        if (t) { 
            SDL_Rect p = {x, y, 0, 0}; 
            SDL_BlitSurface(t, NULL, ecran, &p); 
            SDL_FreeSurface(t); 
        }
    }
    
    Uint32 now = SDL_GetTicks();
    if (g->afficher) { 
        if (now - g->lastTime >= 2000) { 
            g->afficher = 0; 
            g->lastTime = now; 
        } 
    } else { 
        if (now - g->lastTime >= 6000) { 
            g->afficher = 1; 
            g->index = (g->index + 1) % 3; 
            g->lastTime = now; 
        } 
    }
    
    if (g->afficher) {
        SDL_Surface *gs = (g->index == 0) ? g->guide1 : (g->index == 1) ? g->guide2 : g->guide3;
        SDL_Rect *gp = (g->index == 0) ? &g->posGuide1 : (g->index == 1) ? &g->posGuide2 : &g->posGuide3;
        if (gs) SDL_BlitSurface(gs, NULL, ecran, gp);
    }
}

/* ============================================================
 *  5. PARTAGE ÉCRAN
 * ============================================================ */
void partageEcran(SDL_Surface *ecran, Background *B, int screen_w, int screen_h) {
    afficherBackground(ecran, B, screen_w, screen_h, 2);
}

/* ============================================================
 *  6. SCROLLING UNIQUE
 * ============================================================ */
/* ============================================================
 *  6. SCROLLING UNIQUE (version compatible avec back.h)
 * ============================================================ */
void scrolling(Background *B, int direction, int dx, int dy, int numCamera, int cameraWidth, int screen_h) {
    SDL_Rect *camera;
    int distance;
    
    /* Sélection de la caméra */
    if (numCamera == 0) {
        camera = &B->camera;
    } else if (numCamera == 1) {
        camera = &B->camera1;
    } else if (numCamera == 2) {
        camera = &B->camera2;
    } else {
        return;
    }
    
    /* Déterminer la distance à déplacer selon la direction */
    if (direction == 0) {        /* Droite */
        distance = dx;
        camera->x += distance;
    } else if (direction == 1) { /* Gauche */
        distance = dx;
        camera->x -= distance;
    } else if (direction == 2) { /* Haut */
        distance = dy;
        camera->y -= distance;
    } else if (direction == 3) { /* Bas */
        distance = dy;
        camera->y += distance;
    } else {
        return;
    }
    
    /* Gestion du scrolling infini horizontal */
    if (camera->x >= B->bg_width) {
        camera->x = camera->x % B->bg_width;
    } else if (camera->x < 0) {
        camera->x = B->bg_width - ((-camera->x) % B->bg_width);
        if (camera->x >= B->bg_width) {
            camera->x = 0;
        }
    }
    
    /* Gestion du scrolling infini vertical */
    if (camera->y >= B->bg_height) {
        camera->y = camera->y % B->bg_height;
    } else if (camera->y < 0) {
        camera->y = B->bg_height - ((-camera->y) % B->bg_height);
        if (camera->y >= B->bg_height) {
            camera->y = 0;
        }
    }
    
    /* Sécurité : garder les valeurs dans les limites */
    if (camera->x < 0) {
        camera->x = 0;
    }
    if (camera->x >= B->bg_width) {
        camera->x = B->bg_width - 1;
    }
    if (camera->y < 0) {
        camera->y = 0;
    }
    if (camera->y >= B->bg_height) {
        camera->y = B->bg_height - 1;
    }
}
