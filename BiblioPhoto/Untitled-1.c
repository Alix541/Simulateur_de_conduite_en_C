#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>


typedef enum {
  MENU_PRINCIPAL,
  MANOEUVRE,
  AJOUTER_DEPUTE,
  SUPPRIMER_DEPUTE,
  RECHERCHE_DEPUTE
} EtatLogiciel;

typedef struct {
  SDL_Texture *texture;
  SDL_Rect rect;
  SDL_Point center;
  int rotationDeg;
  int layer;
} LayeredImage;

#define MAX_IMAGELAYER 100
LayeredImage LayeredImageTab[MAX_IMAGELAYER];
int ImageNb = 0;

typedef struct {
  float x;
  float y;
} Point;

typedef struct {
  Point start;
  Point end;
} Segment;

Point transforme(Point p, double speedx, double speedy, float tournerRad,
                 float centreX, float centreY, SDL_Rect ObjetCreneau[30],
                 int LevierPos, int VolantPos, double NewTournerRad) {
  Point PointTransform;
  float tournerRadd = 0;

  if (VolantPos == 0 && LevierPos == 2) {
    tournerRadd = -0.06;
  }
  if (VolantPos == 0 && LevierPos == 1) {
    tournerRadd = 0.06;
  }
  if (VolantPos == 1) {
    tournerRadd;
  }
  if (VolantPos == 2 && LevierPos == 2) {
    tournerRadd = 0.06;
  }
  if (VolantPos == 2 && LevierPos == 1) {
    tournerRadd = -0.06;
  }

  if (LevierPos == 1) {
    PointTransform.x = (p.x - centreX) * cos(tournerRadd) -
                       (p.y - centreY) * sin(tournerRadd) + centreX -
                       15 * cos(tournerRad);
    PointTransform.y = (p.x - centreX) * sin(tournerRadd) +
                       (p.y - centreY) * cos(tournerRadd) + centreY -
                       15 * sin(tournerRad);
  }
  if (LevierPos == 2) {
    PointTransform.x = (p.x - centreX) * cos(tournerRadd) -
                       (p.y - centreY) * sin(tournerRadd) + centreX +
                       15 * cos(tournerRad);
    PointTransform.y = (p.x - centreX) * sin(tournerRadd) +
                       (p.y - centreY) * cos(tournerRadd) + centreY +
                       15 * sin(tournerRad);
  }
  // PointTransform.x = ObjetCreneau[4].x+(float)(ObjetCreneau[4].w*(0.25)) +
  // speedx; PointTransform.y =
  // ObjetCreneau[4].y+(float)(ObjetCreneau[4].h*(0.85)) + speedy;
  return PointTransform;
}

/*
void CalculSegment(Segment segment[3],SDL_Point* Point[3]){
    for(int i=0;i<4;i++){
        segment[i]->start=Point[i];
        segment[i]->end=Point[i];
    }
}

//Calcul de la Boite Englobante si besoin
void CalculateBounding(float* min_x,float* max_x,float* min_y,float*
max_y,SDL_Point* Point[3]){ *min_x=*max_x=Point[0].x; *min_y=*max_y=Point[0].y;
    for(int i=1;i<4;i++){
        if(Point[i].x>*max_x){max_x=Point[i].x;}
        if(Point[i].x<*min_x){min_x=Point[i].x;}
        if(Point[i].y>*max_y){max_y=Point[i].y;}
        if(Point[i].y<*min_y){min_y=Point[i].y;}
    }
}
*/

void addLayeredImage(SDL_Texture *textureImage, SDL_Rect RectImagee,
                     SDL_Point *centerImage, int rotationImage, int Layer) {
  if (ImageNb < MAX_IMAGELAYER) {
    LayeredImageTab[ImageNb].texture = textureImage;
    LayeredImageTab[ImageNb].rect = RectImagee;
    if (centerImage) {
      LayeredImageTab[ImageNb].center = *centerImage;
    } else {
      LayeredImageTab[ImageNb].center.x = RectImagee.w / 2;
      LayeredImageTab[ImageNb].center.y = RectImagee.h / 2;
    }
    LayeredImageTab[ImageNb].rotationDeg = rotationImage;
    LayeredImageTab[ImageNb].layer = Layer;
    ImageNb++;
  }
}

void trierImage() {
  for (int i = 0; i < ImageNb - 1; i++) {
    for (int j = i + 1; j < ImageNb; j++) {
      if (LayeredImageTab[i].layer > LayeredImageTab[j].layer) {
        LayeredImage temp = LayeredImageTab[i];
        LayeredImageTab[i] = LayeredImageTab[j];
        LayeredImageTab[j] = temp;
      }
    }
  }
}

void afficherImage(SDL_Renderer *renderer) {
  trierImage();
  for (int i = 0; i < ImageNb; i++) {
    SDL_RenderCopyEx(renderer, LayeredImageTab[i].texture, NULL,
                     &LayeredImageTab[i].rect, LayeredImageTab[i].rotationDeg,
                     &LayeredImageTab[i].center, SDL_FLIP_NONE);
  }
}

void cleanUp() {
  for (int i = 0; i < ImageNb; i++) {
    SDL_DestroyTexture(LayeredImageTab[i].texture);
    LayeredImageTab[i].texture = NULL;
  }
  ImageNb = 0;
}

