#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "back.h"

int estBloqueParCarton(Background *B, Joueur *j, SDL_Rect *camera, int dx, int dy, int w, int h) {
    // Calcul de la nouvelle position APRÈS déplacement
    int nouveauX = j->pos.x + dx;
    int nouveauY = j->pos.y + dy;
    int fx = nouveauX + camera->x;  // Position absolue future
    int fy = nouveauY + camera->y;
    
    for (int i = 0; i < B->nbPlateformes; i++) {
        if (!B->plateformes[i].actif || B->plateformes[i].type != 0) continue;
        SDL_Rect p = B->plateformes[i].pos;
        // Vérifie la collision avec la future position
        if (fx + w > p.x && fx < p.x + p.w && fy + h > p.y && fy < p.y + p.h) 
            return 1;  // Mouvement bloqué
    }
    return 0;  // Mouvement autorisé
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() == -1 || !(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) return -1;

    int sw=1550, sh=1080, mode=0, niveau=1, continuer=1, jeuActif=0, speed=5;
    int niveauDeBloque=1, level1Complete=0;
    Uint32 tempsDebut=0;

    SDL_Window *window = SDL_CreateWindow("Jeu - Partage d'Ecran", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sw, sh, SDL_WINDOW_SHOWN);
    if (!window) return -1;
    SDL_Surface *ecran = SDL_GetWindowSurface(window);
    TTF_Font *police = TTF_OpenFont("arial.ttf", 24);

    // --- Chargement boutons ---
    SDL_Surface *btnMono = IMG_Load("mono.png");
    if (!btnMono)   { btnMono   = SDL_CreateRGBSurface(0,300,80,32,0,0,0,0); SDL_FillRect(btnMono,  NULL,SDL_MapRGB(btnMono->format,  0,200,0));   }
    SDL_Surface *btnMulti = IMG_Load("multi.png");
    if (!btnMulti)  { btnMulti  = SDL_CreateRGBSurface(0,300,80,32,0,0,0,0); SDL_FillRect(btnMulti, NULL,SDL_MapRGB(btnMulti->format,  0,0,200));   }
    SDL_Surface *btnReturn = IMG_Load("return.png");
    if (!btnReturn) { btnReturn = SDL_CreateRGBSurface(0,200,50,32,0,0,0,0); SDL_FillRect(btnReturn,NULL,SDL_MapRGB(btnReturn->format,255,165,0));  }
    SDL_Surface *btnBack = IMG_Load("return.png");
    if (!btnBack)   { btnBack   = SDL_CreateRGBSurface(0,200,50,32,0,0,0,0); SDL_FillRect(btnBack,  NULL,SDL_MapRGB(btnBack->format,  255,100,100));}
    SDL_Surface *btnLevel1 = IMG_Load("level1.png");
    if (!btnLevel1) { btnLevel1 = SDL_CreateRGBSurface(0,180,60,32,0,0,0,0); SDL_FillRect(btnLevel1,NULL,SDL_MapRGB(btnLevel1->format,100,200,100));}
    SDL_Surface *btnLevel2 = IMG_Load("level2.png");
    if (!btnLevel2) { btnLevel2 = SDL_CreateRGBSurface(0,180,60,32,0,0,0,0); SDL_FillRect(btnLevel2,NULL,SDL_MapRGB(btnLevel2->format,200,100,100));}

    SDL_Rect posLevel1={sw/2-220,300,180,60}, posLevel2={sw/2+40,300,180,60};
    SDL_Rect posMono={sw/2-150,300,300,80},   posMulti={sw/2-150,420,300,80};
    SDL_Rect posReturn={sw-220,20,200,50},     posBack={sw-220,90,200,50};

    // --- Initialisation joueurs ---
    Background B; Guide guide;
    Joueur P1, P2;

    P1.surface = IMG_Load("joueur1.png");
    if (!P1.surface) { P1.surface = SDL_CreateRGBSurface(0,60,60,32,0,0,0,0); SDL_FillRect(P1.surface,NULL,SDL_MapRGB(P1.surface->format,0,0,255)); }
    P1.pos.w=60; P1.pos.h=60; P1.pos.x=sw/2-30; P1.pos.y=550;
    P1.posAbs.x=P1.pos.x; P1.posAbs.y=P1.pos.y;
    P1.vies=3; P1.score=0; P1.actif=1; P1.numero=1; P1.invincible=0; P1.invincibleTimer=0;

    P2.surface = IMG_Load("joueur2.png");
    if (!P2.surface) { P2.surface = SDL_CreateRGBSurface(0,60,60,32,0,0,0,0); SDL_FillRect(P2.surface,NULL,SDL_MapRGB(P2.surface->format,0,255,0)); }
    P2.pos.w=60; P2.pos.h=60; P2.pos.x=100; P2.pos.y=sh-150;
    P2.posAbs.x=P2.pos.x; P2.posAbs.y=P2.pos.y;
    P2.vies=3; P2.score=0; P2.actif=1; P2.numero=2; P2.invincible=0; P2.invincibleTimer=0;

    initBackground(&B, niveau, sw, sh);
    initTempsGuide(&guide, &tempsDebut, sw, sh);
    if (B.bg_height > sh) { B.camera.y=B.bg_height-sh; B.camera1.y=B.bg_height-sh; B.camera2.y=B.bg_height-sh; }

    int keysHeld[323]={0};
    SDL_Event event;

    while (continuer) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) continuer=0;
            if (event.type == SDL_KEYDOWN) keysHeld[event.key.keysym.scancode]=1;
            if (event.type == SDL_KEYUP)   keysHeld[event.key.keysym.scancode]=0;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x=event.button.x, y=event.button.y;
                
                if (mode==0) {
                    // Vérification manuelle des clics sur les boutons
                    if (x>=posLevel1.x && x<=posLevel1.x+posLevel1.w && y>=posLevel1.y && y<=posLevel1.y+posLevel1.h) { 
                        niveau=1; mode=1; 
                    }
                    if (niveauDeBloque>=2 && level1Complete && 
                        x>=posLevel2.x && x<=posLevel2.x+posLevel2.w && y>=posLevel2.y && y<=posLevel2.y+posLevel2.h) { 
                        niveau=2; mode=1; 
                    }
                }
                else if (mode==1) {
                    if (x>=posMono.x && x<=posMono.x+posMono.w && y>=posMono.y && y<=posMono.y+posMono.h) {
                        mode=2; jeuActif=1;
                        initBackground(&B,niveau,sw,sh); initTempsGuide(&guide,&tempsDebut,sw,sh);
                        if (B.bg_height>sh) B.camera.y=B.bg_height-sh;
                        P1.pos.x=sw/2-30; P1.pos.y=sh-150; P1.vies=3; P1.score=0; P1.actif=1;
                    }
                    if (x>=posMulti.x && x<=posMulti.x+posMulti.w && y>=posMulti.y && y<=posMulti.y+posMulti.h) {
                        mode=3; jeuActif=1;
                        initBackground(&B,niveau,sw,sh); initTempsGuide(&guide,&tempsDebut,sw,sh);
                        if (B.bg_height>sh) { B.camera1.y=B.bg_height-sh; B.camera2.y=B.bg_height-sh; }
                        P1.pos.x=P2.pos.x=100; P1.pos.y=P2.pos.y=550;
                        P1.vies=P2.vies=3; P1.score=P2.score=0; P1.actif=P2.actif=1;
                    }
                    if (x>=posBack.x && x<=posBack.x+posBack.w && y>=posBack.y && y<=posBack.y+posBack.h) { 
                        mode=0; 
                    }
                }
                else if ((mode==2||mode==3) && jeuActif && 
                         x>=posReturn.x && x<=posReturn.x+posReturn.w && y>=posReturn.y && y<=posReturn.y+posReturn.h) { 
                    mode=0; jeuActif=0; 
                }
            }
            if (event.type==SDL_KEYDOWN && (mode==2||mode==3) && jeuActif) {
                if (event.key.keysym.sym==SDLK_ESCAPE) { mode=0; jeuActif=0; }
                if (event.key.keysym.sym==SDLK_g) { guide.afficher=1; guide.lastTime=SDL_GetTicks(); }
            }
        }

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format,0,0,0));

        // ===== MENU NIVEAUX =====
        if (mode==0) {
            SDL_BlitSurface(btnLevel1,NULL,ecran,&posLevel1);
            if (niveauDeBloque>=2 && level1Complete) {
                SDL_BlitSurface(btnLevel2,NULL,ecran,&posLevel2);
            } else {
                SDL_Surface *g=SDL_CreateRGBSurface(0,180,60,32,0,0,0,0);
                SDL_FillRect(g,NULL,SDL_MapRGB(g->format,80,80,80));
                SDL_BlitSurface(g,NULL,ecran,&posLevel2); SDL_FreeSurface(g);
            }
            if (police) {
                SDL_Surface *t=TTF_RenderText_Solid(police,"CHOISISSEZ UN NIVEAU",(SDL_Color){255,255,255});
                if (t) { SDL_Rect p={sw/2-100,200,0,0}; SDL_BlitSurface(t,NULL,ecran,&p); SDL_FreeSurface(t); }
            }
        }
        // ===== MENU MODES =====
        else if (mode==1) {
            SDL_BlitSurface(btnMono,NULL,ecran,&posMono);
            SDL_BlitSurface(btnMulti,NULL,ecran,&posMulti);
            SDL_BlitSurface(btnBack,NULL,ecran,&posBack);
            if (police) {
                char txt[50]; sprintf(txt,"NIVEAU %d - CHOISISSEZ VOTRE MODE",niveau);
                SDL_Surface *t=TTF_RenderText_Solid(police,txt,(SDL_Color){255,255,0});
                if (t) { SDL_Rect p={sw/2-200,200,0,0}; SDL_BlitSurface(t,NULL,ecran,&p); SDL_FreeSurface(t); }
            }
        }
        // ===== MODE MONO =====
        else if (mode==2 && jeuActif) {
            P1.pos.x=sw/2-30; P1.pos.y=sh/2-30;
            P1.posAbs.x=P1.pos.x+B.camera.x; P1.posAbs.y=P1.pos.y+B.camera.y;

            for (int i=0;i<B.nbPlateformes;i++) {
                if (!B.plateformes[i].actif || B.plateformes[i].type!=1) continue;
                B.plateformes[i].pos.x+=B.plateformes[i].vitesseX;
                B.plateformes[i].pos.y+=B.plateformes[i].vitesseY;
                if (B.plateformes[i].pos.x<=0 || B.plateformes[i].pos.x+B.plateformes[i].pos.w>=B.bg_width) B.plateformes[i].vitesseX=-B.plateformes[i].vitesseX;
                if (B.plateformes[i].pos.y<=100|| B.plateformes[i].pos.y+B.plateformes[i].pos.h>=sh-100)    B.plateformes[i].vitesseY=-B.plateformes[i].vitesseY;
            }
            if (P1.invincible && --P1.invincibleTimer<=0) P1.invincible=0;

            // Mouvement pour le mode MONO (remplace SCROLL_MONO)
            if(keysHeld[SDL_SCANCODE_UP]) {
                int ncx = 0, ncy = -speed;
                if(!estBloqueParCarton(&B, &P1, &B.camera, ncx, ncy, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 2, 0, speed, 0, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_DOWN]) {
                int ncx = 0, ncy = speed;
                if(!estBloqueParCarton(&B, &P1, &B.camera, ncx, ncy, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 3, 0, speed, 0, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_RIGHT]) {
                int ncx = speed, ncy = 0;
                if(!estBloqueParCarton(&B, &P1, &B.camera, ncx, ncy, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 0, speed, 0, 0, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_LEFT]) {
                int ncx = -speed, ncy = 0;
                if(!estBloqueParCarton(&B, &P1, &B.camera, ncx, ncy, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 1, speed, 0, 0, sw, sh);
            }
          
            P1.posAbs.x=P1.pos.x+B.camera.x; P1.posAbs.y=P1.pos.y+B.camera.y;
            for (int i=0;i<B.nbPlateformes;i++) {
                if (!B.plateformes[i].actif) continue;
                int t=B.plateformes[i].type; if (t!=1&&t!=2) continue;
                SDL_Rect pl=B.plateformes[i].pos;
                if (P1.posAbs.x<pl.x+pl.w && P1.posAbs.x+P1.pos.w>pl.x && P1.posAbs.y<pl.y+pl.h && P1.posAbs.y+P1.pos.h>pl.y) {
                    if (t==1) { P1.score+=B.plateformes[i].valeur; B.plateformes[i].actif=0; }
                    else if (t==2&&!P1.invincible) { P1.vies--; P1.invincible=1; P1.invincibleTimer=60; B.plateformes[i].actif=0; }
                }
            }
            if (niveau==1&&!level1Complete&&P1.score>=30) { level1Complete=1; niveauDeBloque=2; printf("NIVEAU 1 COMPLETE!\n"); }
            if (P1.vies<=0) P1.actif=0;

            afficherBackground(ecran,&B,sw,sh,1);
            if (P1.actif) SDL_BlitSurface(P1.surface,NULL,ecran,&P1.pos);
            afficherGuideTemps(ecran,police,&guide,tempsDebut,10,10);
            SDL_BlitSurface(btnReturn,NULL,ecran,&posReturn);
            if (police) {
                char txt[100]; sprintf(txt,"VIES: %d SCORE: %d",P1.vies,P1.score);
                SDL_Surface *t=TTF_RenderText_Solid(police,txt,(SDL_Color){255,255,255});
                if (t) { SDL_Rect p={10,80,0,0}; SDL_BlitSurface(t,NULL,ecran,&p); SDL_FreeSurface(t); }
            }
        }
        // ===== MODE MULTI =====
        else if (mode==3 && jeuActif) {
            int demi=sw/2, cx=demi/2-30;
            P1.pos.x=P2.pos.x=cx; P1.pos.y=P2.pos.y=sh/2-30;
            P1.posAbs.x=P1.pos.x+B.camera1.x; P1.posAbs.y=P1.pos.y+B.camera1.y;
            P2.posAbs.x=P2.pos.x+B.camera2.x; P2.posAbs.y=P2.pos.y+B.camera2.y;

            for (int i=0;i<B.nbPlateformes;i++) {
                if (!B.plateformes[i].actif || B.plateformes[i].type!=1) continue;
                B.plateformes[i].pos.x+=B.plateformes[i].vitesseX;
                B.plateformes[i].pos.y+=B.plateformes[i].vitesseY;
                if (B.plateformes[i].pos.x<=0 || B.plateformes[i].pos.x+B.plateformes[i].pos.w>=B.bg_width) B.plateformes[i].vitesseX=-B.plateformes[i].vitesseX;
                if (B.plateformes[i].pos.y<=100|| B.plateformes[i].pos.y+B.plateformes[i].pos.h>=sh-100)    B.plateformes[i].vitesseY=-B.plateformes[i].vitesseY;
            }
            if (P1.invincible && --P1.invincibleTimer<=0) P1.invincible=0;
            if (P2.invincible && --P2.invincibleTimer<=0) P2.invincible=0;

            // Mouvements Joueur 1 (remplace SCROLL_J)
            if(keysHeld[SDL_SCANCODE_UP]) {
                if(!estBloqueParCarton(&B, &P1, &B.camera1, 0, -speed, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 2, 0, speed, 1, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_DOWN]) {
                if(!estBloqueParCarton(&B, &P1, &B.camera1, 0, speed, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 3, 0, speed, 1, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_RIGHT]) {
                if(!estBloqueParCarton(&B, &P1, &B.camera1, speed, 0, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 0, speed, 0, 1, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_LEFT]) {
                if(!estBloqueParCarton(&B, &P1, &B.camera1, -speed, 0, P1.pos.w, P1.pos.h)) 
                    scrolling(&B, 1, speed, 0, 1, sw, sh);
            }
            
            // Mouvements Joueur 2
            if(keysHeld[SDL_SCANCODE_KP_8]) {
                if(!estBloqueParCarton(&B, &P2, &B.camera2, 0, -speed, P2.pos.w, P2.pos.h)) 
                    scrolling(&B, 2, 0, speed, 2, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_KP_5]) {
                if(!estBloqueParCarton(&B, &P2, &B.camera2, 0, speed, P2.pos.w, P2.pos.h)) 
                    scrolling(&B, 3, 0, speed, 2, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_KP_6]) {
                if(!estBloqueParCarton(&B, &P2, &B.camera2, speed, 0, P2.pos.w, P2.pos.h)) 
                    scrolling(&B, 0, speed, 0, 2, sw, sh);
            }
            if(keysHeld[SDL_SCANCODE_KP_4]) {
                if(!estBloqueParCarton(&B, &P2, &B.camera2, -speed, 0, P2.pos.w, P2.pos.h)) 
                    scrolling(&B, 1, speed, 0, 2, sw, sh);
            }

            P1.posAbs.x=P1.pos.x+B.camera1.x; P1.posAbs.y=P1.pos.y+B.camera1.y;
            P2.posAbs.x=P2.pos.x+B.camera2.x; P2.posAbs.y=P2.pos.y+B.camera2.y;
            for (int i=0;i<B.nbPlateformes;i++) {
                if (!B.plateformes[i].actif) continue;
                int t=B.plateformes[i].type; if (t!=1&&t!=2) continue;
                SDL_Rect pl=B.plateformes[i].pos;
                if (P1.posAbs.x<pl.x+pl.w && P1.posAbs.x+P1.pos.w>pl.x && P1.posAbs.y<pl.y+pl.h && P1.posAbs.y+P1.pos.h>pl.y) {
                    if (t==1) { P1.score+=B.plateformes[i].valeur; B.plateformes[i].actif=0; }
                    else if (t==2&&!P1.invincible) { P1.vies--; P1.invincible=1; P1.invincibleTimer=60; B.plateformes[i].actif=0; }
                }
            }
            for (int i=0;i<B.nbPlateformes;i++) {
                if (!B.plateformes[i].actif) continue;
                int t=B.plateformes[i].type; if (t!=1&&t!=2) continue;
                SDL_Rect pl=B.plateformes[i].pos;
                if (P2.posAbs.x<pl.x+pl.w && P2.posAbs.x+P2.pos.w>pl.x && P2.posAbs.y<pl.y+pl.h && P2.posAbs.y+P2.pos.h>pl.y) {
                    if (t==1) { P2.score+=B.plateformes[i].valeur; B.plateformes[i].actif=0; }
                    else if (t==2&&!P2.invincible) { P2.vies--; P2.invincible=1; P2.invincibleTimer=60; B.plateformes[i].actif=0; }
                }
            }
            if (niveau==1&&!level1Complete&&(P1.score+P2.score)>=30) { level1Complete=1; niveauDeBloque=2; printf("NIVEAU 1 COMPLETE!\n"); }
            if (P1.vies<=0) P1.actif=0;
            if (P2.vies<=0) P2.actif=0;

            afficherBackground(ecran,&B,sw,sh,2);
            if (P1.actif) SDL_BlitSurface(P1.surface,NULL,ecran,&P1.pos);
            if (P2.actif) {
                SDL_Rect pos2={demi+P2.pos.x, P2.pos.y, P2.pos.w, P2.pos.h};
                if (pos2.x+pos2.w>sw) pos2.x=sw-pos2.w;
                if (pos2.x<demi) pos2.x=demi;
                SDL_BlitSurface(P2.surface,NULL,ecran,&pos2);
            }
            afficherGuideTemps(ecran,police,&guide,tempsDebut,10,10);
            SDL_BlitSurface(btnReturn,NULL,ecran,&posReturn);
            if (police) {
                char txt[100]; sprintf(txt,"J1 VIES:%d SCORE:%d | J2 VIES:%d SCORE:%d",P1.vies,P1.score,P2.vies,P2.score);
                SDL_Surface *t=TTF_RenderText_Solid(police,txt,(SDL_Color){0,255,255});
                if (t) { SDL_Rect p={10,80,0,0}; SDL_BlitSurface(t,NULL,ecran,&p); SDL_FreeSurface(t); }
            }
        }

        SDL_UpdateWindowSurface(window); SDL_Delay(16);
    }

    SDL_FreeSurface(btnMono); SDL_FreeSurface(btnMulti); SDL_FreeSurface(btnReturn);
    SDL_FreeSurface(btnBack); SDL_FreeSurface(btnLevel1); SDL_FreeSurface(btnLevel2);
    SDL_FreeSurface(P1.surface); SDL_FreeSurface(P2.surface);
    if (police) TTF_CloseFont(police);
    TTF_Quit(); IMG_Quit(); SDL_DestroyWindow(window); SDL_Quit();
    return 0;
}