bool wait(int attente, bool *waitbool, int *waitCompte, Uint32 Delatime,
          bool stop) {
  if (stop) {
    *waitbool = false;
    return *waitbool;
  }

  if (*waitCompte >= attente) {
    *waitbool = true;
  } else {
    (*waitCompte) += 1 * Delatime;
  }
  return *waitbool;
}

void AjusterImageVertical(SDL_Rect *Rect, SDL_Surface *image, int window_width,
                          int window_height, float dimension) {
  float ratio = (float)image->h / (float)image->w;
  int new_width = window_width * dimension;
  int new_height = new_width * ratio;

  Rect->w = new_width;
  Rect->h = new_height;
}

void AjusterImageVertical2(SDL_Rect *Rect, SDL_Texture *texture,
                           int window_width, int window_height,
                           float dimension) {
  int image_width, image_height;

  SDL_QueryTexture(texture, NULL, NULL, &image_width, &image_height);

  float ratio = (float)image_height / (float)image_width;
  int new_width = window_width * dimension;
  int new_height = new_width * ratio;

  Rect->w = new_width;
  Rect->h = new_height;
}

void AnimVolant(float *VolantRad, int VolantPos) {
  switch (VolantPos) {
  case 0:
    if (*VolantRad >= -9.424777) {
      *VolantRad -= 0.5;
    }
    if (*VolantRad <= -9.424777) {
      *VolantRad = -9.424777;
    }
    break;
  case 1:
    if (*VolantRad > 0.3) {
      *VolantRad -= 0.5;
    } else if (*VolantRad < -0.3) {
      *VolantRad += 0.5;
    }
    if (*VolantRad >= -0.3 && *VolantRad <= 0.3) {
      *VolantRad = 0;
    }
    break;
  case 2:
    if (*VolantRad <= 9.424777) {
      *VolantRad += 0.5;
    }
    if (*VolantRad >= 9.424777) {
      *VolantRad = 9.424777;
    }
    break;
  }
}

void EcrireMenu(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font,
                float y, const char *texte, SDL_Rect BoutonMenue[10],
                int place) {
  char EcritTrieur[] = "Trieur";
  int window_width, window_height;
  SDL_GetWindowSize(window, &window_width, &window_height);

  SDL_Color textColor = {0, 0, 0, 255};
  SDL_Surface *textSurface = TTF_RenderText_Blended(font, texte, textColor);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);

  SDL_Rect textRect;
  textRect.w = textSurface->w;
  textRect.h = textSurface->h;
  textRect.x = (window_width - textRect.w) / 2;
  textRect.y = window_height * y;

  BoutonMenue[place] = textRect;

  SDL_FreeSurface(textSurface);
  SDL_RenderCopy(renderer, texture, NULL, &textRect);
  SDL_DestroyTexture(texture);
}

void Ecrire(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font, float y,
            float x, const char *texte, SDL_Rect EcritTab[10], int place,
            SDL_Surface *image) {
  char EcritTrieur[] = "Trieur";
  int window_width, window_height;
  SDL_GetWindowSize(window, &window_width, &window_height);

  SDL_Color textColor = {0, 0, 0, 255};
  SDL_Surface *textSurface = TTF_RenderText_Blended(font, texte, textColor);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);

  SDL_Rect textRect;
  textRect.w = textSurface->w;
  textRect.h = textSurface->h;
  textRect.x = window_width * x;
  textRect.y = window_height * y;

  AjusterImageVertical(&textRect, textSurface, window_width, window_height,
                       0.15);

  EcritTab[place] = textRect;

  SDL_FreeSurface(textSurface);
  SDL_RenderCopy(renderer, texture, NULL, &textRect);
  SDL_DestroyTexture(texture);
}

void creneau(SDL_Surface *image, SDL_Renderer *renderer, int window_width,
             int window_height, SDL_Rect ObjetCreneau[30], bool *CreneauON,
             double speedx, float tournerRad, double speedy,
             Point VoiturePoint[4], Point *Centre, SDL_Point *center,
             float *tournerDeg) {
  image = IMG_Load("PhotoProg/Decor/parkingCreneau.png");
  SDL_Texture *textureParkingCrenaux =
      SDL_CreateTextureFromSurface(renderer, image);
  SDL_SetTextureScaleMode(textureParkingCrenaux, SDL_ScaleModeLinear);

  SDL_Rect ImageRect = {window_width * 0.02, window_height * 0.10, 1, 0.6};
  AjusterImageVertical2(&ImageRect, textureParkingCrenaux, window_width,
                        window_height, 0.25);
  ObjetCreneau[4] = ImageRect;

  SDL_FreeSurface(image);
  addLayeredImage(textureParkingCrenaux, ImageRect, NULL, 0, 0);

  image = IMG_Load("PhotoProg/Voiture/VoitPrincipalVertical.png");
  SDL_Texture *textureVoitureJaune =
      SDL_CreateTextureFromSurface(renderer, image);
  SDL_SetTextureScaleMode(textureVoitureJaune, SDL_ScaleModeLinear);

  SDL_Rect VoitureRect = {ImageRect.x + (float)(ImageRect.w * (0.25)) + speedx,
                          ImageRect.y + (float)(ImageRect.h * (0.85)) + speedy,
                          image->w, image->h};

  AjusterImageVertical(&VoitureRect, image, window_width, window_height, 0.095);
  ObjetCreneau[3] = VoitureRect;

  SDL_FreeSurface(image);
  *tournerDeg = tournerRad * (180 / M_PI);
  center->x = VoitureRect.w * (7.0 / 10.0);
  center->y = VoitureRect.h / 2;

  Centre->x = ObjetCreneau[3].x + ObjetCreneau[3].w * (7.0 / 10.0);
  Centre->y = ObjetCreneau[3].y + ObjetCreneau[3].h / 2;

  static bool first = true;

  if (first) {
    VoiturePoint[0].x = ObjetCreneau[3].x;
    VoiturePoint[0].y = ObjetCreneau[3].y;

    VoiturePoint[1].x = ObjetCreneau[3].x + ObjetCreneau[3].w;
    VoiturePoint[1].y = ObjetCreneau[3].y;

    VoiturePoint[2].x = ObjetCreneau[3].x;
    VoiturePoint[2].y = ObjetCreneau[3].y + ObjetCreneau[3].h;

    VoiturePoint[3].x = ObjetCreneau[3].x + ObjetCreneau[3].w;
    VoiturePoint[3].y = ObjetCreneau[3].y + ObjetCreneau[3].h;
    first = false;
  }

  addLayeredImage(textureVoitureJaune, VoitureRect, center, *tournerDeg, 2);

  image = IMG_Load("PhotoProg/Voiture/voitureBlanche.png");
  SDL_Texture *textureVoitureRouge =
      SDL_CreateTextureFromSurface(renderer, image);
  SDL_SetTextureScaleMode(textureVoitureRouge, SDL_ScaleModeLinear);

  SDL_Rect VoitureRougeRect = {ImageRect.x + ImageRect.w * 0.69,
                               ImageRect.y + ImageRect.h * 0.324, image->w,
                               image->h};
  AjusterImageVertical(&VoitureRougeRect, image, window_width, window_height,
                       0.096);
  ObjetCreneau[5] = VoitureRougeRect;

  SDL_FreeSurface(image);
  addLayeredImage(textureVoitureRouge, VoitureRougeRect, 0, -90, 1);

  image = IMG_Load("PhotoProg/Voiture/voitureBleu.png");
  SDL_Texture *textureVoitureBleu =
      SDL_CreateTextureFromSurface(renderer, image);
  SDL_SetTextureScaleMode(textureVoitureBleu, SDL_ScaleModeLinear);

  SDL_Rect VoitureBleuRect = {ImageRect.x + ImageRect.w * 0.695,
                              ImageRect.y + ImageRect.h * 0.07, image->w,
                              image->h};
  AjusterImageVertical(&VoitureBleuRect, image, window_width, window_height,
                       0.094);
  ObjetCreneau[6] = VoitureBleuRect;

  SDL_FreeSurface(image);
  addLayeredImage(textureVoitureBleu, VoitureBleuRect, 0, -90, 1);

  /*
  image = IMG_Load("PhotoProg/voitureverte.png");
  SDL_Texture* textureVoitureVerte = SDL_CreateTextureFromSurface(renderer,
  image); SDL_SetTextureScaleMode(textureVoitureVerte, SDL_ScaleModeLinear);

  SDL_Rect VoitureVerteRect={ImageRect.x+ImageRect.w*0.71,
                             ImageRect.y+ImageRect.h*0.765,
                             image->w,
                             image->h};
  AjusterImageVertical(&VoitureVerteRect,image,window_width,window_height,0.043);
  ObjetCreneau[7]=VoitureVerteRect;

  SDL_FreeSurface(image);
  SDL_RenderCopy(renderer, textureVoitureVerte, NULL, &VoitureVerteRect);
  SDL_DestroyTexture(textureVoitureVerte);
  */

  if (*CreneauON == false) {
    image = IMG_Load("PhotoProg/BoutonDiverse/BoutonPlay.png");
    SDL_Texture *texturePlay = SDL_CreateTextureFromSurface(renderer, image);

    SDL_Rect RectPlay = {ImageRect.x + ImageRect.w + ImageRect.w * 0.15,
                         ImageRect.y + ImageRect.h * 0.4, image->w, image->h};
    AjusterImageVertical(&RectPlay, image, window_width, window_height, 0.075);
    ObjetCreneau[2] = RectPlay;

    SDL_FreeSurface(image);
    addLayeredImage(texturePlay, RectPlay, NULL, 0, 1);
  }
}

void BoutonBougerr(SDL_Surface *image, SDL_Renderer *renderer,
                   SDL_Rect BoutonBouger[10], SDL_Rect PhotoReferente[10],
                   int window_width, int window_height, int LevierPos,
                   float *VolantRad, int VolantPos, int freinAmainPos) {
  image = IMG_Load("PhotoProg/Commande/volant.png");
  SDL_Texture *textureVolant = SDL_CreateTextureFromSurface(renderer, image);
  SDL_SetTextureScaleMode(textureVolant, SDL_ScaleModeLinear);

  SDL_Rect VolantRect = {PhotoReferente[4].x + PhotoReferente[4].w * 1.1,
                         PhotoReferente[4].y + PhotoReferente[4].h * 0.3,
                         image->w, image->h};
  AjusterImageVertical(&VolantRect, image, window_width, window_height, 0.1);
  BoutonBouger[0] = VolantRect;

  SDL_FreeSurface(image);
  AnimVolant(VolantRad, VolantPos);
  double VolantDeg = *VolantRad * (180 / M_PI);
  SDL_RenderCopyEx(renderer, textureVolant, NULL, &VolantRect, VolantDeg, NULL,
                   SDL_FLIP_NONE);
  SDL_DestroyTexture(textureVolant);

  image = IMG_Load("PhotoProg/Commande/Accelerateur.png");
  SDL_Texture *textureAccelerateur =
      SDL_CreateTextureFromSurface(renderer, image);
  SDL_SetTextureScaleMode(textureAccelerateur, SDL_ScaleModeLinear);

  SDL_Rect AccelerateurRect = {VolantRect.x + VolantRect.w * 0.7,
                               VolantRect.y + VolantRect.h * 1.25, image->w,
                               image->h};
  AjusterImageVertical(&AccelerateurRect, image, window_width, window_height,
                       0.02);
  BoutonBouger[1] = AccelerateurRect;

  SDL_FreeSurface(image);
  SDL_RenderCopy(renderer, textureAccelerateur, NULL, &AccelerateurRect);
  SDL_DestroyTexture(textureAccelerateur);

  if (LevierPos == 0) {
    image = IMG_Load("PhotoProg/Commande/levier0.png");
    SDL_Texture *textureLevier0 = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureLevier0, SDL_ScaleModeLinear);

    SDL_Rect LevierVitesseRect = {VolantRect.x + VolantRect.w * 1.2,
                                  VolantRect.y + VolantRect.h * 0.35,
                                  image->w * 0.8, image->h * 0.8};
    AjusterImageVertical(&LevierVitesseRect, image, window_width, window_height,
                         0.035);
    BoutonBouger[2] = LevierVitesseRect;

    SDL_FreeSurface(image);
    SDL_RenderCopy(renderer, textureLevier0, NULL, &LevierVitesseRect);
    SDL_DestroyTexture(textureLevier0);
  }
  if (LevierPos == 1) {
    image = IMG_Load("PhotoProg/Commande/levier1.png");
    SDL_Texture *textureLevier1 = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureLevier1, SDL_ScaleModeLinear);

    SDL_Rect LevierVitesseRect = {VolantRect.x + VolantRect.w * 1.2,
                                  VolantRect.y + VolantRect.h * 0.35,
                                  image->w * 0.8, image->h * 0.8};
    AjusterImageVertical(&LevierVitesseRect, image, window_width, window_height,
                         0.035);
    BoutonBouger[2] = LevierVitesseRect;

    SDL_FreeSurface(image);
    SDL_RenderCopy(renderer, textureLevier1, NULL, &LevierVitesseRect);
    SDL_DestroyTexture(textureLevier1);
  }
  if (LevierPos == 2) {
    image = IMG_Load("PhotoProg/Commande/levier2.png");
    SDL_Texture *textureLevier2 = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureLevier2, SDL_ScaleModeLinear);

    SDL_Rect LevierVitesseRect = {VolantRect.x + VolantRect.w * 1.2,
                                  VolantRect.y + VolantRect.h * 0.35,
                                  image->w * 0.8, image->h * 0.8};
    AjusterImageVertical(&LevierVitesseRect, image, window_width, window_height,
                         0.035);
    BoutonBouger[2] = LevierVitesseRect;

    SDL_FreeSurface(image);
    SDL_RenderCopy(renderer, textureLevier2, NULL, &LevierVitesseRect);
    SDL_DestroyTexture(textureLevier2);
  }

  SDL_Rect FreinAmainPosRect = {VolantRect.x + VolantRect.w * 1.26,
                                VolantRect.y + VolantRect.h * 1, image->w,
                                image->h};
  BoutonBouger[3] = FreinAmainPosRect;

  if (freinAmainPos == 0) {
    image = IMG_Load("PhotoProg/Commande/FreinAmainPos0.png");
    SDL_Texture *textureFreinAmainPos0 =
        SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFreinAmainPos0, SDL_ScaleModeLinear);

    AjusterImageVertical(&FreinAmainPosRect, image, window_width, window_height,
                         0.027);

    SDL_FreeSurface(image);
    SDL_RenderCopy(renderer, textureFreinAmainPos0, NULL, &FreinAmainPosRect);
    SDL_DestroyTexture(textureFreinAmainPos0);
  }
  if (freinAmainPos == 1) {
    image = IMG_Load("PhotoProg/Commande/FreinAmainPos1.png");
    SDL_Texture *textureFreinAmainPos1 =
        SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFreinAmainPos1, SDL_ScaleModeLinear);

    AjusterImageVertical(&FreinAmainPosRect, image, window_width, window_height,
                         0.027);

    SDL_FreeSurface(image);
    SDL_RenderCopy(renderer, textureFreinAmainPos1, NULL, &FreinAmainPosRect);
    SDL_DestroyTexture(textureFreinAmainPos1);
  }
}

void DetectionCollision(SDL_Rect BoutonBouger[10], SDL_Rect ObjetCreneau[30],
                        bool *VoitureSortiGauche, bool *VoitureSortiDroite) {
  if (ObjetCreneau[3].x + ObjetCreneau[3].w >=
      ObjetCreneau[4].x + ObjetCreneau[4].w) {
    *VoitureSortiDroite = true;
  } else {
    *VoitureSortiDroite = false;
  }
  if (ObjetCreneau[3].x <= ObjetCreneau[4].x) {
    *VoitureSortiGauche = true;
  } else {
    *VoitureSortiGauche = false;
  }
}

void GestionCommandes(SDL_Event event, bool *EnCour, int *mouseX, int *mouseY,
                      SDL_Rect BoutonMenue[10], SDL_Rect ObjetCreneau[30],
                      EtatLogiciel *Etat, SDL_Rect BoutonBouger[10],
                      bool *CrenauON, double *speedx, double *speedy,
                      float *tournerRad, int *LevierPos, int *freinAmainPos,
                      int *VolantPos, bool *BoutonGauchePresser,
                      bool *afficherValid, int *ValidationPos) {
  if (event.type == SDL_QUIT) {
    *EnCour = false;
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      SDL_GetMouseState(mouseX, mouseY);
      *BoutonGauchePresser = true;
      if (*Etat == 0) {
        if (*mouseX >= BoutonMenue[2].x &&
            *mouseX <= BoutonMenue[2].x + BoutonMenue[2].w &&
            *mouseY >= BoutonMenue[2].y &&
            *mouseY <= BoutonMenue[2].y + BoutonMenue[2].h) {
          *EnCour = false;
        }
        if (*mouseX >= BoutonMenue[0].x &&
            *mouseX <= BoutonMenue[0].x + BoutonMenue[0].w &&
            *mouseY >= BoutonMenue[0].y &&
            *mouseY <= BoutonMenue[0].y + BoutonMenue[0].h) {
          *Etat = MANOEUVRE;
        }
        if (*mouseX >= BoutonMenue[1].x &&
            *mouseX <= BoutonMenue[1].x + BoutonMenue[1].w &&
            *mouseY >= BoutonMenue[1].y &&
            *mouseY <= BoutonMenue[1].y + BoutonMenue[1].h) {
          *Etat = RECHERCHE_DEPUTE;
        }
        if (*mouseX >= BoutonMenue[3].x &&
            *mouseX <= BoutonMenue[3].x + BoutonMenue[3].w &&
            *mouseY >= BoutonMenue[3].y &&
            *mouseY <= BoutonMenue[3].y + BoutonMenue[3].h) {
          *Etat = SUPPRIMER_DEPUTE;
        }
        if (*mouseX >= BoutonMenue[4].x &&
            *mouseX <= BoutonMenue[4].x + BoutonMenue[4].w &&
            *mouseY >= BoutonMenue[4].y &&
            *mouseY <= BoutonMenue[4].y + BoutonMenue[4].h) {
          *Etat = AJOUTER_DEPUTE;
        }
      }
      if (*Etat == 1) {
        if (*CrenauON == false && *mouseX >= ObjetCreneau[2].x &&
            *mouseX <= ObjetCreneau[2].x + ObjetCreneau[2].w &&
            *mouseY >= ObjetCreneau[2].y &&
            *mouseY <= ObjetCreneau[2].y + ObjetCreneau[2].h) {
          *CrenauON = true;
          *afficherValid = true;
          *speedx = 0;
          *speedy = 0;
          *tournerRad = 0; //-1.5707963267948966
          *VolantPos = 1;
          *freinAmainPos = 0;
          *LevierPos = 2;
          *ValidationPos = 0;
        }
      }
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      *BoutonGauchePresser = false;
    }
  }
}

void GestionBoutonBougerr(SDL_Event event, int mouseX, int mouseY,
                          SDL_Rect BoutonBouger[10], int *freinAmainPos,
                          int *LevierPos, int *VolantPos) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      if (mouseX >= BoutonBouger[3].x &&
          mouseX <= BoutonBouger[3].x + BoutonBouger[3].w &&
          mouseY >= BoutonBouger[3].y &&
          mouseY <= BoutonBouger[3].y + BoutonBouger[3].h) {
        if (*freinAmainPos == 0) {
          *freinAmainPos = 1;
        } else {
          *freinAmainPos = 0;
        }
      }
      // contrainte *VoitureSortiGauche==false et *VoitureSortiDroite utilisable
      // LevierPos
      if (mouseX >= BoutonBouger[2].x &&
          mouseX <= BoutonBouger[2].x + BoutonBouger[2].w &&
          mouseY >= BoutonBouger[2].y &&
          mouseY <= BoutonBouger[2].y + BoutonBouger[2].h * 0.33) {
        *LevierPos = 0;
      }
      if (mouseX >= BoutonBouger[2].x &&
          mouseX <= BoutonBouger[2].x + BoutonBouger[2].w &&
          mouseY >= BoutonBouger[2].y + BoutonBouger[2].h * 0.33 &&
          mouseY <= BoutonBouger[2].y + BoutonBouger[2].h * 0.66) {
        *LevierPos = 1;
      }
      if (mouseX >= BoutonBouger[2].x &&
          mouseX <= BoutonBouger[2].x + BoutonBouger[2].w &&
          mouseY >= BoutonBouger[2].y + BoutonBouger[2].h * 0.66 &&
          mouseY <= BoutonBouger[2].y + BoutonBouger[2].h) {
        *LevierPos = 2;
      }
      // VolantPos
      if (mouseX >= BoutonBouger[0].x &&
          mouseX <= BoutonBouger[0].x + BoutonBouger[0].w * 0.33 &&
          mouseY >= BoutonBouger[0].y &&
          mouseY <= BoutonBouger[0].y + BoutonBouger[0].h) {
        *VolantPos = 0;
      }
      if (mouseX >= BoutonBouger[0].x + BoutonBouger[0].w * 0.33 &&
          mouseX <= BoutonBouger[0].x + BoutonBouger[0].w * 0.66 &&
          mouseY >= BoutonBouger[0].y &&
          mouseY <= BoutonBouger[0].y + BoutonBouger[0].h) {
        *VolantPos = 1;
      }
      if (mouseX >= BoutonBouger[0].x + BoutonBouger[0].w * 0.66 &&
          mouseX <= BoutonBouger[0].x + BoutonBouger[0].w &&
          mouseY >= BoutonBouger[0].y &&
          mouseY <= BoutonBouger[0].y + BoutonBouger[0].h) {
        *VolantPos = 2;
      }
    }
  }
}

void SoulignerMenue(SDL_Event event, int *mouseX, int *mouseY,
                    SDL_Rect BoutonMenue[10], SDL_Renderer *renderer,
                    int NombreBoutonMenue) {
  SDL_GetMouseState(mouseX, mouseY);
  for (int i = 0; i < NombreBoutonMenue; i++) {
    if (*mouseX >= BoutonMenue[i].x &&
        *mouseX <= BoutonMenue[i].x + BoutonMenue[i].w &&
        *mouseY >= BoutonMenue[i].y &&
        *mouseY <= BoutonMenue[i].y + BoutonMenue[i].h) {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderDrawLine(renderer, BoutonMenue[i].x,
                         BoutonMenue[i].y + BoutonMenue[i].h,
                         BoutonMenue[i].x + BoutonMenue[i].w,
                         BoutonMenue[i].y + BoutonMenue[i].h);
    }
  }
}

void Mouvement(double *speedx, double *speedy, bool *BoutonGauchePresser,
               int *mouseX, int *mouseY, SDL_Rect BoutonBouger[10],
               bool *VoitureSortiGauche, bool *VoitureSortiDroite,
               Uint32 delatime, float *tournerRad, int *LevierPos,
               int *VolantPos, int *freinAmainPos, Point VoiturePoint[4],
               Point *Centre, SDL_Rect ObjetCreneau[30], SDL_Surface *image,
               SDL_Renderer *renderer, int window_width, int window_height,
               bool *CreneauON, SDL_Point *center, float *tournerDeg) {
  static double previousTournerRad = 0.0f;
  double NewTournerRad = 0.0f;

  previousTournerRad = *tournerRad;
  if (*BoutonGauchePresser) {
    if (*mouseX >= BoutonBouger[1].x &&
        *mouseX <= BoutonBouger[1].x + BoutonBouger[1].w &&
        *mouseY >= BoutonBouger[1].y &&
        *mouseY <= BoutonBouger[1].y + BoutonBouger[1].h) {
      if (*freinAmainPos == 0) {
        if (*LevierPos == 0) {
          *speedx;
          *speedy;
        }
        if (*LevierPos == 1) {
          *speedx -= 15 * cos(*tournerRad);
          *speedy -= 15 * sin(*tournerRad);
          // Centre->x -= 15 * cos(*tournerRad);
          // Centre->y -= 15 * sin(*tournerRad);
        }
        if (*LevierPos == 2) {
          *speedx += 15 * cos(*tournerRad);
          *speedy += 15 * sin(*tournerRad);
          // Centre->x += 15 * cos(*tournerRad);
          // Centre->y += 15 * sin(*tournerRad);
        }

        if (*VolantPos == 0 && *LevierPos == 2) {
          *tournerRad -= 0.06;
        }
        if (*VolantPos == 0 && *LevierPos == 1) {
          *tournerRad += 0.06;
        }
        if (*VolantPos == 1) {
          *tournerRad;
        }
        if (*VolantPos == 2 && *LevierPos == 2) {
          *tournerRad += 0.06;
        }
        if (*VolantPos == 2 && *LevierPos == 1) {
          *tournerRad -= 0.06;
        }
        creneau(image, renderer, window_width, window_height, ObjetCreneau,
                CreneauON, *speedx, *tournerRad, *speedy, VoiturePoint, Centre,
                center, tournerDeg);
        NewTournerRad = *tournerRad - previousTournerRad;
        if (*LevierPos != 0) {
          for (int i = 0; i < 4; i++) {
            VoiturePoint[i] = transforme(
                VoiturePoint[i], *speedx, *speedy, *tournerRad, Centre->x,
                Centre->y, ObjetCreneau, *LevierPos, *VolantPos, NewTournerRad);
          }
        }
      }
    }
  }
}

void Validation(SDL_Surface *image, SDL_Renderer *renderer, int window_width,
                int window_height, SDL_Rect ObjetCreneau[30], Uint32 delaTime,
                bool *afficherValid, int *freinAmainPos, bool *CreneauON,
                int *LevierPos, int *ValidationPos) {
  SDL_Rect ValidRect = {ObjetCreneau[4].x + ObjetCreneau[4].w * 0.75,
                        ObjetCreneau[4].y + ObjetCreneau[4].h * 0.499, 600,
                        500};

  SDL_Rect RectNew;

  static bool stop1 = false;
  static bool stop2 = false;
  static bool stop3 = false;
  static bool stop4 = false;
  static bool stop5 = false;

  static bool waitboolValid1 = false;
  static bool waitboolValid2 = false;
  static bool waitboolValid3 = false;
  static bool waitboolValid4 = false;
  static bool waitboolValid5 = false;

  static int WaitCompte1 = 0;
  static int WaitCompte2 = 0;
  static int WaitCompte3 = 0;
  static int WaitCompte4 = 0;
  static int WaitCompte5 = 0;

  static bool afficher = true;

  if (*CreneauON) {
    if (wait(20, &waitboolValid1, &WaitCompte1, delaTime, stop1)) {
      stop1 = true;
      afficher = false;
    }
    if (wait(40, &waitboolValid2, &WaitCompte2, delaTime, stop2)) {
      stop2 = true;
      afficher = true;
    }
    if (wait(60, &waitboolValid3, &WaitCompte3, delaTime, stop3)) {
      stop3 = true;
      afficher = false;
    }
    if (wait(80, &waitboolValid4, &WaitCompte4, delaTime, stop4)) {
      stop4 = true;
      afficher = true;
    }
    if (wait(100, &waitboolValid5, &WaitCompte5, delaTime, stop5)) {
      stop5 = true;
      afficher = false;
    }
  }

  if (*afficherValid) {
    if (afficher) {
      if (*ValidationPos == 0) {
        image = IMG_Load("PhotoProg/Validation/grilleValidation.png");
        SDL_Texture *textureValidation1 =
            SDL_CreateTextureFromSurface(renderer, image);

        SDL_SetTextureScaleMode(textureValidation1, SDL_ScaleModeLinear);

        AjusterImageVertical(&ValidRect, image, window_width, window_height,
                             0.063);
        SDL_FreeSurface(image);
        addLayeredImage(textureValidation1, ValidRect, NULL, 0, 1);
        RectNew = ValidRect;
      }
      if (*ValidationPos == 1) {
        image = IMG_Load("PhotoProg/Validation/grilleValidationReussite.png");
        SDL_Texture *textureValidation2 =
            SDL_CreateTextureFromSurface(renderer, image);

        SDL_SetTextureScaleMode(textureValidation2, SDL_ScaleModeLinear);

        AjusterImageVertical(&ValidRect, image, window_width, window_height,
                             0.063);

        SDL_FreeSurface(image);
        addLayeredImage(textureValidation2, ValidRect, NULL, 0, 1);
        RectNew = ValidRect;
      }
      if (*ValidationPos == 2) {
        image = IMG_Load("PhotoProg/Validation/grilleValidationEchec.png");
        SDL_Texture *textureValidation3 =
            SDL_CreateTextureFromSurface(renderer, image);

        SDL_SetTextureScaleMode(textureValidation3, SDL_ScaleModeLinear);

        AjusterImageVertical(&ValidRect, image, window_width, window_height,
                             0.063);

        SDL_FreeSurface(image);
        addLayeredImage(textureValidation3, ValidRect, NULL, 0, 1);
        RectNew = ValidRect;
      }
    }
  }

  if (*freinAmainPos == 1 && *LevierPos == 0) {
    printf("dedans");
    stop1 = false;
    stop2 = false;
    stop3 = false;
    stop4 = false;
    stop5 = false;

    waitboolValid1 = false;
    waitboolValid2 = false;
    waitboolValid3 = false;
    waitboolValid4 = false;
    waitboolValid5 = false;

    WaitCompte1 = 0;
    WaitCompte2 = 0;
    WaitCompte3 = 0;
    WaitCompte4 = 0;
    WaitCompte5 = 0;

    afficher = true;
    *CreneauON = false;

    if (ObjetCreneau[3].x >= ValidRect.x &&
        ObjetCreneau[3].x + ObjetCreneau[3].h <= ValidRect.x + ValidRect.w &&
        ObjetCreneau[3].y >= ValidRect.y &&
        ObjetCreneau[3].y + ObjetCreneau[3].w <= ValidRect.y + ValidRect.h) {
      *ValidationPos = 1;
    } else {
      *ValidationPos = 2;
    }
  }
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  SDL_Window *window = SDL_CreateWindow(
      "Ma fenêtre SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000,
      900, SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Surface *image = IMG_Load("PhotoProg/ArrierePlan/fondBleu.png");
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_FreeSurface(image);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_Event event;
  EtatLogiciel Etat = MENU_PRINCIPAL;

  TTF_Font *fontGrand = TTF_OpenFont("police/RockoFLF.ttf", 50);
  TTF_Font *fontMoyen = TTF_OpenFont("police/RockoFLF.ttf", 35);

  SDL_Rect BoutonMenue[10];
  SDL_Rect ObjetCreneau[30];
  SDL_Rect BoutonBouger[10];
  SDL_Rect Trieur;

  Point VoiturePoint[4];
  Point Centre;
  Point haut;

  SDL_Point center;

  double speedx = 0, speedy = 0;
  float tournerRad = 0; //-1.5707963267948966
  float tournerDeg = 0; //-1.5707963267948966
  float VolantRad = 0;

  int NombreBoutonMenue = 5;
  int window_width, window_height;
  int mouseY, mouseX;
  int LevierPos = 2;
  int VolantPos = 1;
  int freinAmainPos = 0;
  int waitComptePlay = 0;
  int ValidationPos = 0;

  Uint32 lastTime = SDL_GetTicks();
  Uint32 delaTime;

  bool EnCour = true;
  bool VoitureSortiGauche = false;
  bool VoitureSortiDroite = true;
  bool BoutonGauchePresser = false;
  bool CreneauON = false;
  bool waitboolPlay = false;
  bool afficherValid = false;

  while (EnCour) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_GetWindowSize(window, &window_width, &window_height);

    Uint32 currentTime = SDL_GetTicks();
    delaTime = (currentTime - lastTime) / 10;
    lastTime = currentTime;

    while (SDL_PollEvent(&event)) {
      GestionCommandes(event, &EnCour, &mouseX, &mouseY, BoutonMenue,
                       ObjetCreneau, &Etat, BoutonBouger, &CreneauON, &speedx,
                       &speedy, &tournerRad, &LevierPos, &freinAmainPos,
                       &VolantPos, &BoutonGauchePresser, &afficherValid,
                       &ValidationPos);
      if (CreneauON == true) {
        GestionBoutonBougerr(event, mouseX, mouseY, BoutonBouger,
                             &freinAmainPos, &LevierPos, &VolantPos);
      }
    }

    switch (Etat) {
    case MENU_PRINCIPAL:
      EcrireMenu(window, renderer, fontGrand, 0.3, "Manoeuvre", BoutonMenue, 0);
      EcrireMenu(window, renderer, fontGrand, 0.4, "Recherche un depute",
                 BoutonMenue, 1);
      EcrireMenu(window, renderer, fontGrand, 0.5, "Ajouter un depute",
                 BoutonMenue, 4);
      EcrireMenu(window, renderer, fontGrand, 0.6, "Suprimmer un depute",
                 BoutonMenue, 3);
      EcrireMenu(window, renderer, fontGrand, 0.7, "Quitter", BoutonMenue, 2);
      SoulignerMenue(event, &mouseX, &mouseY, BoutonMenue, renderer,
                     NombreBoutonMenue);
      break;
    case MANOEUVRE:
      EcrireMenu(window, renderer, fontGrand, 0.01, "Manoeuvre", ObjetCreneau,
                 0);
      Ecrire(window, renderer, fontMoyen, 0.10, 0.28,
             "Exercice crenaux:", ObjetCreneau, 1, image);
      creneau(image, renderer, window_width, window_height, ObjetCreneau,
              &CreneauON, speedx, tournerRad, speedy, VoiturePoint, &Centre,
              &center, &tournerDeg);
      Validation(image, renderer, window_width, window_height, ObjetCreneau,
                 delaTime, &afficherValid, &freinAmainPos, &CreneauON,
                 &LevierPos, &ValidationPos);
      if (CreneauON == true) {
        if (wait(13, &waitboolPlay, &waitComptePlay, delaTime, false)) {
          DetectionCollision(BoutonBouger, ObjetCreneau, &VoitureSortiGauche,
                             &VoitureSortiDroite);
          BoutonBougerr(image, renderer, BoutonBouger, ObjetCreneau,
                        window_width, window_height, LevierPos, &VolantRad,
                        VolantPos, freinAmainPos);
          Mouvement(&speedx, &speedy, &BoutonGauchePresser, &mouseX, &mouseY,
                    BoutonBouger, &VoitureSortiGauche, &VoitureSortiDroite,
                    delaTime, &tournerRad, &LevierPos, &VolantPos,
                    &freinAmainPos, VoiturePoint, &Centre, ObjetCreneau, image,
                    renderer, window_width, window_height, &CreneauON, &center,
                    &tournerDeg);
        }
      }
      break;
    case RECHERCHE_DEPUTE:

      break;
    case AJOUTER_DEPUTE:

      break;
    }
    afficherImage(renderer);
    for (int i = 0; i < 4; i++) {
      SDL_Rect rect = {VoiturePoint[i].x, VoiturePoint[i].y, 5, 5};
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_RenderFillRect(renderer, &rect);
    }
    SDL_Rect rect = {Centre.x, Centre.y, 5, 5};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);

    SDL_Rect rect2 = {haut.x, haut.y, 5, 5};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect2);
    SDL_RenderPresent(renderer);
    cleanUp();
    SDL_Delay(10);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return 0;
}