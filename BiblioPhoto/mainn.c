#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_ttf.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>

typedef enum {
    MENU_PRINCIPAL,
    CRENEAU_BATAILLE,
    DEMITOUR_EPI,
    ROND_POINT,
    INTERSECTION
} EtatLogiciel;

typedef struct{
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Point center;
    int rotationDeg;
    int layer;
}LayeredImage;

#define MAX_IMAGELAYER 100
LayeredImage LayeredImageTab[MAX_IMAGELAYER];
LayeredImage LayeredImageTab2[MAX_IMAGELAYER];
int ImageNb=0;
int ImageNb2=0;

typedef struct{
    float x;
    float y;
}Point;

typedef struct{
    float w;
    float h;
}DimImage;

typedef struct {
    Point start;
    Point end;
} Segment;

//OUTIL
void scaleVoiture(float* scaleX,float* scaleY,SDL_Rect Parkingg[30],int nbParkingg){  //UTILISER DANS: transform et voiturePricipal
    float reference_width;
    float reference_height;
    if(nbParkingg==0 || nbParkingg==1 || nbParkingg==2 || nbParkingg==5){
        reference_width = 1920 * 0.25;
        reference_height = reference_width * 700.0/375.0;
    }
    if(nbParkingg==6){
        reference_width = 1920 * 0.64;
        reference_height = reference_width * 1;
    }

    int current_width = Parkingg[nbParkingg].w;
    int current_height = Parkingg[nbParkingg].h;

    *scaleX = (float)current_width / reference_width;
    *scaleY = (float)current_height / reference_height;
}
bool wait(int attente,bool* waitbool,int* waitCompte,Uint32 Delatime,bool stop){
    if(stop){*waitbool = false;return *waitbool;}

    if(*waitCompte>=attente){
        *waitbool = true;
    }
    else{
        (*waitCompte)+= 1*Delatime;
    }
    return *waitbool;
}
float CrossProduct(Point p1,Point p2,Point p3){
    return (p2.x-p1.x)*(p3.y-p1.y) - (p2.y-p1.y)*(p3.x-p1.x);
}
bool IsPointInRect(Point pt, Point square[4]) {
    // Calculer les produits vectoriels
    float d1 = CrossProduct(square[0], square[1], pt);
    float d2 = CrossProduct(square[1], square[3], pt);
    float d3 = CrossProduct(square[3], square[2], pt);
    float d4 = CrossProduct(square[2], square[0], pt);

    // Vérifier si tous les produits vectoriels ont le même signe
    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0) || (d4 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0) || (d4 > 0);

    return !(has_neg && has_pos); // Retourne vrai si tous les signes sont cohérents
}
int random_number() {
    return (rand() % 8) + 1;
}

//Texture
void TextureCommande(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureCommande[30]){
    image = IMG_Load("PhotoProg/Commande/volant.png");
    textureCommande[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image=IMG_Load("PhotoProg/Commande/Accelerateur.png");
    textureCommande[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image=IMG_Load("PhotoProg/Commande/levier0.png");
    textureCommande[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image=IMG_Load("PhotoProg/Commande/levier1.png");
    textureCommande[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image=IMG_Load("PhotoProg/Commande/levier2.png");
    textureCommande[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[4], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image=IMG_Load("PhotoProg/Commande/FreinAmainPos0.png");
    textureCommande[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[5], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image=IMG_Load("PhotoProg/Commande/FreinAmainPos1.png");
    textureCommande[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[6], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image=IMG_Load("PhotoProg/Commande/tutorielBoutonn.png");
    textureCommande[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCommande[7], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureCreBa(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureCreBa[30],DimImage tabDimImage[30]){
    image = IMG_Load("PhotoProg/Decor/FondGris.png");
    textureCreBa[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/bandeauExerciceee.png");
    textureCreBa[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/BuissonRanger.png");
    textureCreBa[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/usine.png");
    textureCreBa[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/BarriereGrandee.png");  
    textureCreBa[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/BarrierePetite.png");
    textureCreBa[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image); 

    image = IMG_Load("PhotoProg/Decor/CarreFleur1.png");
    textureCreBa[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/CarreFleur2.png");
    textureCreBa[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/potFleur1.png");
    textureCreBa[8] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/potFleur2.png");
    textureCreBa[9] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);   

    image = IMG_Load("PhotoProg/Decor/TablePiqueNique.png");
    textureCreBa[10] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image); 

    image = IMG_Load("PhotoProg/Decor/passagePieton.png");
    textureCreBa[11] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/banc.png");
    textureCreBa[12] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/tablePiqueNique2.png");
    textureCreBa[13] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/barbecue.png");
    textureCreBa[14] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/buisson.png");
    textureCreBa[15] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCreBa[15], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/HommeChien.png");
    textureCreBa[16] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCreBa[16], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/BarrierePetite22.png");
    textureCreBa[17] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCreBa[17], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Decor/maison.png");
    textureCreBa[18] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureCreBa[18], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureVoiture(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureVoiture[30]){
    image = IMG_Load("PhotoProg/Voiture/voitureBlancheVertical.png");
    textureVoiture[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Voiture/voitureBleuVertical.png");
    textureVoiture[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Voiture/voitureBlancheHor.png");
    textureVoiture[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Voiture/camionBlancHor.png");
    textureVoiture[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Voiture/voitureGriseHor.png");
    textureVoiture[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[4], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Voiture/voitureBleuHor.png");
    textureVoiture[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[5], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
    
    image = IMG_Load("PhotoProg/Voiture/VoitPrincipalHorizontal.png");
    textureVoiture[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[6], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Voiture/camionBlancVertical.png");
    textureVoiture[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[7], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Voiture/VoitBleuDroite.png");
    textureVoiture[8] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureVoiture[8], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureMenue(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureMenue[30]){
    image = IMG_Load("PhotoProg/ElementMenue/MenueFond.png");
    textureMenue[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureMenue[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/ElementMenue/logoMenue.png");
    textureMenue[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureMenue[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/ElementMenue/ByAlixCaperan.png");
    textureMenue[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureMenue[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/ElementMenue/ArcDeCercleMenue.png");
    textureMenue[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureMenue[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureValidation(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureValidation[30]){
    image = IMG_Load("PhotoProg/Validation/grilleValidation.png");
    textureValidation[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/grilleValidationReussite.png");
    textureValidation[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/grilleValidationEchec.png");
    textureValidation[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/grilleValidation2.png");
    textureValidation[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/grilleValidationReussite2.png");
    textureValidation[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[4], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/grilleValidationEchec2.png");
    textureValidation[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[5], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/ValidationEpi.png");
    textureValidation[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[6], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/ReussiteEpi.png");
    textureValidation[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[7], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/EchecEpi.png");
    textureValidation[8] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[8], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/IntersectionHautValidation.png");
    textureValidation[9] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[9], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/IntersectionHautReussite.png");
    textureValidation[10] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[10], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Validation/IntersectionHautEchec.png");
    textureValidation[11] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureValidation[11], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureBoutonDiver(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureBoutonDiver[30]){ 
    image = IMG_Load("PhotoProg/BoutonDiverse/QuitterBouton.png");
    textureBoutonDiver[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureBoutonDiver[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/BoutonDiverse/boutonPlay.png");
    textureBoutonDiver[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureBoutonDiver[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureParking(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureParking[30]){
    image = IMG_Load("PhotoProg/Parking/parkingCreneau.png");
    textureParking[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/parkingBataille.png");
    textureParking[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/ParkingDemiTour5.png");
    textureParking[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/trottoirDroite2.png");
    textureParking[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/trottoirGauche2.png");
    textureParking[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[4], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/parkingEpi2.png");
    textureParking[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[5], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/ParkingIntersectionnn.png");
    textureParking[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[6], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/RectBasDroit.png");
    textureParking[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[7], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/RectBasGauche.png");
    textureParking[8] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[8], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/RectHautDroit.png");
    textureParking[9] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[9], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Parking/RectHautGauche.png");
    textureParking[10] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureParking[10], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureGens(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureGens[30]){
    image = IMG_Load("PhotoProg/Gens/PersHautMarche1.png");
    textureGens[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersHautMarche2.png");
    textureGens[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersHautStatic.png");
    textureGens[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersBasMarche1.png");
    textureGens[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersBasMarche2.png");
    textureGens[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[4], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersBasStatic.png");
    textureGens[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[5], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersDroiteMarche1.png");
    textureGens[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[6], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersDroiteMarche2.png");
    textureGens[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[7], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersDroiteStatic.png");
    textureGens[8] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[8], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersGaucheMarche1.png");
    textureGens[9] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[9], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersGaucheMarche2.png");
    textureGens[10] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[10], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Gens/PersGaucheStatic.png");
    textureGens[11] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureGens[11], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureFeu(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureFeu[30]){
    image = IMG_Load("PhotoProg/Feu/FeuPietonRougeBas.png");
    textureFeu[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuPietonVertBas.png");
    textureFeu[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuRougeBas.png");
    textureFeu[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuVertBas.png");
    textureFeu[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuPietonRougeHaut.png");
    textureFeu[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[4], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuPietonVertHaut.png");
    textureFeu[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[5], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuRougeHaut.png");
    textureFeu[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[6], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuVertHaut.png");
    textureFeu[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[7], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuPietonRougeDroite.png");
    textureFeu[8] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[8], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuPietonVertDroite.png");
    textureFeu[9] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[9], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuRougeDroite.png");
    textureFeu[10] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[10], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuVertDroite.png");
    textureFeu[11] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[11], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuPietonRougeGauche.png");
    textureFeu[12] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[12], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuPietonVertGauche.png");
    textureFeu[13] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[13], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuRougeGauche.png");
    textureFeu[14] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[14], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Feu/FeuVertGauche.png");
    textureFeu[15] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureFeu[15], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureAngle(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureAngle[30]){
    image = IMG_Load("PhotoProg/Angle/AngleBasDroit.png");
    textureAngle[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureAngle[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Angle/AngleBasGauche.png");
    textureAngle[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureAngle[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Angle/AngleHautDroit.png");
    textureAngle[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureAngle[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Angle/AngleHautGauche.png");
    textureAngle[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureAngle[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}
void TextureTuto(SDL_Surface* image,SDL_Renderer* renderer,SDL_Texture* textureTuto[30]){
    image = IMG_Load("PhotoProg/Tuto/AllerADroite.png");
    textureTuto[0] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[0], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/AllerAGauche.png");
    textureTuto[1] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[1], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/AllerToutDroit.png");
    textureTuto[2] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[2], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/traitVertical.png");
    textureTuto[3] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[3], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/PasserEnMode.png");
    textureTuto[4] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[4], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/traitHorizontal.png");
    textureTuto[5] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[5], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/ModeAvancer.png");
    textureTuto[6] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[6], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/ModeParking.png");
    textureTuto[7] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[7], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/ModeRecule.png");
    textureTuto[8] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[8], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/MaintenerAccelerer.png");
    textureTuto[9] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[9], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/DesactiverFreinAMain.png");
    textureTuto[10] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[10], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/GarezVousIciCreneau3.png");
    textureTuto[11] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[11], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/TerminerManoeuvre2.png");
    textureTuto[12] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[12], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/GarezVousIciDemiTour2.png");
    textureTuto[13] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[13], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);

    image = IMG_Load("PhotoProg/Tuto/tutoInteraction.png");
    textureTuto[14] = SDL_CreateTextureFromSurface(renderer, image);
    SDL_SetTextureScaleMode(textureTuto[14], SDL_ScaleModeLinear);
    SDL_FreeSurface(image);
}

//LAYER
void addLayeredImage2(SDL_Texture* textureImage, SDL_Rect RectImagee, SDL_Point* centerImage, int rotationImage,int Layer){
    if (ImageNb2 < MAX_IMAGELAYER) {
        LayeredImageTab2[ImageNb2].texture = textureImage;
        LayeredImageTab2[ImageNb2].rect = RectImagee;
        if (centerImage) {
            LayeredImageTab2[ImageNb2].center = *centerImage;
        } else {
            LayeredImageTab2[ImageNb2].center.x = RectImagee.w / 2;
            LayeredImageTab2[ImageNb2].center.y = RectImagee.h / 2;
        }
        LayeredImageTab2[ImageNb2].rotationDeg = rotationImage;
        LayeredImageTab2[ImageNb2].layer = Layer;
        ImageNb2++;
    }
}
void trierImage2(){
    for(int i=0;i<ImageNb2-1; i++){
        for(int j=i+1;j<ImageNb2; j++){
            if(LayeredImageTab2[i].layer > LayeredImageTab2[j].layer){
                LayeredImage temp=LayeredImageTab2[i];
                LayeredImageTab2[i]=LayeredImageTab2[j];
                LayeredImageTab2[j]=temp;
            }
        }
    }
}
void afficherImage2(SDL_Renderer* renderer){
    trierImage2();
    for(int i=0;i<ImageNb2;i++){
        SDL_RenderCopyEx(renderer, LayeredImageTab2[i].texture, NULL, &LayeredImageTab2[i].rect, LayeredImageTab2[i].rotationDeg, &LayeredImageTab2[i].center, SDL_FLIP_NONE);
    }
}
void cleanUp2(){
    for (int i = 0; i < ImageNb2; i++) {
        SDL_DestroyTexture(LayeredImageTab2[i].texture);
        LayeredImageTab2[i].texture = NULL;        
    }
    ImageNb2 = 0;
}

//COLIDER
Point transforme(Point p,double speedx,double speedy,float tournerRad,float centreX,float centreY,SDL_Rect Parkingg[30],int LevierPos,int VolantPos,int window_width,int window_height,int nbParkingg){
    Point PointTransform;
    float tournerRadd=0;

    float scaleX;
    float scaleY;
    scaleVoiture(&scaleX,&scaleY,Parkingg,nbParkingg);

    int reference_width = 1920;
    int reference_height = 1080;

    int current_width = window_width;  // La largeur actuelle de la fenêtre
    int current_height = window_height; // La hauteur actuelle de la fenêtre

    float winX = (float)current_width / reference_width;
    float winY = (float)current_height / reference_height;

    if(VolantPos == 0 && LevierPos == 2){
        tournerRadd = -0.023;
    }
    if(VolantPos == 0 && LevierPos == 1){
        tournerRadd = 0.023;
    }
    if(VolantPos == 1){
        tournerRadd;
    }
    if(VolantPos == 2 && LevierPos == 2){
        tournerRadd = 0.023;     
    }
    if(VolantPos == 2 && LevierPos == 1){
        tournerRadd = -0.023;
    }

    if(LevierPos==1){
        PointTransform.x = ((p.x - centreX) * cos(tournerRadd) - (p.y - centreY) * sin(tournerRadd) + centreX) - (3 * sin(tournerRad))*scaleX;
        PointTransform.y = ((p.x - centreX) * sin(tournerRadd) + (p.y - centreY) * cos(tournerRadd) + centreY) + (3 * cos(tournerRad))*scaleY;
    }
    if(LevierPos==2){
        PointTransform.x = ((p.x - centreX) * cos(tournerRadd) - (p.y - centreY) * sin(tournerRadd) + centreX) + (3 * sin(tournerRad))*scaleX;
        PointTransform.y = ((p.x - centreX) * sin(tournerRadd) + (p.y - centreY) * cos(tournerRadd) + centreY) - (3 * cos(tournerRad))*scaleY;
    }
    return PointTransform;
}
void ColiderVoiture(double* speedx, double* speedy, float* tournerRad,int* LevierPos,int* VolantPos,int* freinAmainPos,Point VoiturePoint[4],Point* Centre,SDL_Rect ObjetCreneau[30],int window_width, int window_height,bool* CreneauON,SDL_Point* center,int* mouseX, int* mouseY, bool* BoutonGauchePresser,SDL_Rect BoutonBouger[10],bool *ColiderInitialisation,SDL_Rect Parkingg[30],int nbParkingg){
    static int redimansionCoter = 10;
    static int redimansionHaut = 10;

    float scaleX;
    float scaleY;
    scaleVoiture(&scaleX,&scaleY,Parkingg,nbParkingg);

    int actual_size_window = window_width * window_height;
    static int reference_size_window = 0;

    if(actual_size_window != reference_size_window){
        *ColiderInitialisation = true;
    }

    if(*freinAmainPos==0){
        if(*ColiderInitialisation){
            reference_size_window = window_width * window_height;

            VoiturePoint[0].x = ObjetCreneau[3].x + redimansionCoter * scaleX;
            VoiturePoint[0].y = ObjetCreneau[3].y + redimansionHaut * scaleY;

            VoiturePoint[1].x = ObjetCreneau[3].x + ObjetCreneau[3].w - redimansionCoter * scaleX;
            VoiturePoint[1].y = ObjetCreneau[3].y + redimansionHaut * scaleY;

            VoiturePoint[2].x = ObjetCreneau[3].x + redimansionCoter * scaleX;
            VoiturePoint[2].y = ObjetCreneau[3].y + ObjetCreneau[3].h - redimansionHaut * scaleY;

            VoiturePoint[3].x = ObjetCreneau[3].x + ObjetCreneau[3].w - redimansionCoter * scaleX;
            VoiturePoint[3].y = ObjetCreneau[3].y + ObjetCreneau[3].h - redimansionHaut * scaleY;

            for(int i=0;i<4;i++){
                float originalX = VoiturePoint[i].x;
                float originalY = VoiturePoint[i].y;
                
                VoiturePoint[i].x = (originalX - Centre->x) * cos(*tournerRad) - (originalY - Centre->y) * sin(*tournerRad) + Centre->x;
                VoiturePoint[i].y = (originalX - Centre->x) * sin(*tournerRad) + (originalY - Centre->y) * cos(*tournerRad) + Centre->y;
            }

            *ColiderInitialisation = false;                        
        }

        if (*BoutonGauchePresser) {
            if (*mouseX >= BoutonBouger[1].x && *mouseX <= BoutonBouger[1].x + BoutonBouger[1].w && *mouseY >= BoutonBouger[1].y && *mouseY <= BoutonBouger[1].y + BoutonBouger[1].h) {
                if(*LevierPos != 0){
                    for(int i=0;i<4;i++){
                        VoiturePoint[i]=transforme(VoiturePoint[i],*speedx,*speedy,*tournerRad,Centre->x,Centre->y,Parkingg,*LevierPos,*VolantPos,window_width,window_height,nbParkingg);
                    }  
                }
            }
        }
    }
}
void DetectionCollision(SDL_Rect BoutonBouger[10],SDL_Rect ObjetCreneau[30],bool* toucheMur,bool* toucheVoiture, Point VoiturePoint[4],bool* CreneauON,SDL_Rect Voiture[15],SDL_Rect Parkingg[30],int nbParkingg){
    if(VoiturePoint[0].x <= Parkingg[nbParkingg].x || VoiturePoint[1].x <= Parkingg[nbParkingg].x || VoiturePoint[2].x <= Parkingg[nbParkingg].x || VoiturePoint[3].x <= Parkingg[nbParkingg].x){
        *toucheMur=true;
    }
    if(VoiturePoint[0].y >= Parkingg[nbParkingg].y + Parkingg[nbParkingg].h || VoiturePoint[1].y >= Parkingg[nbParkingg].y + Parkingg[nbParkingg].h || VoiturePoint[2].y >= Parkingg[nbParkingg].y + Parkingg[nbParkingg].h || VoiturePoint[3].y >= Parkingg[nbParkingg].y + Parkingg[nbParkingg].h){
        *toucheMur=true;
    }
    if(VoiturePoint[0].x >= Parkingg[nbParkingg].x + Parkingg[nbParkingg].w || VoiturePoint[1].x >= Parkingg[nbParkingg].x + Parkingg[nbParkingg].w || VoiturePoint[2].x >= Parkingg[nbParkingg].x + Parkingg[nbParkingg].w || VoiturePoint[3].x >= Parkingg[nbParkingg].x + Parkingg[nbParkingg].w){
        *toucheMur=true;
    }
    if(VoiturePoint[0].y <= Parkingg[nbParkingg].y || VoiturePoint[1].y <= Parkingg[nbParkingg].y || VoiturePoint[2].y <= Parkingg[nbParkingg].y || VoiturePoint[3].y <= Parkingg[nbParkingg].y){
        *toucheMur=true;
    }

    static int redimansionCoter = 10;
    static int redimansionHaut = 10;
    for(int i=0;i<4;i++){
        if(nbParkingg == 0 || nbParkingg == 1){
            for(int t=0;t<7;t++){
                if(VoiturePoint[i].x > Voiture[t].x + redimansionCoter && VoiturePoint[i].x < Voiture[t].x + Voiture[t].w - redimansionCoter && VoiturePoint[i].y > Voiture[t].y + redimansionHaut && VoiturePoint[i].y < Voiture[t].y + Voiture[t].h - redimansionHaut){
                    *toucheVoiture=true;
                }
            }
        }
        if(nbParkingg == 2 || nbParkingg == 5){
            //for(int t=7;t<?;t++){
            //    if(VoiturePoint[i].x > Voiture[t].x + redimansionCoter && VoiturePoint[i].x < Voiture[t].x + Voiture[t].w - redimansionCoter && VoiturePoint[i].y > Voiture[t].y + redimansionHaut && VoiturePoint[i].y < Voiture[t].y + Voiture[t].h - redimansionHaut){
            //        *toucheVoiture=true;
            //   }
            //}            
        }
    }

    if(*toucheMur || *toucheVoiture){
        *CreneauON=false;
    }
}
void DetectionColisionTrottoir(Point VoiturePoint[4],int nbParkingg,bool* toucheMur,bool* boolON,SDL_Rect Parkingg[30]){
    if(nbParkingg == 4){
        if(VoiturePoint[0].x <= Parkingg[nbParkingg].x + Parkingg[nbParkingg].w || VoiturePoint[1].x <= Parkingg[nbParkingg].x+ Parkingg[nbParkingg].w || VoiturePoint[2].x <= Parkingg[nbParkingg].x+ Parkingg[nbParkingg].w || VoiturePoint[3].x <= Parkingg[nbParkingg].x + Parkingg[nbParkingg].w){
            *toucheMur=true;
        }
    }
    if(nbParkingg == 3){
        if(VoiturePoint[0].x >= Parkingg[nbParkingg].x || VoiturePoint[1].x >= Parkingg[nbParkingg].x || VoiturePoint[2].x >= Parkingg[nbParkingg].x || VoiturePoint[3].x >= Parkingg[nbParkingg].x){
            *toucheMur=true;
        }
    }
    if(*toucheMur){
        *boolON=false; //CreneauON, BatailleON etc...
    }
}
void DetectionColisionRebord(Point VoiturePoint[4],bool* toucheMur,bool* boolON,SDL_Rect Parkingg[30]){
    for(int i=0;i<4;i++){
        if(VoiturePoint[i].x >= Parkingg[7].x && VoiturePoint[i].y >= Parkingg[7].y){
            *toucheMur=true;
        }
        if(VoiturePoint[i].x <= Parkingg[8].x + Parkingg[8].w && VoiturePoint[i].y >= Parkingg[8].y){
            *toucheMur=true;
        }
        if(VoiturePoint[i].x >= Parkingg[9].x && VoiturePoint[i].y <= Parkingg[9].y + Parkingg[9].h){
            *toucheMur=true;
        }
        if(VoiturePoint[i].x <= Parkingg[10].x + Parkingg[10].w && VoiturePoint[i].y <= Parkingg[10].y + Parkingg[10].h){ 
            *toucheMur=true;
        }
    }
    if(*toucheMur){
         *boolON=false;
    }
}
void AfficherColiderVoiture(SDL_Surface* image,SDL_Renderer* renderer,Point VoiturePoint[4],Point Centre){
    for(int i=0;i<4;i++){
        SDL_Rect rect = {VoiturePoint[i].x, VoiturePoint[i].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_Rect rect = {Centre.x, Centre.y, 5, 5};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}

//COLIDER VOITURE SECONDAIRE EPI
void VoitureColiderSecondaire(SDL_Rect Voiture[15],int window_width,int window_height,Point VoitureSec[12]){
    static Point CentreSec[3];
    int redimansionCoter = 10;
    int redimansionHaut = 10;

    static bool ContourVoitureSec = true;

    int actual_size_window = window_width * window_height;
    static int reference_size_window = 0;

    if(actual_size_window != reference_size_window){
        ContourVoitureSec = true;
    }

    int t;
    int maxt;
    int mint;

    if(ContourVoitureSec){
        reference_size_window = window_width * window_height;
        for(int i = 6;i<9;i++){
            if(i==6){t=0;maxt=4;mint=0;}
            if(i==7){t=4;maxt=8;mint=4;}
            if(i==8){t=8;maxt=12;mint=8;}

            for(t;t<maxt;t++){

                if(t==0 || t==4 || t==8){
                    VoitureSec[t].x = Voiture[i].x + redimansionCoter;
                    VoitureSec[t].y = Voiture[i].y + redimansionHaut;
                }
                if(t==1 || t==5 || t==9){
                    VoitureSec[t].x = Voiture[i].x + Voiture[i].w - redimansionCoter;
                    VoitureSec[t].y = Voiture[i].y + redimansionHaut;
                }
                if(t==2 || t==6 || t==10){
                    VoitureSec[t].x = Voiture[i].x + redimansionCoter;
                    VoitureSec[t].y = Voiture[i].y + Voiture[i].h - redimansionHaut;
                }
                if(t==3 || t==7 || t==11){
                    VoitureSec[t].x = Voiture[i].x + Voiture[i].w - redimansionCoter;
                    VoitureSec[t].y = Voiture[i].y + Voiture[i].h - redimansionHaut;
                }

                float originalX = VoitureSec[t].x;
                float originalY = VoitureSec[t].y;
                float angle = 40 * M_PI / 180.0;

                CentreSec[i].x =  Voiture[i].x + Voiture[i].w / 2;
                CentreSec[i].y =  Voiture[i].y + Voiture[i].h / 2;

                VoitureSec[t].x = (originalX - CentreSec[i].x) * cos(angle) - (originalY - CentreSec[i].y) * sin(angle) + CentreSec[i].x;
                VoitureSec[t].y = (originalX - CentreSec[i].x) * sin(angle) + (originalY - CentreSec[i].y) * cos(angle) + CentreSec[i].y;
            }
            ContourVoitureSec = false;
        }                     
    }
}
void AfficherColiderSec(Point VoitureSec[12],SDL_Renderer* renderer){
    
    for(int i=0;i<4;i++){
        SDL_Rect rect = {VoitureSec[i].x, VoitureSec[i].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    for(int i=4;i<8;i++){
        SDL_Rect rect = {VoitureSec[i].x, VoitureSec[i].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    for(int i=8;i<12;i++){
        SDL_Rect rect = {VoitureSec[i].x, VoitureSec[i].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    /*
        SDL_Rect rect = {VoitureSec[0].x, VoitureSec[0].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); //BLEU
        SDL_RenderFillRect(renderer, &rect);

        SDL_Rect rect1 = {VoitureSec[1].x, VoitureSec[1].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); //VERT
        SDL_RenderFillRect(renderer, &rect1);

        SDL_Rect rect2 = {VoitureSec[2].x, VoitureSec[2].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //ROUGE
        SDL_RenderFillRect(renderer, &rect2);

        SDL_Rect rect3 = {VoitureSec[3].x, VoitureSec[3].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); //JAUNE
        SDL_RenderFillRect(renderer, &rect3);
    */
}
void DetectionCollisionEpiVoiture(Point VoiturePoint[4],Point VoitureSec[12],bool* boolON,bool* toucheVoiture){
    Point Square1[4];
    Point Square2[4];
    Point Square3[4];
    for(int i=0;i<4;i++){
        Square1[i] = VoitureSec[i];
    }
    for(int i=4;i<8;i++){
        Square2[i-4] = VoitureSec[i];
    }
    for(int i=8;i<12;i++){
        Square3[i-8] = VoitureSec[i];
    }
    for(int i=0;i<4;i++){
        if(IsPointInRect(VoiturePoint[i],Square1)){*toucheVoiture = true;break;}
        if(IsPointInRect(VoiturePoint[i],Square2)){*toucheVoiture = true;break;}
        if(IsPointInRect(VoiturePoint[i],Square3)){*toucheVoiture = true;break;}
    }
    if(*toucheVoiture){
        *boolON = false;
    }
}

//RESPONSIVE
void AjusterImageVertical(SDL_Rect* Rect,SDL_Surface* image,int window_width, int window_height,float dimension){
    float ratio = (float)image->h / (float)image->w;
    int new_width = window_width * dimension;
    int new_height = new_width * ratio;

    Rect->w=new_width;
    Rect->h=new_height;
}
void AjusterRectangleVertical(SDL_Rect* Rect, int window_width, int window_height,float dimension){
    float ratio = (float)Rect->h / (float)Rect->w;
    int new_width = window_width * dimension;
    int new_height = new_width * ratio;
    
    Rect->w=new_width;
    Rect->h=new_height;
}
void AjusterImageVertical2(SDL_Rect* Rect, SDL_Texture* texture, int window_width, int window_height, float dimension) {
    int image_width, image_height;
    
    SDL_QueryTexture(texture, NULL, NULL, &image_width, &image_height);
    
    float ratio = (float)image_height / (float)image_width;
    int new_width = window_width * dimension;
    int new_height = new_width * ratio;
    
    Rect->w = new_width;
    Rect->h = new_height;
}

//ECRITURE
void EcrireMenu(SDL_Window* window,SDL_Renderer* renderer,TTF_Font* font, float x, float y,const char* texte,SDL_Rect BoutonMenue[10],int place){
    char EcritTrieur[] = "Trieur";
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, texte, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    textRect.x = window_width*x-textSurface->w;
    textRect.y = window_height*y;

    BoutonMenue[place]=textRect;

    SDL_FreeSurface(textSurface);
    addLayeredImage2(texture,textRect,NULL,0,2);
}
void Ecrire(int window_width, int window_height,SDL_Renderer* renderer,TTF_Font* font, float y,float x,const char* texte,float dimension){
    static bool first = true;
    static SDL_Surface* textSurface = NULL;
    static SDL_Texture* texture = NULL;
    static char EcritTrieur[50];
    SDL_Color textColor = {255, 255, 255, 255};

    if(first || strcmp(EcritTrieur, texte) != 0){
    strcpy(EcritTrieur, texte);
    textSurface = TTF_RenderText_Blended(font, texte, textColor);
    texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    SDL_FreeSurface(textSurface);
    first = false;
    }

    SDL_Rect textRect;
    textRect.x = window_width*x;
    textRect.y = window_height*y;
    textRect.w = 1000;
    textRect.h = 1000;

    AjusterImageVertical2(&textRect,texture,window_width,window_height,dimension);
    addLayeredImage2(texture,textRect,NULL,0,3);
}
void SoulignerMenue(SDL_Surface* image,SDL_Event event,int* mouseX, int* mouseY,SDL_Rect BoutonMenue[10],SDL_Renderer* renderer,int NombreBoutonMenue){
    SDL_GetMouseState(mouseX,mouseY);
    for(int i=0;i<NombreBoutonMenue;i++){
        if(*mouseX >= BoutonMenue[i].x && *mouseX <= BoutonMenue[i].x+BoutonMenue[i].w && *mouseY >= BoutonMenue[i].y && *mouseY <= BoutonMenue[i].y + BoutonMenue[i].h){
            //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            //SDL_RenderDrawLine(renderer, BoutonMenue[i].x, BoutonMenue[i].y + BoutonMenue[i].h, BoutonMenue[i].x + BoutonMenue[i].w, BoutonMenue[i].y + BoutonMenue[i].h);
            
            image = IMG_Load("PhotoProg/traitsouligner.png");
            SDL_Texture* textureSouligner = SDL_CreateTextureFromSurface(renderer, image);

            SDL_Rect SoulignerRect={BoutonMenue[i].x,
                                    BoutonMenue[i].y + BoutonMenue[i].h,
                                    BoutonMenue[i].w,
                                    BoutonMenue[i].h*0.25};
            SDL_FreeSurface(image);
            addLayeredImage2(textureSouligner,SoulignerRect,0,0,1);
        }
    }
}

//VOITURE PRINCIPAL
void VoiturePrincipale(SDL_Texture* textureVoiture[15], SDL_Surface* image,SDL_Renderer* renderer,int window_width, int window_height, SDL_Rect ObjetCreneau[30],double speedx,double speedy,float tournerRad,float* tournerDeg,Point* Centre,SDL_Point* center,Point VoiturePoint[4],SDL_Rect Parkingg[30],int nbParkingg,float coordonneX,float coordonneY){
    float scaleX;
    float scaleY;
    scaleVoiture(&scaleX,&scaleY,Parkingg,nbParkingg);

    SDL_Rect VoitureRect={Parkingg[nbParkingg].x+(float)(Parkingg[nbParkingg].w*coordonneX) + speedx * scaleX,
                          Parkingg[nbParkingg].y+(float)(Parkingg[nbParkingg].h*coordonneY) + speedy * scaleY};

    AjusterImageVertical2(&VoitureRect,textureVoiture[6],window_width,window_height,0.047);
    ObjetCreneau[3]=VoitureRect;

    *tournerDeg = (tournerRad*(180/M_PI));
    center->x = VoitureRect.w / 2;
    center->y = VoitureRect.h * (9.0/10.0);
    
    Centre->x =  ObjetCreneau[3].x + ObjetCreneau[3].w / 2;
    Centre->y =  ObjetCreneau[3].y + ObjetCreneau[3].h * (9.0/10.0);

    addLayeredImage2(textureVoiture[6],VoitureRect,center,*tournerDeg,2);
}

//BOUTON
void Quitter(int window_width, int window_height,SDL_Texture* textureBoutonDiver[30],SDL_Rect ObjetCreneau[30]){
    SDL_Rect RectQuitter={window_width*0.01,
                          window_height*0.015};
    AjusterImageVertical2(&RectQuitter,textureBoutonDiver[0],window_width,window_height,0.03);
    ObjetCreneau[14]=RectQuitter;
    addLayeredImage2(textureBoutonDiver[0],RectQuitter,NULL,0,2);       
}
void Play(SDL_Texture* textureBoutonDiver[30],SDL_Surface* image,SDL_Renderer* renderer,SDL_Rect ObjetCreneau[30],bool* CreneauON,bool* BatailleON,int window_width, int window_height,SDL_Rect Parkingg[30],int nbParkingg,int nbParkingg2){
    if(*CreneauON == false || *BatailleON == false){ 

        if(*CreneauON == false){
            SDL_Rect RectPlay={Parkingg[nbParkingg].x+Parkingg[nbParkingg].w+Parkingg[nbParkingg].w*0.15,
                               Parkingg[nbParkingg].y+Parkingg[nbParkingg].h*0.4};
            AjusterImageVertical2(&RectPlay,textureBoutonDiver[1],window_width,window_height,0.075);
            ObjetCreneau[2]=RectPlay;
            addLayeredImage2(textureBoutonDiver[1],RectPlay,NULL,0,1);  
        }

        if(*BatailleON == false){
            SDL_Rect RectPlay2={Parkingg[nbParkingg2].x+Parkingg[nbParkingg2].w+Parkingg[nbParkingg2].w*0.15,
                                Parkingg[nbParkingg2].y+Parkingg[nbParkingg2].h*0.4};
            AjusterImageVertical2(&RectPlay2,textureBoutonDiver[1],window_width,window_height,0.075);                       
            ObjetCreneau[13]=RectPlay2;
            addLayeredImage2(textureBoutonDiver[1],RectPlay2,NULL,0,1);
        }
    }
}

//DESIGN CRENEAU
void Parking(SDL_Texture* textureParking[30], SDL_Surface* image,SDL_Renderer* renderer,int window_width, int window_height, SDL_Rect ObjetCreneau[30],bool* CreneauON,double speedx,float tournerRad,double speedy,Point VoiturePoint[4],Point* Centre,SDL_Point* center,float* tournerDeg,SDL_Rect Parkingg[30]){

    SDL_Rect ImageRect={window_width*0.02,window_height*0.10,1,0.6};
    AjusterImageVertical2(&ImageRect,textureParking[0],window_width,window_height,0.25);//ATTENTION,SI MODIFIE DIMENSION, LE FAIRE AUSSI DANS FONCTION SCALEVOITURE
    Parkingg[0]=ImageRect;
    addLayeredImage2(textureParking[0],ImageRect,NULL,0,0);

    SDL_Rect BatailleRect={window_width*0.51,window_height*0.10};
    AjusterImageVertical2(&BatailleRect,textureParking[1],window_width,window_height,0.25);
    Parkingg[1]=BatailleRect;
    addLayeredImage2(textureParking[1],BatailleRect,NULL,0,0);
}
void ChargementVoiture(SDL_Texture* textureVoiture[15],SDL_Surface* image,SDL_Renderer* renderer, SDL_Rect ObjetCreneau[30],int window_width, int window_height,bool* BatailleOn,SDL_Rect Voiture[15],SDL_Rect Parkingg[30]){
    SDL_Rect VoitureRougeRect={Parkingg[0].x+Parkingg[0].w*0.783,
                               Parkingg[0].y+Parkingg[0].h*0.269};                              
    AjusterImageVertical2(&VoitureRougeRect,textureVoiture[0],window_width,window_height,0.051);
    Voiture[0]=VoitureRougeRect;
    addLayeredImage2(textureVoiture[0],VoitureRougeRect,0,0,1);

    SDL_Rect VoitureBleuRect={Parkingg[0].x+Parkingg[0].w*0.783,
                               Parkingg[0].y+Parkingg[0].h*0.028};
    AjusterImageVertical2(&VoitureBleuRect,textureVoiture[1],window_width,window_height,0.051);
    Voiture[1]=VoitureBleuRect;
    addLayeredImage2(textureVoiture[1],VoitureBleuRect,0,0,1);

    SDL_Rect VoitureBlancheBatailleRect={Parkingg[1].x+Parkingg[1].w*0.58,
                                         Parkingg[1].y+Parkingg[1].h*0.0145};
    AjusterImageVertical2(&VoitureBlancheBatailleRect,textureVoiture[2],window_width,window_height,0.099);
    Voiture[2]=VoitureBlancheBatailleRect;
    addLayeredImage2(textureVoiture[2],VoitureBlancheBatailleRect,0,0,1);

    SDL_Rect camionBatRect={Parkingg[1].x+Parkingg[1].w*0.58,
                            Parkingg[1].y+Parkingg[1].h*0.147};
    AjusterImageVertical2(&camionBatRect,textureVoiture[3],window_width,window_height,0.099);
    Voiture[3]=camionBatRect;
    addLayeredImage2(textureVoiture[3],camionBatRect,0,0,1);

    SDL_Rect VoitureGriseBatRect={Parkingg[1].x+Parkingg[1].w*0.58,
                                  Parkingg[1].y+Parkingg[1].h*0.279};
    AjusterImageVertical2(&VoitureGriseBatRect,textureVoiture[4],window_width,window_height,0.099);
    Voiture[4]=VoitureGriseBatRect;
    addLayeredImage2(textureVoiture[4],VoitureGriseBatRect,0,0,1);

    SDL_Rect VoitureBleuHorRect={Parkingg[1].x+Parkingg[1].w*0.58,
                                  Parkingg[1].y+Parkingg[1].h*0.403};
    AjusterImageVertical2(&VoitureBleuHorRect,textureVoiture[5],window_width,window_height,0.096);
    Voiture[5]=VoitureBleuHorRect;
    addLayeredImage2(textureVoiture[5],VoitureBleuHorRect,0,0,1);
}
void DecorBatailleCreneau(SDL_Surface* image,SDL_Renderer* renderer,int window_width,int window_height,SDL_Rect ObjetCreneau[30],SDL_Texture* textureCreBa[30],SDL_Rect Parkingg[30]){    
    
    SDL_Rect rectBuissonRanger={Parkingg[0].x+Parkingg[0].w*1.08,Parkingg[0].y};
    AjusterImageVertical2(&rectBuissonRanger,textureCreBa[2],window_width,window_height,0.2);
    addLayeredImage2(textureCreBa[2],rectBuissonRanger,NULL,0,1);

    SDL_Rect rectUsine={Parkingg[1].x+Parkingg[1].w*1.17,Parkingg[1].y-Parkingg[1].h*0.1};
    AjusterImageVertical2(&rectUsine,textureCreBa[3],window_width,window_height,0.17);
    addLayeredImage2(textureCreBa[3],rectUsine,NULL,0,0);

    SDL_Rect rectBarriereGrande={Parkingg[0].x-Parkingg[0].w*0.037,Parkingg[0].y};
    AjusterImageVertical2(&rectBarriereGrande,textureCreBa[4],window_width,window_height,0.0085);
    addLayeredImage2(textureCreBa[4],rectBarriereGrande,NULL,0,1);

    SDL_Rect rectBarrierePetite={Parkingg[0].x+Parkingg[0].w+Parkingg[0].w*0.005,Parkingg[0].y};
    SDL_Rect rectBarrierePetite2={Parkingg[1].x-Parkingg[0].w*0.037,Parkingg[1].y};
    SDL_Rect rectBarrierePetite3={Parkingg[1].x+Parkingg[1].w+Parkingg[0].w*0.005,Parkingg[1].y};
    AjusterImageVertical2(&rectBarrierePetite,textureCreBa[5],window_width,window_height,0.0085);
    AjusterImageVertical2(&rectBarrierePetite2,textureCreBa[5],window_width,window_height,0.0085);
    AjusterImageVertical2(&rectBarrierePetite3,textureCreBa[5],window_width,window_height,0.0085); 
    addLayeredImage2(textureCreBa[5],rectBarrierePetite,NULL,0,1);
    addLayeredImage2(textureCreBa[5],rectBarrierePetite2,NULL,0,1);
    addLayeredImage2(textureCreBa[5],rectBarrierePetite3,NULL,0,1);

    SDL_Rect rectPassagePieton={Parkingg[1].x+Parkingg[1].w*1.078,Parkingg[1].y+Parkingg[1].h-Parkingg[1].w*0.17};
    SDL_Rect rectPassagePieton2={Parkingg[0].x+Parkingg[0].w*1.078,Parkingg[0].y+Parkingg[0].h-Parkingg[0].w*0.17};
    AjusterImageVertical2(&rectPassagePieton,textureCreBa[11],window_width,window_height,0.2);
    AjusterImageVertical2(&rectPassagePieton2,textureCreBa[11],window_width,window_height,0.2);
    addLayeredImage2(textureCreBa[11],rectPassagePieton,NULL,0,1);
    addLayeredImage2(textureCreBa[11],rectPassagePieton2,NULL,0,1);

    SDL_Rect rectPiqueNique={Parkingg[1].x+Parkingg[1].w*1.28,Parkingg[1].y+Parkingg[1].h*0.67};
    AjusterImageVertical2(&rectPiqueNique,textureCreBa[10],window_width,window_height,0.1);
    addLayeredImage2(textureCreBa[10],rectPiqueNique,NULL,0,1);

    SDL_Rect rectbanc={Parkingg[0].x+Parkingg[0].w*0.98,Parkingg[1].y+Parkingg[1].h*0.75};
    SDL_Rect rectbanc2={Parkingg[1].x-Parkingg[1].w*0.215,Parkingg[1].y+Parkingg[1].h*0.2};
    AjusterImageVertical2(&rectbanc,textureCreBa[12],window_width,window_height,0.06);
    AjusterImageVertical2(&rectbanc2,textureCreBa[12],window_width,window_height,0.06);
    addLayeredImage2(textureCreBa[12],rectbanc,NULL,-90,1);
    addLayeredImage2(textureCreBa[12],rectbanc2,NULL,90,1);

    SDL_Rect rectpotPlante1={Parkingg[1].x-Parkingg[1].w*0.18,Parkingg[1].y+Parkingg[1].h*0.6};
    AjusterImageVertical2(&rectpotPlante1,textureCreBa[9],window_width,window_height,0.03);
    addLayeredImage2(textureCreBa[9],rectpotPlante1,NULL,0,1);

    SDL_Rect rectpotFleur2={Parkingg[0].x+Parkingg[0].w*1.1,Parkingg[1].y+Parkingg[1].h*0.2};
    AjusterImageVertical2(&rectpotFleur2,textureCreBa[8],window_width,window_height,0.03);
    addLayeredImage2(textureCreBa[8],rectpotFleur2,NULL,0,1);

    SDL_Rect rectCarreFleur1={Parkingg[1].x+Parkingg[1].w*1.83,Parkingg[1].y+Parkingg[1].h*0.78};
    AjusterImageVertical2(&rectCarreFleur1,textureCreBa[6],window_width,window_height,0.03);
    addLayeredImage2(textureCreBa[6],rectCarreFleur1,NULL,0,1);

    SDL_Rect rectCarreFleur2={Parkingg[1].x+Parkingg[1].w*1.829,Parkingg[1].y+Parkingg[1].h*0.72};
    AjusterImageVertical2(&rectCarreFleur2,textureCreBa[7],window_width,window_height,0.03);
    addLayeredImage2(textureCreBa[7],rectCarreFleur2,NULL,0,1);
}

//DESIGN EPI
void ParkingEpi(SDL_Texture* textureParking[30],int window_width, int window_height,SDL_Rect Parkingg[30]){
    SDL_Rect ParkingDemiTourRect={window_width*0.02,window_height*0.10};
    AjusterImageVertical2(&ParkingDemiTourRect,textureParking[2],window_width,window_height,0.25);//ATTENTION,SI MODIFIE DIMENSION, LE FAIRE AUSSI DANS FONCTION SCALEVOITURE
    Parkingg[2]=ParkingDemiTourRect;
    addLayeredImage2(textureParking[2],ParkingDemiTourRect,NULL,0,0);

    SDL_Rect TrottoirGaucheRect={ParkingDemiTourRect.x+ParkingDemiTourRect.w-ParkingDemiTourRect.w*0.20,ParkingDemiTourRect.y,ParkingDemiTourRect.w*0.20,ParkingDemiTourRect.h};
    Parkingg[3]=TrottoirGaucheRect;
    addLayeredImage2(textureParking[3],TrottoirGaucheRect,NULL,0,0);

    SDL_Rect TrottoirDroiteRect={ParkingDemiTourRect.x,ParkingDemiTourRect.y,ParkingDemiTourRect.w*0.20,ParkingDemiTourRect.h};
    Parkingg[4]=TrottoirDroiteRect;
    addLayeredImage2(textureParking[4],TrottoirDroiteRect,NULL,0,0);
        
    SDL_Rect EpiRect={window_width*0.51,window_height*0.10,1};
    AjusterImageVertical2(&EpiRect,textureParking[1],window_width,window_height,0.25);
    Parkingg[5]=EpiRect;
    addLayeredImage2(textureParking[5],EpiRect,NULL,0,0);
}
void ChargementVoitureEpi(SDL_Texture* textureVoiture[15],SDL_Surface* image,SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Voiture[15],SDL_Rect Parkingg[30]){
    SDL_Rect VoitureBleuHorRect={Parkingg[5].x+Parkingg[5].w*0.61,
                                 Parkingg[5].y+Parkingg[5].h*0.429};
    AjusterImageVertical2(&VoitureBleuHorRect,textureVoiture[5],window_width,window_height,0.096);
    Voiture[6]=VoitureBleuHorRect;
    addLayeredImage2(textureVoiture[5],VoitureBleuHorRect,0,40,1);

    SDL_Rect VoitureBlancheBatailleRect={Parkingg[5].x+Parkingg[5].w*0.61,
                                         Parkingg[5].y+Parkingg[5].h*0.263};
    AjusterImageVertical2(&VoitureBlancheBatailleRect,textureVoiture[2],window_width,window_height,0.096);
    Voiture[7]=VoitureBlancheBatailleRect;
    addLayeredImage2(textureVoiture[2],VoitureBlancheBatailleRect,0,40,1);

    SDL_Rect VoitureGriseBatRect={Parkingg[5].x+Parkingg[5].w*0.61,
                                  Parkingg[5].y+Parkingg[5].h*0.093};
    AjusterImageVertical2(&VoitureGriseBatRect,textureVoiture[4],window_width,window_height,0.096);
    Voiture[8]=VoitureGriseBatRect;
    addLayeredImage2(textureVoiture[4],VoitureGriseBatRect,0,40,1);
}
void DecorEpiDemiTour(SDL_Surface* image,SDL_Renderer* renderer,int window_width,int window_height,SDL_Texture* textureCreBa[30],SDL_Rect Parkingg[30]){
    SDL_Rect rectBarriereGrande={Parkingg[2].x-Parkingg[2].w*0.037,Parkingg[2].y};
    AjusterImageVertical2(&rectBarriereGrande,textureCreBa[4],window_width,window_height,0.0085);
    addLayeredImage2(textureCreBa[4],rectBarriereGrande,NULL,0,1);

    SDL_Rect rectBarrierePetite={Parkingg[2].x+Parkingg[2].w+Parkingg[2].w*0.005,Parkingg[2].y};
    SDL_Rect rectBarrierePetite2={Parkingg[5].x-Parkingg[2].w*0.037,Parkingg[5].y};
    SDL_Rect rectBarrierePetite3={Parkingg[5].x+Parkingg[5].w+Parkingg[2].w*0.005,Parkingg[5].y};
    AjusterImageVertical2(&rectBarrierePetite,textureCreBa[5],window_width,window_height,0.0085);
    AjusterImageVertical2(&rectBarrierePetite2,textureCreBa[5],window_width,window_height,0.0085);
    AjusterImageVertical2(&rectBarrierePetite3,textureCreBa[5],window_width,window_height,0.0085); 
    addLayeredImage2(textureCreBa[5],rectBarrierePetite,NULL,0,1);
    addLayeredImage2(textureCreBa[5],rectBarrierePetite2,NULL,0,1);
    addLayeredImage2(textureCreBa[5],rectBarrierePetite3,NULL,0,1);

    SDL_Rect rectPassagePieton={Parkingg[5].x+Parkingg[5].w*1.078,Parkingg[5].y+Parkingg[5].h-Parkingg[5].w*0.17};
    SDL_Rect rectPassagePieton2={Parkingg[2].x+Parkingg[2].w*1.078,Parkingg[2].y+Parkingg[2].h-Parkingg[2].w*0.17};
    AjusterImageVertical2(&rectPassagePieton,textureCreBa[11],window_width,window_height,0.2);
    AjusterImageVertical2(&rectPassagePieton2,textureCreBa[11],window_width,window_height,0.2);
    addLayeredImage2(textureCreBa[11],rectPassagePieton,NULL,0,1);
    addLayeredImage2(textureCreBa[11],rectPassagePieton2,NULL,0,1);

    SDL_Rect rectpotFleur2={Parkingg[2].x+Parkingg[2].w*1.08,Parkingg[2].y+Parkingg[5].h*0.8};
    SDL_Rect rectpotFleur22={Parkingg[2].x+Parkingg[2].w*1.53,Parkingg[2].y+Parkingg[5].h*0.8};
    AjusterImageVertical2(&rectpotFleur2,textureCreBa[8],window_width,window_height,0.03);
    AjusterImageVertical2(&rectpotFleur22,textureCreBa[8],window_width,window_height,0.03);
    addLayeredImage2(textureCreBa[8],rectpotFleur2,NULL,0,1);
    addLayeredImage2(textureCreBa[8],rectpotFleur22,NULL,0,1);

    SDL_Rect rectpotPlante1={Parkingg[2].x+Parkingg[2].w*1.3,Parkingg[2].y+Parkingg[5].h*0.8};
    SDL_Rect rectpotPlante11={Parkingg[2].x+Parkingg[2].w*1.76,Parkingg[2].y+Parkingg[5].h*0.8};
    AjusterImageVertical2(&rectpotPlante1,textureCreBa[9],window_width,window_height,0.03);
    AjusterImageVertical2(&rectpotPlante11,textureCreBa[9],window_width,window_height,0.03);
    addLayeredImage2(textureCreBa[9],rectpotPlante1,NULL,0,1);
    addLayeredImage2(textureCreBa[9],rectpotPlante11,NULL,0,1);

    SDL_Rect rectPiqueNique2={Parkingg[5].x+Parkingg[5].w*1.3,Parkingg[5].y+Parkingg[5].h*0.7};
    AjusterImageVertical2(&rectPiqueNique2,textureCreBa[13],window_width,window_height,0.09);
    addLayeredImage2(textureCreBa[13],rectPiqueNique2,NULL,0,1);

    SDL_Rect rectBarbecue={Parkingg[5].x+Parkingg[5].w*1.62,Parkingg[5].y+Parkingg[5].h*0};
    AjusterImageVertical2(&rectBarbecue,textureCreBa[14],window_width,window_height,0.07);
    addLayeredImage2(textureCreBa[14],rectBarbecue,NULL,0,1);

    SDL_Rect rectBuisson={Parkingg[5].x+Parkingg[5].w*1.1,Parkingg[5].y+Parkingg[5].h*0.05};
    SDL_Rect rectBuisson2={Parkingg[5].x+Parkingg[5].w*1.5,Parkingg[5].y+Parkingg[5].h*0.2};
    AjusterImageVertical2(&rectBuisson,textureCreBa[15],window_width,window_height,0.06);
    AjusterImageVertical2(&rectBuisson2,textureCreBa[15],window_width,window_height,0.06);
    addLayeredImage2(textureCreBa[15],rectBuisson,NULL,0,1);
    addLayeredImage2(textureCreBa[15],rectBuisson2,NULL,40,1);

    SDL_Rect rectbanc={Parkingg[2].x+Parkingg[2].w*0.98,Parkingg[5].y+Parkingg[5].h*0.05};
    SDL_Rect rectbanc2={Parkingg[5].x-Parkingg[5].w*0.215,Parkingg[5].y+Parkingg[5].h*0.2};
    AjusterImageVertical2(&rectbanc,textureCreBa[12],window_width,window_height,0.06);
    AjusterImageVertical2(&rectbanc2,textureCreBa[12],window_width,window_height,0.06);
    addLayeredImage2(textureCreBa[12],rectbanc,NULL,-90,1);
    addLayeredImage2(textureCreBa[12],rectbanc2,NULL,90,1);

    SDL_Rect rectCarreFleur={Parkingg[2].x+Parkingg[2].w*1.05,Parkingg[5].y+Parkingg[5].h*0.18};
    AjusterImageVertical2(&rectCarreFleur,textureCreBa[6],window_width,window_height,0.025);
    addLayeredImage2(textureCreBa[6],rectCarreFleur,NULL,0,1);

    SDL_Rect rectCarreFleur2={Parkingg[2].x+Parkingg[2].w*1.048,Parkingg[5].y+Parkingg[5].h*0.23};
    AjusterImageVertical2(&rectCarreFleur2,textureCreBa[7],window_width,window_height,0.025);
    addLayeredImage2(textureCreBa[7],rectCarreFleur2,NULL,0,1);

    SDL_Rect rectHommeChien={Parkingg[2].x+Parkingg[2].w*0.82,Parkingg[2].y+Parkingg[2].h*0.5};
    AjusterImageVertical2(&rectHommeChien,textureCreBa[16],window_width,window_height,0.05);
    addLayeredImage2(textureCreBa[16],rectHommeChien,NULL,180,1);
}

//DESIGN INTERSECTION
void ParkingIntersection(SDL_Texture* textureParking[30],SDL_Renderer* renderer,int window_width,int window_height,SDL_Rect Parkingg[30]){
    SDL_Rect ParkingIntersectionRect={window_width*0.08,window_height*0.095};
    AjusterImageVertical2(&ParkingIntersectionRect,textureParking[6],window_width,window_height,0.64);
    Parkingg[6]=ParkingIntersectionRect;
    addLayeredImage2(textureParking[6],ParkingIntersectionRect,NULL,0,0);

    SDL_Rect CoinHautGaucheRect={Parkingg[6].x,Parkingg[6].y};
    AjusterImageVertical2(&CoinHautGaucheRect,textureParking[10],window_width,window_height,0.23);
    Parkingg[10]=CoinHautGaucheRect;
    addLayeredImage2(textureParking[10],CoinHautGaucheRect,NULL,0,0);

    SDL_Rect CoinBasDroitRect={Parkingg[6].x+Parkingg[6].w-Parkingg[10].w,Parkingg[6].y+Parkingg[6].h-Parkingg[10].h};
    AjusterImageVertical2(&CoinBasDroitRect,textureParking[7],window_width,window_height,0.23);
    Parkingg[7]=CoinBasDroitRect;
    addLayeredImage2(textureParking[7],CoinBasDroitRect,NULL,0,0);

    SDL_Rect CoinBasGauchetRect={Parkingg[6].x,Parkingg[6].y+Parkingg[6].h-Parkingg[10].h};
    AjusterImageVertical2(&CoinBasGauchetRect,textureParking[8],window_width,window_height,0.23);
    Parkingg[8]=CoinBasGauchetRect;
    addLayeredImage2(textureParking[8],CoinBasGauchetRect,NULL,0,0);

    SDL_Rect CoinHautDroitRect={Parkingg[6].x+Parkingg[6].w-Parkingg[10].w,Parkingg[6].y};
    AjusterImageVertical2(&CoinHautDroitRect,textureParking[9],window_width,window_height,0.23);
    Parkingg[9]=CoinHautDroitRect;
    addLayeredImage2(textureParking[9],CoinHautDroitRect,NULL,0,0);

}
void DecorIntersection(SDL_Renderer* renderer,int window_width,int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureCreBa[30]){
    
    SDL_Rect PassagePietonRect={Parkingg[10].x + Parkingg[10].w * 1.022, Parkingg[6].y + Parkingg[6].h * 0.225};
    SDL_Rect PassagePietonRect2={Parkingg[8].x + Parkingg[8].w * 1.022, Parkingg[6].y + Parkingg[6].h * 0.705};
    SDL_Rect PassagePietonRect3={Parkingg[8].x + Parkingg[8].w * 0.54, Parkingg[6].y + Parkingg[6].h * 0.467};
    SDL_Rect PassagePietonRect4={Parkingg[8].x + Parkingg[8].w * 1.515, Parkingg[6].y + Parkingg[6].h * 0.467};
    AjusterImageVertical2(&PassagePietonRect,textureCreBa[11],window_width,window_height,0.17);
    AjusterImageVertical2(&PassagePietonRect2,textureCreBa[11],window_width,window_height,0.17);
    AjusterImageVertical2(&PassagePietonRect3,textureCreBa[11],window_width,window_height,0.17);
    AjusterImageVertical2(&PassagePietonRect4,textureCreBa[11],window_width,window_height,0.17);
    addLayeredImage2(textureCreBa[11],PassagePietonRect,NULL,0,0);
    addLayeredImage2(textureCreBa[11],PassagePietonRect2,NULL,0,0);
    addLayeredImage2(textureCreBa[11],PassagePietonRect3,NULL,90,0);
    addLayeredImage2(textureCreBa[11],PassagePietonRect4,NULL,90,0);

    SDL_Rect BarrierePetite2Rect={Parkingg[6].x-Parkingg[6].w*0.015,Parkingg[6].y-Parkingg[6].h*0};
    SDL_Rect BarrierePetite2Rect2={Parkingg[6].x+Parkingg[6].w*1.004,Parkingg[6].y+Parkingg[6].h*0};
    SDL_Rect BarrierePetite2Rect3={Parkingg[6].x+Parkingg[6].w*1.004,Parkingg[6].y+Parkingg[6].h*0.7};
    SDL_Rect BarrierePetite2Rect4={Parkingg[6].x-Parkingg[6].w*0.015,Parkingg[6].y+Parkingg[6].h*0.7};
    AjusterImageVertical2(&BarrierePetite2Rect,textureCreBa[17],window_width,window_height,0.0073);
    AjusterImageVertical2(&BarrierePetite2Rect2,textureCreBa[17],window_width,window_height,0.0073);
    AjusterImageVertical2(&BarrierePetite2Rect3,textureCreBa[17],window_width,window_height,0.0073);
    AjusterImageVertical2(&BarrierePetite2Rect4,textureCreBa[17],window_width,window_height,0.0073);
    addLayeredImage2(textureCreBa[17],BarrierePetite2Rect,0,0,1);
    addLayeredImage2(textureCreBa[17],BarrierePetite2Rect2,0,0,1);
    addLayeredImage2(textureCreBa[17],BarrierePetite2Rect3,0,0,1);
    addLayeredImage2(textureCreBa[17],BarrierePetite2Rect4,0,0,1);

    SDL_Rect rectbanc={Parkingg[6].x-Parkingg[6].w*0.077,Parkingg[6].y+Parkingg[6].h*0.1};
    SDL_Rect rectbanc2={Parkingg[6].x+Parkingg[6].w*1.35,Parkingg[6].y+Parkingg[6].h*0.15};
    AjusterImageVertical2(&rectbanc,textureCreBa[12],window_width,window_height,0.055);
    AjusterImageVertical2(&rectbanc2,textureCreBa[12],window_width,window_height,0.055);
    addLayeredImage2(textureCreBa[12],rectbanc,NULL,90,1);
    addLayeredImage2(textureCreBa[12],rectbanc2,NULL,90,1);

    SDL_Rect rectCarreFleur1={Parkingg[6].x-Parkingg[6].w*0.06,Parkingg[6].y+Parkingg[6].h*0.82};
    AjusterImageVertical2(&rectCarreFleur1,textureCreBa[6],window_width,window_height,0.027);
    addLayeredImage2(textureCreBa[6],rectCarreFleur1,NULL,0,1);

    SDL_Rect rectCarreFleur2={Parkingg[6].x-Parkingg[6].w*0.06,Parkingg[6].y+Parkingg[6].h*0.76};
    SDL_Rect rectCarreFleur22={Parkingg[6].x-Parkingg[6].w*0.06,Parkingg[6].y+Parkingg[6].h*0.88};
    AjusterImageVertical2(&rectCarreFleur2,textureCreBa[7],window_width,window_height,0.027);
    AjusterImageVertical2(&rectCarreFleur22,textureCreBa[7],window_width,window_height,0.027);
    addLayeredImage2(textureCreBa[7],rectCarreFleur2,NULL,0,1);
    addLayeredImage2(textureCreBa[7],rectCarreFleur22,NULL,0,1);

    SDL_Rect rectBuissonRanger={Parkingg[6].x+Parkingg[6].w*1.06,Parkingg[6].y};
    AjusterImageVertical2(&rectBuissonRanger,textureCreBa[2],window_width,window_height,0.2);
    addLayeredImage2(textureCreBa[2],rectBuissonRanger,NULL,0,1);

    SDL_Rect rectpotFleur2={Parkingg[6].x+Parkingg[6].w*1.02,Parkingg[6].y+Parkingg[6].h*0.2};
    AjusterImageVertical2(&rectpotFleur2,textureCreBa[8],window_width,window_height,0.03);
    addLayeredImage2(textureCreBa[8],rectpotFleur2,NULL,0,1);

    SDL_Rect maisonRect={Parkingg[6].x+Parkingg[6].w*1.1,Parkingg[6].y+Parkingg[6].h*0.75};
    AjusterImageVertical2(&maisonRect,textureCreBa[18],window_width,window_height,0.14);
    addLayeredImage2(textureCreBa[18],maisonRect,NULL,0,1); 
}

//DESIGN COMMUN ET MENU
void FondBase(int window_width,int window_height,SDL_Texture* textureCreBa[30]){
    SDL_Rect rectFondGris={0,0,window_width,window_height};
    addLayeredImage2(textureCreBa[0],rectFondGris,NULL,0,0);

    SDL_Rect rectbandeauExerciceee={0,0};
    AjusterImageVertical2(&rectbandeauExerciceee,textureCreBa[1],window_width,window_height,1);
    addLayeredImage2(textureCreBa[1],rectbandeauExerciceee,NULL,0,1);    
}
void ArrierePlanMenue(SDL_Texture* textureMenue[30],SDL_Surface* image,SDL_Renderer* renderer,int window_width,int window_height){
    SDL_Rect rectArrierePlan={0,0,window_width,window_height};
    addLayeredImage2(textureMenue[0],rectArrierePlan,NULL,0,0);

    SDL_Rect logoRect={window_width*0.005,window_height*0.01,616,174};
    addLayeredImage2(textureMenue[1],logoRect,NULL,0,1);

    SDL_Rect ByAlixCaperanRect={window_width*0.005,window_height*0.97,207,23};
    addLayeredImage2(textureMenue[2],ByAlixCaperanRect,NULL,0,1);

    SDL_Rect ArcRect={window_width-654,0,654,window_height};
    addLayeredImage2(textureMenue[3],ArcRect,NULL,0,1);
}

//TUTORIEL
void tutoriel(SDL_Rect BoutonBouger[10],int window_width, int window_height,SDL_Texture* textureTuto[30]){
    SDL_Rect AllerADroiteRect={BoutonBouger[0].x+BoutonBouger[0].w*0.9,
                               BoutonBouger[0].y-BoutonBouger[0].h*0.4};
    AjusterImageVertical2(&AllerADroiteRect,textureTuto[0],window_width,window_height,0.1);
    addLayeredImage2(textureTuto[0],AllerADroiteRect,0,0,2);

    SDL_Rect AllerToutDroitRect={BoutonBouger[0].x-BoutonBouger[0].w*0.3,
                               BoutonBouger[0].y-BoutonBouger[0].h*0.5};
    AjusterImageVertical2(&AllerToutDroitRect,textureTuto[2],window_width,window_height,0.1);
    addLayeredImage2(textureTuto[2],AllerToutDroitRect,0,0,2);

    SDL_Rect AllerAGaucheRect={BoutonBouger[0].x-BoutonBouger[0].w*0.9,
                               BoutonBouger[0].y-BoutonBouger[0].h*0.1};
    AjusterImageVertical2(&AllerAGaucheRect,textureTuto[1],window_width,window_height,0.1);
    addLayeredImage2(textureTuto[1],AllerAGaucheRect,0,0,2);

    SDL_Rect traitVerticalRect={BoutonBouger[0].x+BoutonBouger[0].w*0.3,
                                BoutonBouger[0].y-BoutonBouger[0].h*0.05};
    SDL_Rect traitVerticalRect2={BoutonBouger[0].x+BoutonBouger[0].w*0.7,
                                BoutonBouger[0].y-BoutonBouger[0].h*0.05};
    AjusterImageVertical2(&traitVerticalRect,textureTuto[3],window_width,window_height,0.0021);
    AjusterImageVertical2(&traitVerticalRect2,textureTuto[3],window_width,window_height,0.0021);
    addLayeredImage2(textureTuto[3],traitVerticalRect,0,0,2);
    addLayeredImage2(textureTuto[3],traitVerticalRect2,0,0,2);

    SDL_Rect PasserEnModeRect={BoutonBouger[0].x+BoutonBouger[0].w*1,
                               BoutonBouger[0].y+BoutonBouger[0].h*0.14};
    AjusterImageVertical2(&PasserEnModeRect,textureTuto[4],window_width,window_height,0.09);
    addLayeredImage2(textureTuto[4],PasserEnModeRect,0,0,2);

    SDL_Rect traitHorizontalRect={BoutonBouger[0].x+BoutonBouger[0].w*1.2,
                                  BoutonBouger[0].y+BoutonBouger[0].h*0.5};
    SDL_Rect traitHorizontalRect2={BoutonBouger[0].x+BoutonBouger[0].w*1.2,
                                   BoutonBouger[0].y+BoutonBouger[0].h*0.65};
    AjusterImageVertical2(&traitHorizontalRect,textureTuto[5],window_width,window_height,0.035);
    AjusterImageVertical2(&traitHorizontalRect2,textureTuto[5],window_width,window_height,0.035);
    addLayeredImage2(textureTuto[5],traitHorizontalRect,0,0,2);
    addLayeredImage2(textureTuto[5],traitHorizontalRect2,0,0,2);

    SDL_Rect ModeParkingRect={BoutonBouger[0].x+BoutonBouger[0].w*1.55,
                               BoutonBouger[0].y+BoutonBouger[0].h*0.4};
    AjusterImageVertical2(&ModeParkingRect,textureTuto[7],window_width,window_height,0.05);
    addLayeredImage2(textureTuto[7],ModeParkingRect,0,0,2);

    SDL_Rect ModeReculRect={BoutonBouger[0].x+BoutonBouger[0].w*1.55,
                               BoutonBouger[0].y+BoutonBouger[0].h*0.55};
    AjusterImageVertical2(&ModeReculRect,textureTuto[8],window_width,window_height,0.048);
    addLayeredImage2(textureTuto[8],ModeReculRect,0,0,2);

    SDL_Rect ModeAvancerRect={BoutonBouger[0].x+BoutonBouger[0].w*1.55,
                               BoutonBouger[0].y+BoutonBouger[0].h*0.7};
    AjusterImageVertical2(&ModeAvancerRect,textureTuto[6],window_width,window_height,0.055);
    addLayeredImage2(textureTuto[6],ModeAvancerRect,0,0,2);

    SDL_Rect MaintenezAccRect={BoutonBouger[0].x+BoutonBouger[0].w*0.15,
                               BoutonBouger[0].y+BoutonBouger[0].h*1.7};
    AjusterImageVertical2(&MaintenezAccRect,textureTuto[9],window_width,window_height,0.11);
    addLayeredImage2(textureTuto[9],MaintenezAccRect,0,0,2);

    SDL_Rect DesactiverFreinAMainRect={BoutonBouger[0].x+BoutonBouger[0].w*0.72,
                               BoutonBouger[0].y+BoutonBouger[0].h*1.7};
    AjusterImageVertical2(&DesactiverFreinAMainRect,textureTuto[10],window_width,window_height,0.12);
    addLayeredImage2(textureTuto[10],DesactiverFreinAMainRect,0,0,2);
}
void tutorielCreneau(SDL_Rect BoutonBouger[10],int window_width, int window_height,SDL_Texture* textureTuto[30]){
    SDL_Rect GarezIciRect={BoutonBouger[0].x-BoutonBouger[0].w*1.55,
                                    BoutonBouger[0].y+BoutonBouger[0].h*0.5};
    AjusterImageVertical2(&GarezIciRect,textureTuto[11],window_width,window_height,0.115);
    addLayeredImage2(textureTuto[11],GarezIciRect,0,0,2);   

    SDL_Rect TerminerManoeuvreRect={BoutonBouger[0].x-BoutonBouger[0].w*2.2,
                                    BoutonBouger[0].y+BoutonBouger[0].h*1.1};
    AjusterImageVertical2(&TerminerManoeuvreRect,textureTuto[12],window_width,window_height,0.115);
    addLayeredImage2(textureTuto[12],TerminerManoeuvreRect,0,0,2); 
}
void tutorielBataille(SDL_Rect BoutonBouger[10],int window_width, int window_height,SDL_Texture* textureTuto[30]){
    SDL_Rect GarezIciRect={BoutonBouger[0].x-BoutonBouger[0].w*2,
                                    BoutonBouger[0].y+BoutonBouger[0].h*0.6};
    AjusterImageVertical2(&GarezIciRect,textureTuto[11],window_width,window_height,0.115);
    addLayeredImage2(textureTuto[11],GarezIciRect,0,0,2);   

    SDL_Rect TerminerManoeuvreRect={BoutonBouger[0].x-BoutonBouger[0].w*2.5,
                                    BoutonBouger[0].y+BoutonBouger[0].h*1.3};
    AjusterImageVertical2(&TerminerManoeuvreRect,textureTuto[12],window_width,window_height,0.115);
    addLayeredImage2(textureTuto[12],TerminerManoeuvreRect,0,0,2); 
}
void tutorielEpi(SDL_Rect BoutonBouger[10],int window_width, int window_height,SDL_Texture* textureTuto[30]){
    SDL_Rect GarezIciRect={BoutonBouger[0].x-BoutonBouger[0].w*2,
                                    BoutonBouger[0].y+BoutonBouger[0].h*0.65};
    AjusterImageVertical2(&GarezIciRect,textureTuto[11],window_width,window_height,0.13);
    addLayeredImage2(textureTuto[11],GarezIciRect,0,0,2);   

    SDL_Rect TerminerManoeuvreRect={BoutonBouger[0].x-BoutonBouger[0].w*2.5,
                                    BoutonBouger[0].y+BoutonBouger[0].h*1.3};
    AjusterImageVertical2(&TerminerManoeuvreRect,textureTuto[12],window_width,window_height,0.115);
    addLayeredImage2(textureTuto[12],TerminerManoeuvreRect,0,0,2); 
}
void tutorielDemiTour(SDL_Rect BoutonBouger[10],int window_width, int window_height,SDL_Texture* textureTuto[30]){
    SDL_Rect GarezIciRect={BoutonBouger[0].x-BoutonBouger[0].w*1.9,
                           BoutonBouger[0].y+BoutonBouger[0].h*1.55};
    AjusterImageVertical2(&GarezIciRect,textureTuto[13],window_width,window_height,0.13);
    addLayeredImage2(textureTuto[13],GarezIciRect,0,0,2);

    SDL_Rect TerminerManoeuvreRect={BoutonBouger[0].x-BoutonBouger[0].w*1.5,
                                    BoutonBouger[0].y+BoutonBouger[0].h*0.3};
    AjusterImageVertical2(&TerminerManoeuvreRect,textureTuto[12],window_width,window_height,0.115);
    addLayeredImage2(textureTuto[12],TerminerManoeuvreRect,0,0,2);
}
void tutoInteraction(SDL_Rect BoutonBouger[10],int window_width, int window_height,SDL_Texture* textureTuto[30]){
    SDL_Rect InteractionTutoRect={BoutonBouger[0].x-BoutonBouger[0].w*7,
                                    BoutonBouger[0].y-BoutonBouger[0].h*1.2};
    AjusterImageVertical2(&InteractionTutoRect,textureTuto[14],window_width,window_height,0.6);
    addLayeredImage2(textureTuto[14],InteractionTutoRect,0,0,2);   
}

//COMMANDE DIRECTION
void AnimVolant(float* VolantRad,int VolantPos,Uint32 delaTime){
    static bool waitBoolVolant=false;
    static int waitCompteVolant=0;
    if(wait(2,&waitBoolVolant,&waitCompteVolant,delaTime,false)){
        switch(VolantPos){
            case 0:
                if(*VolantRad >= -9.424777){*VolantRad -= 0.5;}
                if(*VolantRad <= -9.424777){*VolantRad = -9.424777;}
                break;
            case 1:
                if(*VolantRad > 0.3){*VolantRad -= 0.5;}
                else if(*VolantRad < -0.3){*VolantRad += 0.5;}
                if(*VolantRad >= -0.3 && *VolantRad <= 0.3){*VolantRad = 0;}
                break;
            case 2:
                if(*VolantRad <= 9.424777){*VolantRad += 0.5;}
                if(*VolantRad >= 9.424777){*VolantRad = 9.424777;}
                break;
        }
        waitBoolVolant=false;
        waitCompteVolant=0;
    }
}
void BoutonBougerr(SDL_Texture* textureCommande[30],SDL_Surface* image,SDL_Renderer* renderer,SDL_Rect BoutonBouger[10],SDL_Rect ObjetCreneau[10],int window_width, int window_height,int LevierPos,float* VolantRad,int VolantPos,int freinAmainPos,bool* CreneauON,bool* BatailleON,Uint32 delaTime,SDL_Rect Parkingg[30],int nbObjetCreneau,float coordonneX,float coordonneY){

    SDL_Rect VolantRect={Parkingg[nbObjetCreneau].x+Parkingg[nbObjetCreneau].w*coordonneX,
                         Parkingg[nbObjetCreneau].y+Parkingg[nbObjetCreneau].h*coordonneY};
    AjusterImageVertical2(&VolantRect,textureCommande[0],window_width,window_height,0.1);
    BoutonBouger[0]=VolantRect;
    AnimVolant(VolantRad,VolantPos,delaTime);
    double VolantDeg = *VolantRad*(180/M_PI);
    addLayeredImage2(textureCommande[0],VolantRect,NULL,VolantDeg,2);

    SDL_Rect BoutonTutorielRect={VolantRect.x+VolantRect.w*1.15,
                                 VolantRect.y-VolantRect.h*0.15};
    AjusterImageVertical2(&BoutonTutorielRect,textureCommande[7],window_width,window_height,0.04);
    BoutonBouger[5]=BoutonTutorielRect;
    addLayeredImage2(textureCommande[7],BoutonTutorielRect,0,0,1);

    SDL_Rect AccelerateurRect={VolantRect.x+VolantRect.w*0.7,
                               VolantRect.y+VolantRect.h*1.25};
    AjusterImageVertical2(&AccelerateurRect,textureCommande[1],window_width,window_height,0.02);
    BoutonBouger[1]=AccelerateurRect;
    addLayeredImage2(textureCommande[1],AccelerateurRect,0,0,1);


    if(LevierPos==0){
        SDL_Rect LevierVitesseRect={VolantRect.x+VolantRect.w*1.2,
                                    VolantRect.y+VolantRect.h*0.35,};
        AjusterImageVertical2(&LevierVitesseRect,textureCommande[2],window_width,window_height,0.035);
        BoutonBouger[2]=LevierVitesseRect;
        addLayeredImage2(textureCommande[2],LevierVitesseRect,0,0,1);
    }
    if(LevierPos==1){
        SDL_Rect LevierVitesseRect={VolantRect.x+VolantRect.w*1.2,
                                    VolantRect.y+VolantRect.h*0.35};
        AjusterImageVertical2(&LevierVitesseRect,textureCommande[3],window_width,window_height,0.035);
        BoutonBouger[2]=LevierVitesseRect;
        addLayeredImage2(textureCommande[3],LevierVitesseRect,0,0,1);
    }
    if(LevierPos==2){
        SDL_Rect LevierVitesseRect={VolantRect.x+VolantRect.w*1.2,
                                    VolantRect.y+VolantRect.h*0.35};
        AjusterImageVertical2(&LevierVitesseRect,textureCommande[4],window_width,window_height,0.035);
        BoutonBouger[2]=LevierVitesseRect;
        addLayeredImage2(textureCommande[4],LevierVitesseRect,0,0,1);
    }

    SDL_Rect FreinAmainPosRect={VolantRect.x+VolantRect.w*1.26,
                                VolantRect.y+VolantRect.h*1};

    if(freinAmainPos==0){
        AjusterImageVertical2(&FreinAmainPosRect,textureCommande[5],window_width,window_height,0.027);
        addLayeredImage2(textureCommande[5],FreinAmainPosRect,0,0,1);         
    }
    if(freinAmainPos==1){
        AjusterImageVertical2(&FreinAmainPosRect,textureCommande[6],window_width,window_height,0.027);
        addLayeredImage2(textureCommande[6],FreinAmainPosRect,0,0,1);
    }
    BoutonBouger[3]=FreinAmainPosRect;
}  
void GestionBoutonBougerr(SDL_Event event, int mouseX,int mouseY,SDL_Rect BoutonBouger[10],int* freinAmainPos,int*LevierPos,int*VolantPos,bool* CreneauON,bool* BatailleON,SDL_Texture* textureTuto[30],int window_width, int window_height,bool* tutoBool){
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT){
            //freinAmain
            if(mouseX >= BoutonBouger[3].x && mouseX <= BoutonBouger[3].x + BoutonBouger[3].w && mouseY >= BoutonBouger[3].y && mouseY <= BoutonBouger[3].y + BoutonBouger[3].h){
                if(*freinAmainPos == 0) {
                    *freinAmainPos = 1;
                }
                else {
                    *freinAmainPos = 0;
                }
            }
            //LevierPos
            if (mouseX >= BoutonBouger[2].x && mouseX <= BoutonBouger[2].x + BoutonBouger[2].w && mouseY >= BoutonBouger[2].y && mouseY <= BoutonBouger[2].y + BoutonBouger[2].h*0.33) {
                *LevierPos = 0;
            }
            if (mouseX >= BoutonBouger[2].x && mouseX <= BoutonBouger[2].x + BoutonBouger[2].w && mouseY >= BoutonBouger[2].y + BoutonBouger[2].h*0.33 && mouseY <= BoutonBouger[2].y + BoutonBouger[2].h*0.66) {
                *LevierPos = 1;
            }
            if (mouseX >= BoutonBouger[2].x && mouseX <= BoutonBouger[2].x + BoutonBouger[2].w && mouseY >= BoutonBouger[2].y + BoutonBouger[2].h*0.66 && mouseY <= BoutonBouger[2].y + BoutonBouger[2].h) {
                *LevierPos = 2;
            }
            //VolantPos
            if (mouseX >= BoutonBouger[0].x && mouseX <= BoutonBouger[0].x + BoutonBouger[0].w*0.33 && mouseY >= BoutonBouger[0].y && mouseY <= BoutonBouger[0].y + BoutonBouger[0].h) {
                *VolantPos = 0;       
            }
            if (mouseX >= BoutonBouger[0].x + BoutonBouger[0].w*0.33 && mouseX <= BoutonBouger[0].x + BoutonBouger[0].w*0.66 && mouseY >= BoutonBouger[0].y && mouseY <= BoutonBouger[0].y + BoutonBouger[0].h) {
                *VolantPos = 1;             
            }
            if (mouseX >= BoutonBouger[0].x + BoutonBouger[0].w*0.66 && mouseX <= BoutonBouger[0].x + BoutonBouger[0].w && mouseY >= BoutonBouger[0].y && mouseY <= BoutonBouger[0].y + BoutonBouger[0].h) {
                *VolantPos = 2;         
            }
            if(*freinAmainPos==1){
                if(*LevierPos==0){
                    *CreneauON=false;
                    *BatailleON=false;
                }
            }
            if (mouseX >= BoutonBouger[5].x && mouseX <= BoutonBouger[5].x + BoutonBouger[5].w && mouseY >= BoutonBouger[5].y && mouseY <= BoutonBouger[5].y + BoutonBouger[5].h) {
                *tutoBool = !*tutoBool;
                printf("appui\n");
            }
        }
    }
}
void Mouvement(double* speedx, double* speedy, bool* BoutonGauchePresser,int* mouseX, int* mouseY, SDL_Rect BoutonBouger[10], Uint32 delatime,float* tournerRad,int* LevierPos,int* VolantPos,int* freinAmainPos,Point VoiturePoint[4],Point* Centre,SDL_Rect ObjetCreneau[30],SDL_Surface* image,SDL_Renderer* renderer,int window_width, int window_height,SDL_Point* center,float* tournerDeg){
    if (*BoutonGauchePresser) {
        if (*mouseX >= BoutonBouger[1].x && *mouseX <= BoutonBouger[1].x + BoutonBouger[1].w && *mouseY >= BoutonBouger[1].y && *mouseY <= BoutonBouger[1].y + BoutonBouger[1].h) {
            if(*freinAmainPos == 0){
                if(*LevierPos == 0){
                    *speedx;
                    *speedy;
                }
                if(*LevierPos == 1){
                    *speedx -= (3 * sin(*tournerRad));
                    *speedy += (3 * cos(*tournerRad));
                    //Centre->x -= 15 * cos(*tournerRad);
                    //Centre->y -= 15 * sin(*tournerRad);   
                }
                if(*LevierPos == 2){
                    *speedx += (3 * sin(*tournerRad));
                    *speedy -= (3 * cos(*tournerRad));
                    //Centre->x += 15 * cos(*tournerRad);
                    //Centre->y += 15 * sin(*tournerRad);                                            
                }

                if(*VolantPos == 0 && *LevierPos == 2){
                    *tournerRad -= 0.023;
                }
                if(*VolantPos == 0 && *LevierPos == 1){
                    *tournerRad += 0.023;
                }
                if(*VolantPos == 1){
                    *tournerRad;
                }
                if(*VolantPos == 2 && *LevierPos == 2){
                    *tournerRad += 0.023;
                }
                if(*VolantPos == 2 && *LevierPos == 1){
                    *tournerRad -= 0.023;
                }
            }
        }
    }
}

//VALIDATION
void ImageValidationCreneau(SDL_Texture* textureValidation[30],SDL_Surface* image, SDL_Renderer* renderer, SDL_Rect ObjetCreneau[30],int ValidationPos,int window_width, int window_height, SDL_Rect* ValidRect,float dimension){
    if(ValidationPos==0){

        AjusterImageVertical2(ValidRect,textureValidation[0],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[0],*ValidRect,NULL,0,1);

    }
    if(ValidationPos==1){

        AjusterImageVertical2(ValidRect,textureValidation[1],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[1],*ValidRect,NULL,0,1); 

    }
    if(ValidationPos==2){

        AjusterImageVertical2(ValidRect,textureValidation[2],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[2],*ValidRect,NULL,0,1);  

    } 
}
void ImageValidationBataille(SDL_Texture* textureValidation[30],SDL_Surface* image, SDL_Renderer* renderer, SDL_Rect ObjetCreneau[30],int ValidationPos,int window_width, int window_height, SDL_Rect* ValidRect,float dimension,int tournerDegg){
    if(ValidationPos==0){

        AjusterImageVertical2(ValidRect,textureValidation[3],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[3],*ValidRect,NULL,tournerDegg,1);

    }
    if(ValidationPos==1){

        AjusterImageVertical2(ValidRect,textureValidation[4],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[4],*ValidRect,NULL,tournerDegg,1);

    }
    if(ValidationPos==2){

        AjusterImageVertical2(ValidRect,textureValidation[5],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[5],*ValidRect,NULL,tournerDegg,1);

    }    
}
void ImageValidationEpi(SDL_Texture* textureValidation[30],SDL_Surface* image, SDL_Renderer* renderer, SDL_Rect ObjetCreneau[30],int ValidationPos,int window_width, int window_height, SDL_Rect* ValidRect,float dimension,int tournerDegg){
    if(ValidationPos==0){

        AjusterImageVertical2(ValidRect,textureValidation[6],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[6],*ValidRect,NULL,tournerDegg,1);

    }
    if(ValidationPos==1){

        AjusterImageVertical2(ValidRect,textureValidation[7],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[7],*ValidRect,NULL,tournerDegg,1);

    }
    if(ValidationPos==2){

        AjusterImageVertical2(ValidRect,textureValidation[8],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[8],*ValidRect,NULL,tournerDegg,1);

    }    
}
void ImageValidationInterHaut(SDL_Texture* textureValidation[30],SDL_Surface* image, SDL_Renderer* renderer, SDL_Rect ObjetCreneau[30],int ValidationPos,int window_width, int window_height, SDL_Rect* ValidRect,float dimension){
    if(ValidationPos==0){

        AjusterImageVertical2(ValidRect,textureValidation[9],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[9],*ValidRect,NULL,0,1);

    }
    if(ValidationPos==1){

        AjusterImageVertical2(ValidRect,textureValidation[10],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[10],*ValidRect,NULL,0,1);

    }
    if(ValidationPos==2){

        AjusterImageVertical2(ValidRect,textureValidation[11],window_width,window_height,dimension);
        addLayeredImage2(textureValidation[11],*ValidRect,NULL,0,1);

    }    
}
void Validation(SDL_Texture* textureValidation[30],SDL_Surface* image,SDL_Renderer* renderer,int window_width, int window_height, SDL_Rect ObjetCreneau[30],Uint32 delaTime,bool* afficherValid,int* freinAmainPos,int* LevierPos,int*ValidationPos,Point VoiturePoint[4],bool* BatailleON,bool* toucheMur,bool* toucheVoiture,bool* commencementValid,SDL_Rect Parkingg[30],Point ValidRectPoint[4],SDL_Texture* textureAngle[30],SDL_Rect BoutonBouger[10],bool* mauvaisChoix,bool* Choix,int nbObjetCreneau,float coordonneX,float coordonneY,float dimension,int HorVer){

    int agrandissement=0;

    SDL_Rect ValidRect={Parkingg[nbObjetCreneau].x+Parkingg[nbObjetCreneau].w*coordonneX,
                        Parkingg[nbObjetCreneau].y+Parkingg[nbObjetCreneau].h*coordonneY,
                        10,
                        10};
    int nbTextureValid;
    if(nbObjetCreneau==1 || nbObjetCreneau==5 || (nbObjetCreneau==6 && HorVer==0)){nbTextureValid = 3;}
    if(nbObjetCreneau==0 || nbObjetCreneau==2){nbTextureValid = 0;}
    if(nbObjetCreneau==6 && HorVer==1){nbTextureValid = 11;}
    AjusterImageVertical2(&ValidRect,textureValidation[nbTextureValid],window_width,window_height,dimension);

    static bool ContourVoitureSec = true;
    static Point CentreValidRect;
     
    int actual_size_window = window_width * window_height;
    static int reference_size_window = 0;

    if(actual_size_window != reference_size_window){
        ContourVoitureSec = true;
    }

    if(ContourVoitureSec && nbObjetCreneau==5){
            reference_size_window = window_width * window_height;

            ValidRectPoint[0].x = ValidRect.x;
            ValidRectPoint[0].y = ValidRect.y;

            ValidRectPoint[1].x = ValidRect.x + ValidRect.w;
            ValidRectPoint[1].y = ValidRect.y;

            ValidRectPoint[2].x = ValidRect.x;
            ValidRectPoint[2].y = ValidRect.y + ValidRect.h;

            ValidRectPoint[3].x = ValidRect.x + ValidRect.w;
            ValidRectPoint[3].y = ValidRect.y + ValidRect.h;

        for(int i=0;i<4;i++){
            float originalX = ValidRectPoint[i].x;
            float originalY = ValidRectPoint[i].y;
            float angle = 39 * M_PI / 180.0;

            CentreValidRect.x =  ValidRect.x + ValidRect.w / 2;
            CentreValidRect.y =  ValidRect.y + ValidRect.h / 2;
                
            ValidRectPoint[i].x = (originalX - CentreValidRect.x) * cos(angle) - (originalY - CentreValidRect.y) * sin(angle) + CentreValidRect.x;
            ValidRectPoint[i].y = (originalX - CentreValidRect.x) * sin(angle) + (originalY - CentreValidRect.y) * cos(angle) + CentreValidRect.y;
        }
        ContourVoitureSec = false;
    }

    static SDL_Rect RectNew;

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

    if(*commencementValid){
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

        *commencementValid = false;
        afficher=true;
    }

    if(*Choix){
        if(wait(20,&waitboolValid1,&WaitCompte1,delaTime,stop1)){stop1=true;afficher=false;}
        if(wait(40,&waitboolValid2,&WaitCompte2,delaTime,stop2)){stop2=true;afficher=true;}
        if(wait(60,&waitboolValid3,&WaitCompte3,delaTime,stop3)){stop3=true;afficher=false;}
        if(wait(80,&waitboolValid4,&WaitCompte4,delaTime,stop4)){stop4=true;afficher=true;}
        if(wait(100,&waitboolValid5,&WaitCompte5,delaTime,stop5)){stop5=true;afficher=false;}
    }

    if(*afficherValid){
        if(afficher){
            if(*ValidationPos==0){
                if(nbObjetCreneau==0 || nbObjetCreneau==2){
                    ImageValidationCreneau(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension);
                }
                if(nbObjetCreneau==1 || (nbObjetCreneau==6 && HorVer==0)){
                    ImageValidationBataille(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension,0);
                }  
                if(nbObjetCreneau==5){
                    ImageValidationEpi(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension,39);
                }
                if(nbObjetCreneau==6 && HorVer==1){
                    ImageValidationInterHaut(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension);
                }
                RectNew=ValidRect;
            }
            if(*ValidationPos==1){
                if(nbObjetCreneau==0 || nbObjetCreneau==2){
                    ImageValidationCreneau(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension);
                }
                if(nbObjetCreneau==1 || (nbObjetCreneau==6 && HorVer==0)){
                    ImageValidationBataille(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension,0);
                }  
                if(nbObjetCreneau==5){
                    ImageValidationEpi(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension,39);
                }  
                if(nbObjetCreneau==6 && HorVer==1){
                    ImageValidationInterHaut(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension);
                }
                RectNew=ValidRect;
            }
            if(*ValidationPos==2){
                if(nbObjetCreneau==0 || nbObjetCreneau==2){
                    ImageValidationCreneau(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension);
                }
                if(nbObjetCreneau==1 || (nbObjetCreneau==6 && HorVer==0)){
                    ImageValidationBataille(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension,0);
                }
                if(nbObjetCreneau==5){
                    ImageValidationEpi(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension,39);
                }
                if(nbObjetCreneau==6 && HorVer==1){
                    ImageValidationInterHaut(textureValidation,image,renderer,ObjetCreneau,*ValidationPos,window_width,window_height,&ValidRect,dimension);
                }
                RectNew=ValidRect; 
            }
        }
    } 

    if((*freinAmainPos==1 && *LevierPos==0) || *toucheMur || *toucheVoiture || *mauvaisChoix){

        if(nbObjetCreneau==6){
            agrandissement=10;
        }
        if ((VoiturePoint[0].x >= ValidRect.x - agrandissement && VoiturePoint[0].x <= ValidRect.x + ValidRect.w + agrandissement && VoiturePoint[0].y >= ValidRect.y - agrandissement && VoiturePoint[0].y <= ValidRect.y + ValidRect.h + agrandissement) &&
            (VoiturePoint[1].x >= ValidRect.x - agrandissement && VoiturePoint[1].x <= ValidRect.x + ValidRect.w + agrandissement && VoiturePoint[1].y >= ValidRect.y - agrandissement && VoiturePoint[1].y <= ValidRect.y + ValidRect.h + agrandissement) &&
            (VoiturePoint[2].x >= ValidRect.x - agrandissement && VoiturePoint[2].x <= ValidRect.x + ValidRect.w + agrandissement && VoiturePoint[2].y >= ValidRect.y - agrandissement && VoiturePoint[2].y <= ValidRect.y + ValidRect.h + agrandissement) &&
            (VoiturePoint[2].x >= ValidRect.x - agrandissement && VoiturePoint[3].x <= ValidRect.x + ValidRect.w + agrandissement && VoiturePoint[3].y >= ValidRect.y - agrandissement && VoiturePoint[3].y <= ValidRect.y + ValidRect.h + agrandissement)){
            *ValidationPos=1;
        }else{
            *ValidationPos=2;
        }

        if(nbObjetCreneau == 5){
            if(IsPointInRect(VoiturePoint[0],ValidRectPoint) && IsPointInRect(VoiturePoint[1],ValidRectPoint) && IsPointInRect(VoiturePoint[2],ValidRectPoint) && IsPointInRect(VoiturePoint[3],ValidRectPoint)){
                *ValidationPos=1;
            }else{
                *ValidationPos=2;
            }
        }

        if(*toucheMur && nbObjetCreneau != 6){
            *ValidationPos=2;           
        }

        afficher=true;
        *Choix=false;
        *toucheMur=false;
        *toucheVoiture=false;
        *mauvaisChoix=false;
        
        for(int i=0;i<10;i++){
            BoutonBouger[i].x=0;
            BoutonBouger[i].y=0;
            BoutonBouger[i].w=0;
            BoutonBouger[i].h=0;
        }
    }

    if(nbObjetCreneau==6 && (HorVer==0 || HorVer==1)){
        SDL_Rect AngleBasDroitRect = {ValidRect.x + ValidRect.w*0.98,ValidRect.y + ValidRect.h*0.975};
        AjusterImageVertical2(&AngleBasDroitRect,textureAngle[0],window_width,window_height,0.004);
        addLayeredImage2(textureAngle[0],AngleBasDroitRect,NULL,0,1);

        SDL_Rect AngleBasGaucheRect = {ValidRect.x - ValidRect.w*0.01,ValidRect.y + ValidRect.h*0.975};
        AjusterImageVertical2(&AngleBasGaucheRect,textureAngle[1],window_width,window_height,0.004);
        addLayeredImage2(textureAngle[1],AngleBasGaucheRect,NULL,0,1);

        SDL_Rect AngleHautDroitRect = {ValidRect.x + ValidRect.w*0.98,ValidRect.y - ValidRect.w*0.01};
        AjusterImageVertical2(&AngleHautDroitRect,textureAngle[2],window_width,window_height,0.004);
        addLayeredImage2(textureAngle[2],AngleHautDroitRect,NULL,0,1);

        SDL_Rect AngleHautGaucheRect = {ValidRect.x - ValidRect.w*0.01,ValidRect.y - ValidRect.w*0.01};
        AjusterImageVertical2(&AngleHautGaucheRect,textureAngle[3],window_width,window_height,0.004);
        addLayeredImage2(textureAngle[3],AngleHautGaucheRect,NULL,0,1);
    }
}
void AfficherValidRect(Point ValidRectPoint[4],SDL_Renderer* renderer){
    for(int i=0;i<4;i++){
        SDL_Rect rect = {ValidRectPoint[i].x, ValidRectPoint[i].y, 5, 5};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

//GENS AUTOMATIQUE
void GensBouge(SDL_Renderer* renderer, int window_width, int window_height, SDL_Texture* textureGens[30], SDL_Rect GensRect[30], SDL_Rect Parkingg[30], bool HommeAffBouge[4], Uint32 delaTime, int speedPers[4], bool fin[4], int nb) {
    static bool waitBoolGens[4] = {false, false, false, false};
    static bool waitBoolGens2[4] = {false, false, false, false};
    static int waitCompteGens[4] = {0, 0, 0, 0};
    static int waitCompteGens2[4] = {0, 0, 0, 0};
    static int toggle[4] = {0, 0, 0, 0};

    bool* HommeAffBougee = NULL;
    bool* finn = NULL;
    int* speedPerss = NULL;
    SDL_Rect RectPers;
    SDL_Texture* texture0;
    SDL_Texture* texture1;
    SDL_Texture* texture2;
    
    static SDL_Rect GensHaut;
    static SDL_Rect GensBas;
    static SDL_Rect GensDroite;
    static SDL_Rect GensGauche;

    speedPerss = &speedPers[nb];
    HommeAffBougee = &HommeAffBouge[nb];
    finn = &fin[nb];

    float scaleX;
    float scaleY;
    scaleVoiture(&scaleX,&scaleY,Parkingg,6);

    // Imperatif de garder les rectGens dans les if.
    if (nb == 0) {
        GensHaut.x = Parkingg[6].x + Parkingg[6].w * 0.65 - *speedPerss * scaleX;
        GensHaut.y = Parkingg[6].y + Parkingg[6].h * 0.226;
        AjusterImageVertical2(&GensHaut, textureGens[2], window_width, window_height, 0.035);
        GensRect[0] = GensHaut;
        RectPers = GensHaut;
        texture0 = textureGens[0];
        texture1 = textureGens[1];
        texture2 = textureGens[2];
    } else if (nb == 1) {
        GensBas.x = Parkingg[6].x + Parkingg[6].w * 0.3 + *speedPerss * scaleX;
        GensBas.y = Parkingg[6].y + Parkingg[6].h * 0.705;
        AjusterImageVertical2(&GensBas, textureGens[5], window_width, window_height, 0.026);
        GensRect[1] = GensBas;
        RectPers = GensBas;
        texture0 = textureGens[3];
        texture1 = textureGens[4];
        texture2 = textureGens[5];
    } else if (nb == 2) {
        GensDroite.x = Parkingg[6].x + Parkingg[6].w * 0.65;
        GensDroite.y = Parkingg[6].y + Parkingg[6].h * 0.705  - *speedPerss * scaleY;
        AjusterImageVertical2(&GensDroite, textureGens[8], window_width, window_height, 0.035);
        GensRect[2] = GensDroite;
        RectPers = GensDroite;
        texture0 = textureGens[6];
        texture1 = textureGens[7];
        texture2 = textureGens[8];
    } else if (nb == 3) {
        GensGauche.x = Parkingg[6].x + Parkingg[6].w * 0.30;
        GensGauche.y = Parkingg[6].y + Parkingg[6].h * 0.24 + *speedPerss * scaleY;
        AjusterImageVertical2(&GensGauche, textureGens[11], window_width, window_height, 0.035);
        GensRect[3] = GensGauche;
        RectPers = GensGauche;
        texture0 = textureGens[9];
        texture1 = textureGens[10];
        texture2 = textureGens[11];
    }

    if (*HommeAffBougee) {

        if (wait(0.5, &waitBoolGens[nb], &waitCompteGens[nb], delaTime, false)) {
            *speedPerss += 2;
            waitBoolGens[nb] = false;
            waitCompteGens[nb] = 0;
        }

        if (wait(30, &waitBoolGens2[nb], &waitCompteGens2[nb], delaTime, false)) {
            toggle[nb] = !toggle[nb];
            waitBoolGens2[nb] = false;
            waitCompteGens2[nb] = 0;
        }

        if (toggle[nb] == 0) {
            addLayeredImage2(texture0, RectPers, NULL, 0, 1);
        } else if (toggle[nb] == 1) {
            addLayeredImage2(texture1, RectPers, NULL, 0, 1);
        }
    } else if (*finn == true) {
        addLayeredImage2(texture2, RectPers, NULL, 0, 1);
    }

    if (RectPers.x <= Parkingg[6].x || RectPers.x >= Parkingg[6].x + Parkingg[6].w || RectPers.y >= Parkingg[6].y + Parkingg[6].h || RectPers.y <= Parkingg[6].y) {
        *HommeAffBougee = false;
        *finn = false;
    }
}
void GensBougeDesign(SDL_Renderer* renderer, int window_width, int window_height, SDL_Texture* textureGens[30], SDL_Rect GensRect[30], SDL_Rect Parkingg[30], bool HommeAffBouge[4], Uint32 delaTime, int speedPers[4], bool fin[4], int nb){
    static bool waitBoolGens[4] = {false, false, false, false};
    static bool waitBoolGens2[4] = {false, false, false, false};
    static int waitCompteGens[4] = {0, 0, 0, 0};
    static int waitCompteGens2[4] = {0, 0, 0, 0};
    static int toggle[4] = {0, 0, 0, 0};

    bool* HommeAffBougee = NULL;
    bool* finn = NULL;
    int* speedPerss = NULL;
    SDL_Rect RectPers;
    SDL_Texture* texture0;
    SDL_Texture* texture1;
    SDL_Texture* texture2;
    
    static SDL_Rect GensHaut;
    static SDL_Rect GensBas;
    static SDL_Rect GensDroite;
    static SDL_Rect GensGauche;

    speedPerss = &speedPers[nb];
    HommeAffBougee = &HommeAffBouge[nb];
    finn = &fin[nb];

    float scaleX;
    float scaleY;
    scaleVoiture(&scaleX,&scaleY,Parkingg,6);

    // Imperatif de garder les rectGens dans les if.
    if (nb == 0) {
        GensHaut.x = Parkingg[6].x + Parkingg[6].w * 0.65 - *speedPerss * scaleX;
        GensHaut.y = Parkingg[6].y + Parkingg[6].h * 0.226;
        AjusterImageVertical2(&GensHaut, textureGens[2], window_width, window_height, 0.035);
        GensRect[0] = GensHaut;
        RectPers = GensHaut;
        texture0 = textureGens[0];
        texture1 = textureGens[1];
        texture2 = textureGens[2];
    } else if (nb == 1) {
        GensBas.x = Parkingg[6].x + Parkingg[6].w * 0.3 + *speedPerss * scaleX;
        GensBas.y = Parkingg[6].y + Parkingg[6].h * 0.705;
        AjusterImageVertical2(&GensBas, textureGens[5], window_width, window_height, 0.026);
        GensRect[1] = GensBas;
        RectPers = GensBas;
        texture0 = textureGens[3];
        texture1 = textureGens[4];
        texture2 = textureGens[5];
    } else if (nb == 2) {
        GensDroite.x = Parkingg[6].x + Parkingg[6].w * 0.65;
        GensDroite.y = Parkingg[6].y + Parkingg[6].h * 0.705  - *speedPerss * scaleY;
        AjusterImageVertical2(&GensDroite, textureGens[8], window_width, window_height, 0.035);
        GensRect[2] = GensDroite;
        RectPers = GensDroite;
        texture0 = textureGens[6];
        texture1 = textureGens[7];
        texture2 = textureGens[8];
    } else if (nb == 3) {
        GensGauche.x = Parkingg[6].x + Parkingg[6].w * 0.30;
        GensGauche.y = Parkingg[6].y + Parkingg[6].h * 0.24 + *speedPerss * scaleY;
        AjusterImageVertical2(&GensGauche, textureGens[11], window_width, window_height, 0.035);
        GensRect[3] = GensGauche;
        RectPers = GensGauche;
        texture0 = textureGens[9];
        texture1 = textureGens[10];
        texture2 = textureGens[11];
    }
    if(*finn==true){
        addLayeredImage2(texture2, RectPers, NULL, 0, 1);
    }
}

//VOITURE AUTOMATIQUE
void VoitureDroiteBouge1(SDL_Rect RectVoit,SDL_Rect Parkingg[30],float *tournerVoitureRad){
    if(RectVoit.x < Parkingg[6].x + Parkingg[6].w*0.47 && *tournerVoitureRad > -1.55708){
        *tournerVoitureRad -= 0.01;
    }
    else if(RectVoit.x < Parkingg[6].x + Parkingg[6].w*0.45){
        *tournerVoitureRad = -1.55709;
    }
}
void VoitureDroiteBouge2(SDL_Rect RectVoit,SDL_Rect Parkingg[30],float *tournerVoitureRad){
    if(RectVoit.x < Parkingg[6].x + Parkingg[6].w*0.6 && *tournerVoitureRad < 1.55708){
        *tournerVoitureRad += 0.01;
    }
    else if(RectVoit.x < Parkingg[6].x + Parkingg[6].w*0.6){
        *tournerVoitureRad = 1.55709;
    }
}
void VoitureGaucheBouge1(SDL_Rect RectVoit,SDL_Rect Parkingg[30],float *tournerVoitureRad){
    if(RectVoit.x > Parkingg[6].x + Parkingg[6].w*0.22 && *tournerVoitureRad > -1.5608){
        *tournerVoitureRad -= 0.01;
    }
    else if(RectVoit.x > Parkingg[6].x + Parkingg[6].w*0.22){
        *tournerVoitureRad = -1.5609;
    }
}
void VoitureGaucheBouge2(SDL_Rect RectVoit,SDL_Rect Parkingg[30],float *tournerVoitureRad){
    if(RectVoit.x > Parkingg[6].x + Parkingg[6].w*0.38 && *tournerVoitureRad < 1.55708){
        *tournerVoitureRad += 0.01;
    }
    else if(RectVoit.x > Parkingg[6].x + Parkingg[6].w*0.38){
        *tournerVoitureRad = 1.55709;
    }
}
void VoitureHautBouge1(SDL_Rect RectVoit,SDL_Rect Parkingg[30],float *tournerVoitureRad){
    if(RectVoit.y > Parkingg[6].y + Parkingg[6].h*0.33 && *tournerVoitureRad > -1.5608){
        *tournerVoitureRad -= 0.01;
    }
    else if(RectVoit.y > Parkingg[6].y + Parkingg[6].h*0.33){
        *tournerVoitureRad = -1.5609;
    }
}
void VoitureHautBouge2(SDL_Rect RectVoit,SDL_Rect Parkingg[30],float *tournerVoitureRad){
    if(RectVoit.y > Parkingg[6].y + Parkingg[6].h*0.14 && *tournerVoitureRad < 1.55708){
        *tournerVoitureRad += 0.01;
    }
    else if(RectVoit.y > Parkingg[6].y + Parkingg[6].h*0.14){
        *tournerVoitureRad = 1.55709;
    }
}
void VoitureBougee(SDL_Renderer* renderer, int window_width, int window_height, SDL_Rect Parkingg[30], SDL_Rect Voiture[15], SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],Uint32 delaTime,bool VoitureBouge[3],float tournerVoit[3],int nb,int manoeuvre){
    static bool waitBoolGens[4] = {false, false, false};
    static float waitCompteGens[4] = {0, 0, 0};
    int *speedVoitureX;
    int *speedVoitureY;
    float *tournerVoitureRad;
    double tournerDegg;
    bool* Bougee = NULL;
    bool* finn = NULL;
    SDL_Rect RectVoit;
    SDL_Texture *texture;
    SDL_Point centree;

    float scaleX;
    float scaleY;
    scaleVoiture(&scaleX,&scaleY,Parkingg,6);

    speedVoitureX = &speedVoitX[nb];
    speedVoitureY = &speedVoitY[nb];
    Bougee = &VoitureBouge[nb];
    finn = &Voiturefin[nb];
    tournerVoitureRad = &tournerVoit[nb];

    if(nb==0){
        SDL_Rect VoitureRect = {Parkingg[6].x + Parkingg[6].w * 0.85 - *speedVoitureX * scaleX, Parkingg[6].y + Parkingg[6].h * 0.338 + *speedVoitureY * scaleY};
        AjusterImageVertical2(&VoitureRect, textureVoiture[2], window_width, window_height, 0.1);
        Voiture[9]=VoitureRect;
        RectVoit = VoitureRect;
        texture = textureVoiture[2];

        centree.x = VoitureRect.w * (9.0/10.0);
        centree.y = VoitureRect.h / 2;

        tournerDegg = ((*tournerVoitureRad)*(180/M_PI));
    }

    if(nb==1){
        SDL_Rect VoitureRect2 = {Parkingg[6].x + Parkingg[6].w * 0.005 + *speedVoitureX * scaleX, Parkingg[6].y + Parkingg[6].h * 0.54 + *speedVoitureY * scaleY};
        AjusterImageVertical2(&VoitureRect2, textureVoiture[8], window_width, window_height, 0.095);
        Voiture[10]=VoitureRect2;
        RectVoit = VoitureRect2;
        texture = textureVoiture[8];
        centree.x = VoitureRect2.w * (1.0/10.0);
        centree.y = VoitureRect2.h / 2;

        tournerDegg = -((*tournerVoitureRad)*(180/M_PI));
    }

    if(nb==2){
        SDL_Rect VoitureRect3 = {Parkingg[6].x + Parkingg[6].w * 0.395 + *speedVoitureY * scaleY, Parkingg[6].y + Parkingg[6].h * 0.005 + *speedVoitureX* scaleX};
        AjusterImageVertical2(&VoitureRect3, textureVoiture[7], window_width, window_height, 0.05);
        Voiture[11]=VoitureRect3;
        RectVoit = VoitureRect3;
        texture = textureVoiture[7];
        centree.x = VoitureRect3.w / 2;
        centree.y = VoitureRect3.h * (1.0/10.0);

        tournerDegg = ((*tournerVoitureRad)*(180/M_PI));
    }

    if(*Bougee){
        *speedVoitureX += (3 * cos(*tournerVoitureRad));
        *speedVoitureY -= (3 * sin(*tournerVoitureRad));
        waitBoolGens[nb] = false;
        waitCompteGens[nb] = 0;

        if(manoeuvre == 0 && nb==0){}//Pour dire qu'on touche a rien
        if(manoeuvre == 1 && nb==0){VoitureDroiteBouge1(RectVoit,Parkingg,tournerVoitureRad);}
        if(manoeuvre == 2 && nb==0){VoitureDroiteBouge2(RectVoit,Parkingg,tournerVoitureRad);}

        if(manoeuvre == 0 && nb==1){}//Pour dire qu'on touche a rien
        if(manoeuvre == 1 && nb==1){VoitureGaucheBouge1(RectVoit,Parkingg,tournerVoitureRad);}
        if(manoeuvre == 2 && nb==1){VoitureGaucheBouge2(RectVoit,Parkingg,tournerVoitureRad);}

        if(manoeuvre == 0 && nb==2){}//Pour dire qu'on touche a rien
        if(manoeuvre == 1 && nb==2){VoitureHautBouge1(RectVoit,Parkingg,tournerVoitureRad);}
        if(manoeuvre == 2 && nb==2){VoitureHautBouge2(RectVoit,Parkingg,tournerVoitureRad);}

        addLayeredImage2(texture, RectVoit, &centree, tournerDegg, 0);
    }
    if(*finn){
        addLayeredImage2(texture, RectVoit, &centree, tournerDegg, 0);
    }

    if (RectVoit.x <= Parkingg[6].x || RectVoit.x >= Parkingg[6].x + Parkingg[6].w || RectVoit.y >= Parkingg[6].y + Parkingg[6].h || RectVoit.y <= Parkingg[6].y) {
        *Bougee = false;
        *finn = false;
    }
}
void VoitureBougeDesign(SDL_Renderer* renderer, int window_width, int window_height, SDL_Rect Parkingg[30], SDL_Rect Voiture[15], SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],Uint32 delaTime,bool VoitureBouge[3],float tournerVoit[3],int nb,int manoeuvre){
    static bool waitBoolGens[4] = {false, false, false};
    static float waitCompteGens[4] = {0, 0, 0};
    int *speedVoitureX;
    int *speedVoitureY;
    float *tournerVoitureRad;
    double tournerDegg;
    bool* Bougee = NULL;
    bool* finn = NULL;
    SDL_Rect RectVoit;
    SDL_Texture *texture;
    SDL_Point centree;

    float scaleX;
    float scaleY;
    scaleVoiture(&scaleX,&scaleY,Parkingg,6);

    speedVoitureX = &speedVoitX[nb];
    speedVoitureY = &speedVoitY[nb];
    Bougee = &VoitureBouge[nb];
    finn = &Voiturefin[nb];
    tournerVoitureRad = &tournerVoit[nb];

    if(nb==0){
        SDL_Rect VoitureRect = {Parkingg[6].x + Parkingg[6].w * 0.85 - *speedVoitureX * scaleX, Parkingg[6].y + Parkingg[6].h * 0.338 + *speedVoitureY * scaleY};
        AjusterImageVertical2(&VoitureRect, textureVoiture[2], window_width, window_height, 0.1);
        Voiture[9]=VoitureRect;
        RectVoit = VoitureRect;
        texture = textureVoiture[2];

        centree.x = VoitureRect.w * (9.0/10.0);
        centree.y = VoitureRect.h / 2;

        tournerDegg = ((*tournerVoitureRad)*(180/M_PI));
    }

    if(nb==1){
        SDL_Rect VoitureRect2 = {Parkingg[6].x + Parkingg[6].w * 0.005 + *speedVoitureX * scaleX, Parkingg[6].y + Parkingg[6].h * 0.54 + *speedVoitureY * scaleY};
        AjusterImageVertical2(&VoitureRect2, textureVoiture[8], window_width, window_height, 0.095);
        Voiture[10]=VoitureRect2;
        RectVoit = VoitureRect2;
        texture = textureVoiture[8];
        centree.x = VoitureRect2.w * (1.0/10.0);
        centree.y = VoitureRect2.h / 2;

        tournerDegg = -((*tournerVoitureRad)*(180/M_PI));
    }

    if(nb==2){
        SDL_Rect VoitureRect3 = {Parkingg[6].x + Parkingg[6].w * 0.395 + *speedVoitureY * scaleY, Parkingg[6].y + Parkingg[6].h * 0.005 + *speedVoitureX* scaleX};
        AjusterImageVertical2(&VoitureRect3, textureVoiture[7], window_width, window_height, 0.05);
        Voiture[11]=VoitureRect3;
        RectVoit = VoitureRect3;
        texture = textureVoiture[7];
        centree.x = VoitureRect3.w / 2;
        centree.y = VoitureRect3.h * (1.0/10.0);

        tournerDegg = ((*tournerVoitureRad)*(180/M_PI));
    }
    if(*finn==true){
        addLayeredImage2(texture, RectVoit, &centree, tournerDegg, 0);
    }
}

//FEU AFFICHAGE
void feu1(int window_width, int window_height, SDL_Rect Parkingg[30],SDL_Texture* textureFeu[30]){
    SDL_Rect rectFeuPietonRougeBas={Parkingg[6].x+Parkingg[6].w*0.335,Parkingg[6].y+Parkingg[6].h*0.78};
    AjusterImageVertical2(&rectFeuPietonRougeBas,textureFeu[0],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[0],rectFeuPietonRougeBas,NULL,0,0);

    SDL_Rect rectFeuVertBas={Parkingg[6].x+Parkingg[6].w*0.62,Parkingg[6].y+Parkingg[6].h*0.78};
    AjusterImageVertical2(&rectFeuVertBas,textureFeu[3],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[3],rectFeuVertBas,NULL,0,0);

    SDL_Rect rectFeuPietonRougeHaut={Parkingg[6].x+Parkingg[6].w*0.647,Parkingg[6].y+Parkingg[6].h*0.17};
    AjusterImageVertical2(&rectFeuPietonRougeHaut,textureFeu[4],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[4],rectFeuPietonRougeHaut,NULL,0,0);

    SDL_Rect rectFeuVertHaut={Parkingg[6].x+Parkingg[6].w*0.362,Parkingg[6].y+Parkingg[6].h*0.15};
    AjusterImageVertical2(&rectFeuVertHaut,textureFeu[7],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[7],rectFeuVertHaut,NULL,0,0);

    SDL_Rect rectFeuPietonVertDroite={Parkingg[6].x+Parkingg[6].w*0.71,Parkingg[6].y+Parkingg[6].h*0.705};
    AjusterImageVertical2(&rectFeuPietonVertDroite,textureFeu[9],window_width,window_height,0.023);
    addLayeredImage2(textureFeu[9],rectFeuPietonVertDroite,NULL,0,0);

    SDL_Rect rectFeuRougeDroite={Parkingg[6].x+Parkingg[6].w*0.706,Parkingg[6].y+Parkingg[6].h*0.306};
    AjusterImageVertical2(&rectFeuRougeDroite,textureFeu[10],window_width,window_height,0.035);
    addLayeredImage2(textureFeu[10],rectFeuRougeDroite,NULL,0,0);

    SDL_Rect rectFeuPietonVertGauche={Parkingg[6].x+Parkingg[6].w*0.26,Parkingg[6].y+Parkingg[6].h*0.269};
    AjusterImageVertical2(&rectFeuPietonVertGauche,textureFeu[9],window_width,window_height,0.023);
    addLayeredImage2(textureFeu[13],rectFeuPietonVertGauche,NULL,0,0);

    SDL_Rect rectFeuRougeGauche={Parkingg[6].x+Parkingg[6].w*0.243,Parkingg[6].y+Parkingg[6].h*0.67};
    AjusterImageVertical2(&rectFeuRougeGauche,textureFeu[14],window_width,window_height,0.035);
    addLayeredImage2(textureFeu[14],rectFeuRougeGauche,NULL,0,0);
}
void feu2(int window_width, int window_height, SDL_Rect Parkingg[30],SDL_Texture* textureFeu[30]){
    SDL_Rect rectFeuPietonVertBas={Parkingg[6].x+Parkingg[6].w*0.335,Parkingg[6].y+Parkingg[6].h*0.78};
    AjusterImageVertical2(&rectFeuPietonVertBas,textureFeu[1],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[1],rectFeuPietonVertBas,NULL,0,0);

    SDL_Rect rectFeuRougeBas={Parkingg[6].x+Parkingg[6].w*0.62,Parkingg[6].y+Parkingg[6].h*0.78};
    AjusterImageVertical2(&rectFeuRougeBas,textureFeu[2],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[2],rectFeuRougeBas,NULL,0,0);

    SDL_Rect rectFeuPietonVertHaut={Parkingg[6].x+Parkingg[6].w*0.647,Parkingg[6].y+Parkingg[6].h*0.17};
    AjusterImageVertical2(&rectFeuPietonVertHaut,textureFeu[5],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[5],rectFeuPietonVertHaut,NULL,0,0);

    SDL_Rect rectFeuRougeHaut={Parkingg[6].x+Parkingg[6].w*0.362,Parkingg[6].y+Parkingg[6].h*0.15};
    AjusterImageVertical2(&rectFeuRougeHaut,textureFeu[6],window_width,window_height,0.012);
    addLayeredImage2(textureFeu[6],rectFeuRougeHaut,NULL,0,0);

    SDL_Rect rectFeuPietonRougeDroite={Parkingg[6].x+Parkingg[6].w*0.71,Parkingg[6].y+Parkingg[6].h*0.705};
    AjusterImageVertical2(&rectFeuPietonRougeDroite,textureFeu[8],window_width,window_height,0.023);
    addLayeredImage2(textureFeu[8],rectFeuPietonRougeDroite,NULL,0,0);

    SDL_Rect rectFeuVertDroite={Parkingg[6].x+Parkingg[6].w*0.706,Parkingg[6].y+Parkingg[6].h*0.306};
    AjusterImageVertical2(&rectFeuVertDroite,textureFeu[11],window_width,window_height,0.035);
    addLayeredImage2(textureFeu[11],rectFeuVertDroite,NULL,0,0);

    SDL_Rect rectFeuPietonRougeGauche={Parkingg[6].x+Parkingg[6].w*0.26,Parkingg[6].y+Parkingg[6].h*0.269};
    AjusterImageVertical2(&rectFeuPietonRougeGauche,textureFeu[12],window_width,window_height,0.023);
    addLayeredImage2(textureFeu[12],rectFeuPietonRougeGauche,NULL,0,0);

    SDL_Rect rectFeuVertGauche={Parkingg[6].x+Parkingg[6].w*0.243,Parkingg[6].y+Parkingg[6].h*0.67};
    AjusterImageVertical2(&rectFeuVertGauche,textureFeu[15],window_width,window_height,0.035);
    addLayeredImage2(textureFeu[15],rectFeuVertGauche,NULL,0,0);
}

//SCENARIO ET DESIGN
void scenario1(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3]){
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,2);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,2);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,2);

    for(int i=0;i<4;i++){
        if(VoitureAppuit[0]==false){
            if(VoiturePoint[i].y<=Parkingg[6].y+Parkingg[6].h*0.5){
                *mauvaisChoix=true;
            } 
        }
    }
    if(VoitureAppuit[1]==true || VoitureAppuit[2]==true){
        *mauvaisChoix=true;
    }
}
void scenarioDesign1(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,2);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,2);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,2);
}
void scenario2(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3]){
    for(int i=0;i<4;i++){
        if(Voiture[11].y>Parkingg[6].y+Parkingg[6].h*0.4 && VoiturePoint[i].y>Parkingg[6].y+Parkingg[6].h*0.6 && VoitureAppuit[2] == true){
            VoitureBouge[2]=false;
        }else if(VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.6 && VoitureAppuit[2] == true){
            VoitureBouge[2]=true;
        }
        if(VoiturePoint[i].x<Parkingg[6].x+Parkingg[6].w*0.5 && VoitureAppuit[2] == false){
            *mauvaisChoix=true;
        }
        if(VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.5 && VoitureAppuit[0] == false){
            *mauvaisChoix=true;
        }
        if(VoitureAppuit[1] == true){
            *mauvaisChoix=true;
        }
        if (Voiture[11].x <= Parkingg[6].x || Voiture[11].x >= Parkingg[6].x + Parkingg[6].w || Voiture[11].y >= Parkingg[6].y + Parkingg[6].h || Voiture[11].y <= Parkingg[6].y) {
            VoitureBouge[2] = false;
        }
    }
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,2);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,1);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,1);
}
void scenarioDesign2(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,2);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,1);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,1);
}
void scenario3(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    for(int i=0;i<4;i++){
        if(VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.78 && GensRect[1].x<Parkingg[6].x+Parkingg[6].w*0.63){
            *mauvaisChoix=true;
        }
        if(VoiturePoint[i].x>Parkingg[6].x+Parkingg[6].w*0.65 && GensRect[2].y>Parkingg[6].y+Parkingg[6].h*0.28){
            *mauvaisChoix=true;
        }
        if(VoiturePoint[i].x>Parkingg[6].x+Parkingg[6].w*0.65 && GensRect[2].y>Parkingg[6].y+Parkingg[6].h*0.28){
            *mauvaisChoix=true;
        }
        if(VoitureAppuit[1] || VoitureAppuit[0]){
            *mauvaisChoix=true;
        }
    }

    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,2);

    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
}
void scenarioDesign3(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,2);

    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
}
void scenario4(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    for(int i=0;i<4;i++){
        if(VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.78 && GensRect[1].x<Parkingg[6].x+Parkingg[6].w*0.63){
            *mauvaisChoix=true;
        }
        if(VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.5 && VoitureAppuit[0] == false){
            *mauvaisChoix=true;
        }
        if(VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.3 && GensRect[0].x>Parkingg[6].x+Parkingg[6].w*0.35){
            *mauvaisChoix=true;
        }
        if(GensAppuit[3] == true){
            *mauvaisChoix=true;
        }
        if(VoitureAppuit[2] == true){
            *mauvaisChoix=true;
        }
        if(Voiture[9].y>Parkingg[6].y+Parkingg[6].h*0.45 && GensAppuit[1] == false){
            VoitureBouge[0] = false;
        }else if(GensAppuit[1] == true && Voiture[9].y>Parkingg[6].y+Parkingg[6].h*0.45 && GensRect[1].x > Parkingg[6].x + Parkingg[6].w * 0.6){
            VoitureBouge[0] = true;
        }
        if (Voiture[9].x <= Parkingg[6].x || Voiture[9].x >= Parkingg[6].x + Parkingg[6].w || Voiture[9].y >= Parkingg[6].y + Parkingg[6].h || Voiture[9].y <= Parkingg[6].y) {
            VoitureBouge[0] = false;
        }
    }
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,1);

    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
}
void scenarioDesign4(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,1);

    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
}
void scenario5(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){

    for(int i=0;i<4;i++){
        if(Voiture[9].x<Parkingg[6].x+Parkingg[6].w*0.72  && VoitureAppuit[0] == true && GensRect[2].y>Parkingg[6].y+Parkingg[6].h*0.3){
           VoitureBouge[0] = false;
        }else if(GensRect[2].y<Parkingg[6].y+Parkingg[6].h*0.3 && VoitureAppuit[0] == true){
           VoitureBouge[0] = true;
        }
        if(GensRect[0].x>Parkingg[6].x+Parkingg[6].w*0.3 && VoitureAppuit[2] == true){
            VoitureBouge[2]=false;
        }
        if(Voiture[11].y>Parkingg[6].y+Parkingg[6].h*0.4 && VoiturePoint[i].y>Parkingg[6].y+Parkingg[6].h*0.6 && VoitureAppuit[2] == true){
            VoitureBouge[2]=false;
        }else if(VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.6 && VoitureAppuit[2] == true && GensRect[0].x<Parkingg[6].x+Parkingg[6].w*0.3){
            VoitureBouge[2]=true;
        }
        if(Voiture[9].x>Parkingg[6].x+Parkingg[6].w*0.7 && VoiturePoint[i].y<Parkingg[6].y+Parkingg[6].h*0.5){
            *mauvaisChoix=true;
        }
        if(Voiture[11].y<Parkingg[6].y+Parkingg[6].h*0.4 && VoiturePoint[i].x<Parkingg[6].x+Parkingg[6].w*0.5){
            *mauvaisChoix=true;
        }
        if(GensRect[3].y<Parkingg[6].y+Parkingg[6].h*0.7 && VoiturePoint[i].x<Parkingg[6].x+Parkingg[6].w*0.35){
            *mauvaisChoix=true;
        }
        if (Voiture[11].x <= Parkingg[6].x || Voiture[11].x >= Parkingg[6].x + Parkingg[6].w || Voiture[11].y >= Parkingg[6].y + Parkingg[6].h || Voiture[11].y <= Parkingg[6].y) {
            VoitureBouge[2] = false;
        }
    }
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,1);

    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
}
void scenarioDesign5(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,1);

    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
}
void scenario6(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){

    for(int i=0;i<4;i++){
        if(GensAppuit[1] || GensAppuit[0] || GensAppuit[3]){
            *mauvaisChoix=true;
        }
        if(VoitureAppuit[1] || VoitureAppuit[2]){
            *mauvaisChoix=true;
        }
        if(VoiturePoint[i].x>Parkingg[6].x+Parkingg[6].w*0.65 && GensRect[2].y>Parkingg[6].y+Parkingg[6].h*0.28){
            *mauvaisChoix=true;
        }
    }
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,1);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,0);

    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);

    feu1(window_width,window_height,Parkingg,textureFeu);
}
void scenarioDesign6(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,1);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,0);

    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);

    feu1(window_width,window_height,Parkingg,textureFeu);
}
void scenario7(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){

    for(int i=0;i<4;i++){
        if(GensAppuit[1] || GensAppuit[0] || GensAppuit[2]){
            *mauvaisChoix=true;            
        }
        if(VoitureAppuit[0] || VoitureAppuit[2]){
            *mauvaisChoix=true;
        }
    }
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,0);

    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);

    feu1(window_width,window_height,Parkingg,textureFeu);
}
void scenarioDesign7(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,0,1);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,0);

    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,2);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,1);

    feu1(window_width,window_height,Parkingg,textureFeu);
}
void scenario8(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    for(int i=0;i<4;i++){
        if(GensAppuit[0]){
            *mauvaisChoix=true;           
        }
        if(VoitureAppuit[1]){
            *mauvaisChoix=true;
        }
        if(GensRect[3].y<Parkingg[6].y+Parkingg[6].h*0.7 && VoiturePoint[i].x<Parkingg[6].x+Parkingg[6].w*0.35){
            *mauvaisChoix=true;
        }
        if(VoitureAppuit[2]==false && VoiturePoint[i].x<Parkingg[6].x+Parkingg[6].w*0.5){
            *mauvaisChoix=true;
        }
        if(GensRect[3].y<Parkingg[6].y+Parkingg[6].h*0.7 && Voiture[11].y>Parkingg[6].y+Parkingg[6].h*0.22){
            VoitureBouge[2]=false;
        }else if(GensRect[3].y>Parkingg[6].y+Parkingg[6].h*0.7 && VoitureAppuit[2]){
            VoitureBouge[2]=true;
        }
        if (Voiture[11].x <= Parkingg[6].x || Voiture[11].x >= Parkingg[6].x + Parkingg[6].w || Voiture[11].y >= Parkingg[6].y + Parkingg[6].h || Voiture[11].y <= Parkingg[6].y) {
            VoitureBouge[2] = false;
        }
    }
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,1);
    VoitureBougee(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,2);

    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBouge(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);

    feu1(window_width,window_height,Parkingg,textureFeu);
}
void scenarioDesign8(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,1,1);
    VoitureBougeDesign(renderer,window_width,window_height,Parkingg,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,delaTime,VoitureBouge,tournerVoit,2,2);

    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,0);
    GensBougeDesign(renderer,window_width,window_height,textureGens,GensRect,Parkingg,HommeAffBouge,delaTime,speedPers,Gensfin,3);

    feu1(window_width,window_height,Parkingg,textureFeu);
}
void PresentationIntersection(SDL_Renderer* renderer, int window_width, int window_height,SDL_Rect Parkingg[30],SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],Uint32 delaTime,int speedPers[4],bool Gensfin[4],SDL_Rect Voiture[15],SDL_Texture* textureVoiture[30],bool Voiturefin[3],int speedVoitX[3],int speedVoitY[3],bool VoitureBouge[3],float tournerVoit[3],SDL_Texture* textureFeu[30],Point VoiturePoint[4],bool*CreneauON,bool* mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4]){
    feu1(window_width,window_height,Parkingg,textureFeu);
    
    SDL_Rect VoitureRect = {Parkingg[6].x + Parkingg[6].w * 0.85, Parkingg[6].y + Parkingg[6].h * 0.338};
    AjusterImageVertical2(&VoitureRect, textureVoiture[2], window_width, window_height, 0.1);
    addLayeredImage2(textureVoiture[2], VoitureRect, 0, 0, 1);

    SDL_Rect VoitureRect2 = {Parkingg[6].x + Parkingg[6].w * 0.005, Parkingg[6].y + Parkingg[6].h * 0.54};
    AjusterImageVertical2(&VoitureRect2, textureVoiture[8], window_width, window_height, 0.095);
    addLayeredImage2(textureVoiture[8], VoitureRect2, 0, 0, 1);

    SDL_Rect VoitureRect3 = {Parkingg[6].x + Parkingg[6].w * 0.395, Parkingg[6].y + Parkingg[6].h * 0.005};
    AjusterImageVertical2(&VoitureRect3, textureVoiture[7], window_width, window_height, 0.05);
    addLayeredImage2(textureVoiture[7], VoitureRect3, 0, 0, 1);

    SDL_Rect GensHaut = {Parkingg[6].x + Parkingg[6].w * 0.65,GensHaut.y = Parkingg[6].y + Parkingg[6].h * 0.226};
    AjusterImageVertical2(&GensHaut, textureGens[2], window_width, window_height, 0.035);
    addLayeredImage2(textureGens[2], GensHaut, 0, 0, 1);

    SDL_Rect GensBas = {GensBas.x = Parkingg[6].x + Parkingg[6].w * 0.3,Parkingg[6].y + Parkingg[6].h * 0.705};
    AjusterImageVertical2(&GensBas, textureGens[5], window_width, window_height, 0.026);
    addLayeredImage2(textureGens[5], GensBas, 0, 0, 1);

    SDL_Rect GensDroite = {Parkingg[6].x + Parkingg[6].w * 0.65,Parkingg[6].y + Parkingg[6].h * 0.705};
    AjusterImageVertical2(&GensDroite, textureGens[8], window_width, window_height, 0.035);
    addLayeredImage2(textureGens[8], GensDroite, 0, 0, 1);

    SDL_Rect GensGauche = {Parkingg[6].x + Parkingg[6].w * 0.30,Parkingg[6].y + Parkingg[6].h * 0.24};
    AjusterImageVertical2(&GensGauche, textureGens[11], window_width, window_height, 0.035);
    addLayeredImage2(textureGens[11], GensGauche, 0, 0, 1);
}

//GESTION COMMANDE
void GestionCommandes(SDL_Event event, bool* EnCour,int* mouseX, int* mouseY,SDL_Rect BoutonMenue[10], SDL_Rect ObjetCreneau[30], EtatLogiciel* Etat,SDL_Rect BoutonBouger[10],bool *CrenauON, double* speedx, double* speedy,float* tournerRad,int*LevierPos,int*freinAmainPos,int*VolantPos, bool* BoutonGauchePresser, bool* afficherValid,int*ValidationPos, bool* ColiderInitialisation, bool*toucheMur,bool* toucheVoiture,bool* BatailleON,bool *commencementValid,int* waitComptePlay,bool* waitboolPlay,bool HommeAffBouge[4],SDL_Rect GensRect[30],int speedPers[4],bool Gensfin[4],bool VoitureBouge[3],SDL_Rect Voiture[15],int speedVoitX[3],bool Voiturefin[3],float tournerVoit[3],int speedVoitY[3],bool *mauvaisChoix,bool VoitureAppuit[3],bool GensAppuit[4],int* nbScenario,bool* PresIntersection,bool* tutoBool){
    if (event.type == SDL_QUIT) {
        *EnCour=false;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT){
            SDL_GetMouseState(mouseX,mouseY);
            *BoutonGauchePresser = true;
            if(*Etat == 0){
                if(*mouseX >= BoutonMenue[2].x && *mouseX <= BoutonMenue[2].x+BoutonMenue[2].w && *mouseY >= BoutonMenue[2].y && *mouseY <= BoutonMenue[2].y + BoutonMenue[2].h){
                    *EnCour=false;
                }
                if(*mouseX >= BoutonMenue[0].x && *mouseX <= BoutonMenue[0].x+BoutonMenue[0].w && *mouseY >= BoutonMenue[0].y && *mouseY <= BoutonMenue[0].y + BoutonMenue[0].h){
                    *Etat = CRENEAU_BATAILLE;
                }
                if(*mouseX >= BoutonMenue[1].x && *mouseX <= BoutonMenue[1].x+BoutonMenue[1].w && *mouseY >= BoutonMenue[1].y && *mouseY <= BoutonMenue[1].y + BoutonMenue[1].h){
                    *Etat = INTERSECTION;
                    *PresIntersection=true;
                }
                if(*mouseX >= BoutonMenue[3].x && *mouseX <= BoutonMenue[3].x+BoutonMenue[3].w && *mouseY >= BoutonMenue[3].y && *mouseY <= BoutonMenue[3].y + BoutonMenue[3].h){
                    *Etat = DEMITOUR_EPI;
                }
                if(*mouseX >= BoutonMenue[4].x && *mouseX <= BoutonMenue[4].x+BoutonMenue[4].w && *mouseY >= BoutonMenue[4].y && *mouseY <= BoutonMenue[4].y + BoutonMenue[4].h){
                    *Etat = ROND_POINT;
                }
            }
            if(*Etat == 1){
                if(*mouseX >= ObjetCreneau[14].x && *mouseX <= ObjetCreneau[14].x+ObjetCreneau[14].w && *mouseY >= ObjetCreneau[14].y && *mouseY <= ObjetCreneau[14].y + ObjetCreneau[14].h){
                    *Etat = 0;
                    *CrenauON=false;
                    *BatailleON=false;
                    *afficherValid=false;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;  
                    *LevierPos=2; 
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;  
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=false;
                    *waitComptePlay=0;
                    *waitboolPlay=false;
                    *mauvaisChoix=false;
                    *tutoBool=false; 
                }
                if(*CrenauON==false && *mouseX >= ObjetCreneau[2].x && *mouseX <= ObjetCreneau[2].x+ObjetCreneau[2].w && *mouseY >= ObjetCreneau[2].y && *mouseY <= ObjetCreneau[2].y+ObjetCreneau[2].h){
                    *CrenauON=true;
                    *BatailleON=false;
                    *afficherValid=true;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;  
                    *LevierPos=2; 
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;  
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=true;
                    *waitComptePlay=0;
                    *waitboolPlay=false;
                    *mauvaisChoix=false;
                    *tutoBool=false; 
                }     
                if(*BatailleON==false && *mouseX >= ObjetCreneau[13].x && *mouseX <= ObjetCreneau[13].x+ObjetCreneau[13].w && *mouseY >= ObjetCreneau[13].y && *mouseY <= ObjetCreneau[13].y+ObjetCreneau[13].h){
                    *BatailleON=true;
                    *CrenauON=false;
                    *afficherValid=true;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;
                    *LevierPos=2;
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;  
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=true;    
                    *waitComptePlay=0;
                    *waitboolPlay=false;     
                    *mauvaisChoix=false; 
                    *tutoBool=false;         
                }    
            }
            if(*Etat == 2){
                if(*mouseX >= ObjetCreneau[14].x && *mouseX <= ObjetCreneau[14].x+ObjetCreneau[14].w && *mouseY >= ObjetCreneau[14].y && *mouseY <= ObjetCreneau[14].y + ObjetCreneau[14].h){
                    *Etat = 0;
                    *CrenauON=false;
                    *BatailleON=false;
                    *afficherValid=false;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;  
                    *LevierPos=2; 
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;  
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=false;
                    *waitComptePlay=0;
                    *waitboolPlay=false;
                    *mauvaisChoix=false;
                    *tutoBool=false; 
                }
                if(*CrenauON==false && *mouseX >= ObjetCreneau[2].x && *mouseX <= ObjetCreneau[2].x+ObjetCreneau[2].w && *mouseY >= ObjetCreneau[2].y && *mouseY <= ObjetCreneau[2].y+ObjetCreneau[2].h){
                    *CrenauON=true;
                    *BatailleON=false;
                    *afficherValid=true;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;  
                    *LevierPos=2; 
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;  
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=true;
                    *waitComptePlay=0;
                    *waitboolPlay=false;
                    *mauvaisChoix=false;
                    *tutoBool=false; 
                }     
                if(*BatailleON==false && *mouseX >= ObjetCreneau[13].x && *mouseX <= ObjetCreneau[13].x+ObjetCreneau[13].w && *mouseY >= ObjetCreneau[13].y && *mouseY <= ObjetCreneau[13].y+ObjetCreneau[13].h){
                    *BatailleON=true;
                    *CrenauON=false;
                    *afficherValid=true;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;
                    *LevierPos=2;
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;  
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=true;    
                    *waitComptePlay=0;
                    *waitboolPlay=false;    
                    *mauvaisChoix=false;
                    *tutoBool=false;         
                }    
            }
            if(*Etat == 3){
                if(*mouseX >= ObjetCreneau[14].x && *mouseX <= ObjetCreneau[14].x+ObjetCreneau[14].w && *mouseY >= ObjetCreneau[14].y && *mouseY <= ObjetCreneau[14].y + ObjetCreneau[14].h){
                    *Etat = 0;
                    *afficherValid = false;
                }
            }
            if(*Etat == 4){
                if(*mouseX >= ObjetCreneau[14].x && *mouseX <= ObjetCreneau[14].x+ObjetCreneau[14].w && *mouseY >= ObjetCreneau[14].y && *mouseY <= ObjetCreneau[14].y + ObjetCreneau[14].h){
                    *Etat = 0;
                    *CrenauON=false;
                    *BatailleON=false;
                    *afficherValid=false;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;  
                    *LevierPos=2;
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=false;
                    *waitComptePlay=0;
                    *waitboolPlay=false;
                    *mauvaisChoix=false;
                    *tutoBool=false;

                    for(int i=0;i<4;i++){
                        speedPers[i]=0;
                        Gensfin[i]=true;
                        HommeAffBouge[i]=false;
                        GensAppuit[i]=false;
                    }
                    for(int i=0;i<3;i++){
                        speedVoitX[i]=0;
                        speedVoitY[i]=0;
                        tournerVoit[i]=0;
                        VoitureBouge[i] = false;
                        Voiturefin[i]=true;
                        VoitureAppuit[i]=false;
                    }
                }
                if(*CrenauON==false && *mouseX >= ObjetCreneau[2].x && *mouseX <= ObjetCreneau[2].x+ObjetCreneau[2].w && *mouseY >= ObjetCreneau[2].y && *mouseY <= ObjetCreneau[2].y+ObjetCreneau[2].h){
                    *CrenauON=true;
                    *BatailleON=false;
                    *afficherValid=true;
                    *speedx=0;
                    *speedy=0;
                    *tournerRad=0;
                    *VolantPos=1;
                    *freinAmainPos=0;
                    *LevierPos=2; 
                    *ValidationPos=0;     
                    *ColiderInitialisation = true;  
                    *toucheMur = false;
                    *toucheVoiture = false;
                    *commencementValid=true;
                    *waitComptePlay=0;
                    *waitboolPlay=false;
                    *mauvaisChoix=false;
                    *nbScenario=random_number();
                    *PresIntersection=false;
                    *tutoBool=false; 

                    for(int i=0;i<4;i++){
                        speedPers[i]=0;
                        Gensfin[i]=true;
                        HommeAffBouge[i]=false;
                        GensAppuit[i]=false;
                    }
                    for(int i=0;i<3;i++){
                        speedVoitX[i]=0;
                        speedVoitY[i]=0;
                        tournerVoit[i]=0;
                        VoitureBouge[i] = false;
                        Voiturefin[i]=true;
                        VoitureAppuit[i]=false;
                    }                
                }
                for(int i=0;i<4;i++){
                    if(*mouseX >= GensRect[i].x && *mouseX <= GensRect[i].x+GensRect[i].w && *mouseY >= GensRect[i].y && *mouseY <= GensRect[i].y + GensRect[i].h){
                        HommeAffBouge[i] = true;
                        GensAppuit[i] = true;
                    }
                }
                for (int i = 9; i < 12; i++) {
                    int t = i - 9;
                    if (*mouseX >= Voiture[i].x && *mouseX <= Voiture[i].x + Voiture[i].w && *mouseY >= Voiture[i].y && *mouseY <= Voiture[i].y + Voiture[i].h) {
                        VoitureBouge[t] = true;
                        VoitureAppuit[t]=true;
                    }
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

//ONGLET
void DemiTourEpi(SDL_Renderer* renderer,SDL_Surface* image,int window_width,int window_height,SDL_Texture* textureBoutonDiver[30],
                 SDL_Texture* textureCreBa[30],SDL_Texture* textureParking[30],SDL_Texture* textureCommande[30],SDL_Texture* textureVoiture[30], 
                 SDL_Rect ObjetCreneau[30],TTF_Font* fontMoyen,SDL_Rect Parkingg[30],bool*CreneauON,bool*BatailleON,
                 SDL_Rect Voiture[15],Uint32 delaTime,bool* waitboolPlay,int* waitComptePlay,SDL_Rect BoutonBouger[10],int* VolantPos,int* freinAmainPos,
                 int* LevierPos,double* speedx,double* speedy,int* mouseX,int* mouseY, float* tournerRad, float* tournerDeg, float* VolantRad,
                 SDL_Texture* textureValidation[30],bool* BoutonGauchePresser,Point VoiturePoint[4],Point* Centre,SDL_Point* center,
                 bool* toucheMur,bool* toucheVoiture,bool* afficherValid,int* ValidationPos,bool* commencementValid,bool* ColiderInitialisation,
                 Point ValidRectPoint[4],Point VoitureSec[12],SDL_Texture* textureAngle[30],bool* mauvaisChoix,SDL_Texture* textureTuto[30],
                 bool* tutoBool){
    static int ParkingEnCour=4;

    Quitter(window_width,window_height,textureBoutonDiver,ObjetCreneau);
    FondBase(window_width,window_height,textureCreBa);
    Ecrire(window_width,window_height,renderer,fontMoyen,0.015,0.346,"Simulation Demi Tour et Epi",0.3);
    ParkingEpi(textureParking,window_width,window_height,Parkingg);
    Play(textureBoutonDiver,image,renderer,ObjetCreneau,CreneauON,BatailleON,window_width,window_height,Parkingg,
         2,5);
    ChargementVoitureEpi(textureVoiture,image,renderer,window_width,window_height,Voiture,Parkingg);
    DecorEpiDemiTour(image,renderer,window_width,window_height,textureCreBa,Parkingg);
            
    if(*CreneauON){
        ParkingEnCour=0;// dernier utiliser CreneauON
    }
    if(*BatailleON){
        ParkingEnCour=1;// dernier utiliser BatailleON
    }
    if(*CreneauON){
        if(wait(30,waitboolPlay,waitComptePlay,delaTime,false)){
            BoutonBougerr(textureCommande,image, renderer,BoutonBouger,ObjetCreneau,window_width,window_height,*LevierPos,VolantRad,*VolantPos,*freinAmainPos,CreneauON,BatailleON,delaTime,Parkingg,
                          2,1.1,0.3);
            Mouvement(speedx,speedy,BoutonGauchePresser,mouseX,mouseY,BoutonBouger,delaTime,tournerRad,LevierPos,VolantPos,freinAmainPos,VoiturePoint,Centre,ObjetCreneau,image,renderer,window_width,window_height,center,tournerDeg);
            VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                              2,0.575,0.75);
            ColiderVoiture(speedx, speedy, tournerRad, LevierPos, VolantPos, freinAmainPos, VoiturePoint, Centre, ObjetCreneau, window_width, window_height, CreneauON, center,mouseX,mouseY,BoutonGauchePresser,BoutonBouger,ColiderInitialisation,Parkingg,
                           2);
            DetectionCollision(BoutonBouger,ObjetCreneau,toucheMur,toucheVoiture,VoiturePoint,CreneauON,Voiture,Parkingg,
                               2);
            DetectionColisionTrottoir(VoiturePoint,3,toucheMur,CreneauON,Parkingg);
            DetectionColisionTrottoir(VoiturePoint,4,toucheMur,CreneauON,Parkingg);
            if(*tutoBool){
                tutoriel(BoutonBouger,window_width,window_height,textureTuto);
                tutorielDemiTour(BoutonBouger,window_width,window_height,textureTuto);
            }
        }
    }
    else if(ParkingEnCour==1){
        Validation(textureValidation,image,renderer,window_width,window_height,ObjetCreneau,delaTime,afficherValid,freinAmainPos,LevierPos,ValidationPos,VoiturePoint,BatailleON,toucheMur,toucheVoiture,commencementValid,Parkingg,ValidRectPoint,textureAngle,BoutonBouger,mauvaisChoix,
                   BatailleON,5,0.55,0.582,0.1195,0);
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,0,0,0,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          2,0.575,0.75);
    }
    else{
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          2,0.575,0.75);
    }
    if(*BatailleON){
        if(wait(30,waitboolPlay,waitComptePlay,delaTime,false)){
            BoutonBougerr(textureCommande,image, renderer,BoutonBouger,ObjetCreneau,window_width,window_height,*LevierPos,VolantRad,*VolantPos,*freinAmainPos,CreneauON,BatailleON,delaTime,Parkingg,
                          5,1.1,0.3);
            VoitureColiderSecondaire(Voiture,window_width,window_height,VoitureSec);
            Mouvement(speedx,speedy,BoutonGauchePresser,mouseX,mouseY,BoutonBouger,delaTime,tournerRad,LevierPos,VolantPos,freinAmainPos,VoiturePoint,Centre,ObjetCreneau,image,renderer,window_width,window_height,center,tournerDeg);
            VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                              5,0.34,0.78);
            ColiderVoiture(speedx, speedy, tournerRad, LevierPos, VolantPos, freinAmainPos, VoiturePoint, Centre, ObjetCreneau, window_width, window_height, CreneauON, center,mouseX,mouseY,BoutonGauchePresser,BoutonBouger,ColiderInitialisation,Parkingg,
                           5);
            DetectionCollision(BoutonBouger,ObjetCreneau,toucheMur,toucheVoiture,VoiturePoint,CreneauON,Voiture,Parkingg,
                               5);
            DetectionCollisionEpiVoiture(VoiturePoint,VoitureSec,BatailleON,toucheVoiture);
            if(*tutoBool){
                tutoriel(BoutonBouger,window_width,window_height,textureTuto);
                tutorielEpi(BoutonBouger,window_width,window_height,textureTuto);
            }
        }
    }
    else if(ParkingEnCour==0){
        Validation(textureValidation,image,renderer,window_width,window_height,ObjetCreneau,delaTime,afficherValid,freinAmainPos,LevierPos,ValidationPos,VoiturePoint,BatailleON,toucheMur,toucheVoiture,commencementValid,Parkingg,ValidRectPoint,textureAngle,BoutonBouger,mauvaisChoix,
                   CreneauON,2,0.22,0.72,0.063,0);
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,0,0,0,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          5,0.34,0.78);
    }
    else{
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          5,0.34,0.78);
    }   
}

void CreneauBataile(SDL_Renderer* renderer,SDL_Surface* image,int window_width,int window_height,SDL_Texture* textureBoutonDiver[30],
                    SDL_Texture* textureCreBa[30],SDL_Texture* textureParking[30],SDL_Texture* textureCommande[30],SDL_Texture* textureVoiture[30], 
                    SDL_Rect ObjetCreneau[30],TTF_Font* fontMoyen,SDL_Rect Parkingg[30],bool*CreneauON,bool*BatailleON,
                    SDL_Rect Voiture[15],Uint32 delaTime,bool* waitboolPlay,int* waitComptePlay,SDL_Rect BoutonBouger[10],int* VolantPos,int* freinAmainPos,
                    int* LevierPos,double* speedx,double* speedy,int* mouseX,int* mouseY, float* tournerRad, float* tournerDeg, float* VolantRad,
                    SDL_Texture* textureValidation[30],bool* BoutonGauchePresser,Point VoiturePoint[4],Point* Centre,SDL_Point* center,
                    bool* toucheMur,bool* toucheVoiture,bool* afficherValid,int* ValidationPos,bool* commencementValid,bool* ColiderInitialisation,
                    Point ValidRectPoint[4],Point VoitureSec[12],SDL_Texture* textureAngle[30],bool* mauvaisChoix,SDL_Texture* textureTuto[30],
                    bool* tutoBool){
    static int ParkingEnCour=4;
    FondBase(window_width,window_height,textureCreBa);
    DecorBatailleCreneau(image,renderer,window_width,window_height,ObjetCreneau,textureCreBa,Parkingg);
    Ecrire(window_width,window_height,renderer,fontMoyen,0.015,0.346,"Simulation creneau et bataille",0.3);
    //Ecrire(window_width,window_height,renderer,fontMoyen,0.09,0.03,"Simulation crenaux",0.12);
    //Ecrire(window_width,window_height,renderer,fontMoyen,0.09,0.52,"Simulation bataille",0.12);
    Parking(textureParking,image,renderer,window_width,window_height,ObjetCreneau,CreneauON,*speedx,*tournerRad,*speedy,VoiturePoint,Centre,center,tournerDeg,Parkingg);
    Play(textureBoutonDiver,image,renderer,ObjetCreneau,CreneauON,BatailleON,window_width,window_height,Parkingg,
         0,1);
    ChargementVoiture(textureVoiture,image,renderer,ObjetCreneau,window_width,window_height,BatailleON,Voiture,Parkingg);
    Quitter(window_width,window_height,textureBoutonDiver,ObjetCreneau);

    if(*CreneauON){
        ParkingEnCour=0;// dernier utiliser CreneauON
    }
    if(*BatailleON){
        ParkingEnCour=1;// dernier utiliser BatailleON
    }
    if(*CreneauON){
        if(wait(30,waitboolPlay,waitComptePlay,delaTime,false)){
            BoutonBougerr(textureCommande,image, renderer,BoutonBouger,ObjetCreneau,window_width,window_height,*LevierPos,VolantRad,*VolantPos,*freinAmainPos,CreneauON,BatailleON,delaTime,Parkingg,
                          0,1.1,0.3);
            Mouvement(speedx,speedy,BoutonGauchePresser,mouseX,mouseY,BoutonBouger,delaTime,tournerRad,LevierPos,VolantPos,freinAmainPos,VoiturePoint,Centre,ObjetCreneau,image,renderer,window_width,window_height,center,tournerDeg);
            VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                              0,0.55,0.78);
            ColiderVoiture(speedx, speedy, tournerRad, LevierPos, VolantPos, freinAmainPos, VoiturePoint, Centre, ObjetCreneau, window_width, window_height, CreneauON, center,mouseX,mouseY,BoutonGauchePresser,BoutonBouger,ColiderInitialisation,Parkingg,
                           0);
            DetectionCollision(BoutonBouger,ObjetCreneau,toucheMur,toucheVoiture,VoiturePoint,CreneauON,Voiture,Parkingg,
                               0);
            if(*tutoBool){
                tutoriel(BoutonBouger,window_width,window_height,textureTuto);
                tutorielCreneau(BoutonBouger,window_width,window_height,textureTuto);
            }
        }
    }
    else if(ParkingEnCour==1){
        Validation(textureValidation,image,renderer,window_width,window_height,ObjetCreneau,delaTime,afficherValid,freinAmainPos,LevierPos,ValidationPos,VoiturePoint,BatailleON,toucheMur,toucheVoiture,commencementValid,Parkingg,ValidRectPoint,textureAngle,BoutonBouger,mauvaisChoix,
                   BatailleON,1,0.525,0.52345,0.1195,0);
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,0,0,0,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          0,0.55,0.78);
    }
    else{
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          0,0.55,0.78);
    }
    if(*BatailleON){
        if(wait(30,waitboolPlay,waitComptePlay,delaTime,false)){
            BoutonBougerr(textureCommande,image, renderer,BoutonBouger,ObjetCreneau,window_width,window_height,*LevierPos,VolantRad,*VolantPos,*freinAmainPos,CreneauON,BatailleON,delaTime,Parkingg,
                          1,1.1,0.3);
            Mouvement(speedx,speedy,BoutonGauchePresser,mouseX,mouseY,BoutonBouger,delaTime,tournerRad,LevierPos,VolantPos,freinAmainPos,VoiturePoint,Centre,ObjetCreneau,image,renderer,window_width,window_height,center,tournerDeg);
            VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                              1,0.34,0.78);
            ColiderVoiture(speedx, speedy, tournerRad, LevierPos, VolantPos, freinAmainPos, VoiturePoint, Centre, ObjetCreneau, window_width, window_height, CreneauON, center,mouseX,mouseY,BoutonGauchePresser,BoutonBouger,ColiderInitialisation,Parkingg,
                           1);
            DetectionCollision(BoutonBouger,ObjetCreneau,toucheMur,toucheVoiture,VoiturePoint,CreneauON,Voiture,Parkingg,
                               1);
            if(*tutoBool){
                tutoriel(BoutonBouger,window_width,window_height,textureTuto);
                tutorielBataille(BoutonBouger,window_width,window_height,textureTuto);
            }
        }           
    }
    else if(ParkingEnCour==0){
        Validation(textureValidation,image,renderer,window_width,window_height,ObjetCreneau,delaTime,afficherValid,freinAmainPos,LevierPos,ValidationPos,VoiturePoint,BatailleON,toucheMur,toucheVoiture,commencementValid,Parkingg,ValidRectPoint,textureAngle,BoutonBouger,mauvaisChoix,
                   CreneauON,0,0.75,0.499,0.063,0);
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,0,0,0,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          1,0.34,0.78);
    }
    else{
        VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                          1,0.34,0.78);                
    }
}

void Intersection(SDL_Renderer* renderer,SDL_Surface* image,int window_width,int window_height,SDL_Texture* textureBoutonDiver[30],
                  SDL_Texture* textureCreBa[30],SDL_Texture* textureParking[30],SDL_Texture* textureCommande[30],SDL_Texture* textureVoiture[30], 
                  SDL_Rect ObjetCreneau[30],TTF_Font* fontMoyen,SDL_Rect Parkingg[30],bool*CreneauON,bool*BatailleON,
                  SDL_Rect Voiture[15],Uint32 delaTime,bool* waitboolPlay,int* waitComptePlay,SDL_Rect BoutonBouger[10],int* VolantPos,int* freinAmainPos,
                  int* LevierPos,double* speedx,double* speedy,int* mouseX,int* mouseY, float* tournerRad, float* tournerDeg, float* VolantRad,
                  SDL_Texture* textureValidation[30],bool* BoutonGauchePresser,Point VoiturePoint[4],Point* Centre,SDL_Point* center,
                  bool* toucheMur,bool* toucheVoiture,bool* afficherValid,int* ValidationPos,bool* commencementValid,bool* ColiderInitialisation,
                  Point ValidRectPoint[4],Point VoitureSec[12],SDL_Texture* textureAngle[30],bool* mauvaisChoix,int nbScenario,bool*PresIntersection,
                  SDL_Texture* textureGens[30],SDL_Rect GensRect[30],bool HommeAffBouge[4],bool Gensfin[4],bool VoitureBouge[3],bool Voiturefin[3],
                  bool VoitureAppuit[3],bool GensAppuit[4],int speedPers[4],int speedVoitX[3],int speedVoitY[3],SDL_Texture* textureFeu[30],
                  float tournerVoit[3],SDL_Texture* textureTuto[30],bool* tutoBool){

    Quitter(window_width,window_height,textureBoutonDiver,ObjetCreneau);
    FondBase(window_width,window_height,textureCreBa);
    Ecrire(window_width,window_height,renderer,fontMoyen,0.015,0.346,"Simulation en intersection",0.3);
    ParkingIntersection(textureParking,renderer,window_width,window_height,Parkingg);
    Play(textureBoutonDiver,image,renderer,ObjetCreneau,CreneauON,BatailleON,window_width,window_height,Parkingg,
         6,28);
    DecorIntersection(renderer,window_width,window_height,Parkingg,textureCreBa);
    Mouvement(speedx,speedy,BoutonGauchePresser,mouseX,mouseY,BoutonBouger,delaTime,tournerRad,LevierPos,VolantPos,freinAmainPos,VoiturePoint,Centre,ObjetCreneau,image,renderer,window_width,window_height,center,tournerDeg);
    VoiturePrincipale(textureVoiture,image,renderer,window_width,window_height,ObjetCreneau,*speedx,*speedy,*tournerRad,tournerDeg,Centre,center,VoiturePoint,Parkingg,
                      6,0.535,0.79);
    if(*PresIntersection){
        PresentationIntersection(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
    }
    if((nbScenario==3 || nbScenario==6) && *PresIntersection==false){
        Validation(textureValidation,image,renderer,window_width,window_height,ObjetCreneau,delaTime,afficherValid,freinAmainPos,LevierPos,ValidationPos,VoiturePoint,BatailleON,toucheMur,toucheVoiture,commencementValid,Parkingg,ValidRectPoint,textureAngle,BoutonBouger,mauvaisChoix,
                   CreneauON,6,0.795,0.525,0.13,0);
    }
            
    if((nbScenario==2 || nbScenario==5 || nbScenario==8) && *PresIntersection==false){
        Validation(textureValidation,image,renderer,window_width,window_height,ObjetCreneau,delaTime,afficherValid,freinAmainPos,LevierPos,ValidationPos,VoiturePoint,BatailleON,toucheMur,toucheVoiture,commencementValid,Parkingg,ValidRectPoint,textureAngle,BoutonBouger,mauvaisChoix,
                   CreneauON,6,0,0.325,0.13,0);
    }

    if((nbScenario==1 || nbScenario==4 || nbScenario==7) && *PresIntersection==false){
        Validation(textureValidation,image,renderer,window_width,window_height,ObjetCreneau,delaTime,afficherValid,freinAmainPos,LevierPos,ValidationPos,VoiturePoint,BatailleON,toucheMur,toucheVoiture,commencementValid,Parkingg,ValidRectPoint,textureAngle,BoutonBouger,mauvaisChoix,
                   CreneauON,6,0.518,0,0.068,1);
    }

    if(*CreneauON){
        if(wait(30,waitboolPlay,waitComptePlay,delaTime,false)){
            BoutonBougerr(textureCommande,image, renderer,BoutonBouger,ObjetCreneau,window_width,window_height,*LevierPos,VolantRad,*VolantPos,*freinAmainPos,CreneauON,BatailleON,delaTime,Parkingg,
                          6,1.1,0.3);
                
            ColiderVoiture(speedx, speedy, tournerRad, LevierPos, VolantPos, freinAmainPos, VoiturePoint, Centre, ObjetCreneau, window_width, window_height, CreneauON, center,mouseX,mouseY,BoutonGauchePresser,BoutonBouger,ColiderInitialisation,Parkingg,
                           6);
            DetectionCollision(BoutonBouger,ObjetCreneau,toucheMur,toucheVoiture,VoiturePoint,CreneauON,Voiture,Parkingg,
                               6);
            DetectionColisionRebord(VoiturePoint,toucheMur,CreneauON,Parkingg);

            if(nbScenario==1){
                scenario1(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit);
            }
            if(nbScenario==2){
                scenario2(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit);
            }
            if(nbScenario==3){
                scenario3(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
            }
            if(nbScenario==4){
                scenario4(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
            }
            if(nbScenario==5){
                scenario5(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
            }
            if(nbScenario==6){
                scenario6(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
            }
            if(nbScenario==7){
                scenario7(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
            }
            if(nbScenario==8){
                scenario8(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
            }
            if(*tutoBool){
                tutoriel(BoutonBouger,window_width,window_height,textureTuto);
                tutoInteraction(BoutonBouger,window_width,window_height,textureTuto);
            }
        }
    }else{
        if(nbScenario==8 && *PresIntersection==false){
            scenarioDesign8(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
        }
        if(nbScenario==7 && *PresIntersection==false){
            scenarioDesign7(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
        }
        if(nbScenario==6 && *PresIntersection==false){
            scenarioDesign6(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
        }
        if(nbScenario==5 && *PresIntersection==false){
            scenarioDesign5(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
        }
        if(nbScenario==4 && *PresIntersection==false){
            scenarioDesign4(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
        }
        if(nbScenario==3 && *PresIntersection==false){
            scenarioDesign3(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit,GensAppuit);
        }
        if(nbScenario==2 && *PresIntersection==false){
            scenarioDesign2(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit);
        }
        if(nbScenario==1 && *PresIntersection==false){
            scenarioDesign1(renderer,window_width,window_height,Parkingg,textureGens,GensRect,HommeAffBouge,delaTime,speedPers,Gensfin,Voiture,textureVoiture,Voiturefin,speedVoitX,speedVoitY,VoitureBouge,tournerVoit,textureFeu,VoiturePoint,CreneauON,mauvaisChoix,VoitureAppuit);
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Ma fenêtre SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 3000, 3000, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface* image;

    SDL_Event event;
    EtatLogiciel Etat = MENU_PRINCIPAL;

    SDL_Texture* textureCreBa[30];
    SDL_Texture* textureCommande[30];
    SDL_Texture* textureVoiture[30];
    SDL_Texture* textureMenue[30];
    SDL_Texture* textureValidation[30];
    SDL_Texture* textureBoutonDiver[30];
    SDL_Texture* textureParking[30];
    SDL_Texture* textureGens[30];
    SDL_Texture* textureFeu[30];
    SDL_Texture* textureAngle[30];
    SDL_Texture* textureTuto[30];
    DimImage tabDimImage[30];

    TTF_Font* fontGrand = TTF_OpenFont("police/RockoFLF.ttf", 60);
    TTF_Font* fontMoyen = TTF_OpenFont("police/RockoFLF.ttf", 70);

    SDL_Rect BoutonMenue[10];
    SDL_Rect ObjetCreneau[30];
    SDL_Rect ObjetEpi[30];
    SDL_Rect BoutonBouger[10];
    SDL_Rect Voiture[15];
    SDL_Rect Parkingg[30];
    SDL_Rect Trieur;
    SDL_Rect GensRect[30]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//IMPORTANT

    Point VoiturePoint[4];
    Point VoitureSec[12];
    Point ValidRectPoint[4];
    Point Centre;
    Point Centre2;
    Point haut;

    SDL_Point center;

    double speedx = 0, speedy = 0;
    float tournerRad=0;//-1.5707963267948966
    float tournerDeg=0;
    float VolantRad=0;
    float tournerVoit[3] = {0,0,0};

    int NombreBoutonMenue=5;
    int window_width, window_height;
    int mouseY,mouseX;
    int LevierPos=2;
    int VolantPos=1;
    int freinAmainPos=0;
    int waitComptePlay=0;
    int ValidationPos=0;
    int speedPers[4] = {0,0,0,0};
    int speedVoitX[3] = {0,0,0};
    int speedVoitY[3] = {0,0,0};
    int nbScenario=8;

    Uint32 lastTime = SDL_GetTicks();
    Uint32 delaTime;

    bool EnCour=true;
    bool BoutonGauchePresser=false;
    bool CreneauON=false;
    bool BatailleON=false;
    bool waitboolPlay=false;
    bool afficherValid=false;
    bool commencementValid=false;
    bool ColiderInitialisation=true;
    bool toucheMur=false;
    bool mauvaisChoix=false;
    bool toucheVoiture=false;
    bool HommeAffBouge[4]={false, false, false, false};
    bool Gensfin[4]={false, false, false, false};
    bool VoitureBouge[3]={false, false, false};
    bool Voiturefin[3]={false, false, false};
    bool VoitureAppuit[3]={false, false, false};
    bool GensAppuit[4]={false, false, false,false};
    bool PresIntersection=false;
    bool tutoBool=false;

    TextureCreBa(image,renderer,textureCreBa,tabDimImage);
    TextureCommande(image,renderer,textureCommande);
    TextureVoiture(image,renderer,textureVoiture);
    TextureMenue(image,renderer,textureMenue);
    TextureValidation(image,renderer,textureValidation);
    TextureBoutonDiver(image,renderer,textureBoutonDiver);
    TextureParking(image,renderer,textureParking);
    TextureGens(image,renderer,textureGens);
    TextureFeu(image,renderer,textureFeu);
    TextureAngle(image,renderer,textureAngle);
    TextureTuto(image,renderer,textureTuto);

    while (EnCour) {
        SDL_RenderClear(renderer);
        SDL_GetWindowSize(window, &window_width, &window_height);

        Uint32 currentTime = SDL_GetTicks();
        delaTime = (currentTime - lastTime) / 10;
        lastTime = currentTime;

        srand(time(0));

        while(SDL_PollEvent(&event)){
            GestionCommandes(event,&EnCour,&mouseX,&mouseY,BoutonMenue,ObjetCreneau,&Etat,BoutonBouger,&CreneauON,
                             &speedx,&speedy,&tournerRad,&LevierPos,&freinAmainPos,&VolantPos,&BoutonGauchePresser,
                             &afficherValid,&ValidationPos,&ColiderInitialisation,&toucheMur,&toucheVoiture,&BatailleON,
                             &commencementValid,&waitComptePlay,&waitboolPlay,HommeAffBouge,GensRect,speedPers,Gensfin,
                             VoitureBouge,Voiture,speedVoitX,Voiturefin,tournerVoit,speedVoitY,&mauvaisChoix,VoitureAppuit,
                             GensAppuit,&nbScenario,&PresIntersection,&tutoBool);
            GestionBoutonBougerr(event,mouseX,mouseY,BoutonBouger,&freinAmainPos,&LevierPos,&VolantPos,&CreneauON,&BatailleON,textureTuto,window_width,window_height,&tutoBool);
        }

        static int ParkingEnCour=4;

        switch(Etat){
        case MENU_PRINCIPAL:
            ArrierePlanMenue(textureMenue,image,renderer,window_width,window_height);
            EcrireMenu(window,renderer,fontGrand,0.97,0.1,"Creneau/Bataille",BoutonMenue,0);
            EcrireMenu(window,renderer,fontGrand,0.98,0.36,"Demi Tour/Epi",BoutonMenue,3);
            EcrireMenu(window,renderer,fontGrand,0.94,0.63,"Intersection",BoutonMenue,1);
            EcrireMenu(window,renderer,fontGrand,0.84,0.9,"Quitter",BoutonMenue,2);
            SoulignerMenue(image,event,&mouseX,&mouseY,BoutonMenue,renderer,NombreBoutonMenue);
            break;
        case CRENEAU_BATAILLE:
            CreneauBataile(renderer,image,window_width,window_height,textureBoutonDiver,textureCreBa,textureParking,textureCommande,
                           textureVoiture,ObjetCreneau,fontMoyen,Parkingg,&CreneauON,&BatailleON,Voiture,delaTime,&waitboolPlay,
                           &waitComptePlay,BoutonBouger,&VolantPos,&freinAmainPos,&LevierPos,&speedx,&speedy,&mouseX,&mouseY,&tournerRad,
                           &tournerDeg,&VolantRad,textureValidation,&BoutonGauchePresser,VoiturePoint,&Centre,&center,&toucheMur,&toucheVoiture,
                           &afficherValid,&ValidationPos,&commencementValid,&ColiderInitialisation,ValidRectPoint,VoitureSec,textureAngle,&mauvaisChoix
                           ,textureTuto,&tutoBool);
            break;
        case DEMITOUR_EPI:
            DemiTourEpi(renderer,image,window_width,window_height,textureBoutonDiver,textureCreBa,textureParking,textureCommande,
                        textureVoiture,ObjetCreneau,fontMoyen,Parkingg,&CreneauON,&BatailleON,Voiture,delaTime,&waitboolPlay,
                        &waitComptePlay,BoutonBouger,&VolantPos,&freinAmainPos,&LevierPos,&speedx,&speedy,&mouseX,&mouseY,&tournerRad,
                        &tournerDeg,&VolantRad,textureValidation,&BoutonGauchePresser,VoiturePoint,&Centre,&center,&toucheMur,&toucheVoiture,
                        &afficherValid,&ValidationPos,&commencementValid,&ColiderInitialisation,ValidRectPoint,VoitureSec,textureAngle,&mauvaisChoix
                        ,textureTuto,&tutoBool);
        break;
        case ROND_POINT:
            Quitter(window_width,window_height,textureBoutonDiver,ObjetCreneau);
            FondBase(window_width,window_height,textureCreBa);
            Ecrire(window_width,window_height,renderer,fontMoyen,0.015,0.346,"Simulation en rond point",0.3);
            break;
        case INTERSECTION:
            Intersection(renderer,image,window_width,window_height,textureBoutonDiver,textureCreBa,textureParking,textureCommande,
                         textureVoiture,ObjetCreneau,fontMoyen,Parkingg,&CreneauON,&BatailleON,Voiture,delaTime,&waitboolPlay,
                         &waitComptePlay,BoutonBouger,&VolantPos,&freinAmainPos,&LevierPos,&speedx,&speedy,&mouseX,&mouseY,&tournerRad,
                         &tournerDeg,&VolantRad,textureValidation,&BoutonGauchePresser,VoiturePoint,&Centre,&center,&toucheMur,&toucheVoiture,
                         &afficherValid,&ValidationPos,&commencementValid,&ColiderInitialisation,ValidRectPoint,VoitureSec,textureAngle,&mauvaisChoix,
                         nbScenario,&PresIntersection,textureGens,GensRect,HommeAffBouge,Gensfin,VoitureBouge,Voiturefin,VoitureAppuit,
                         GensAppuit,speedPers,speedVoitX,speedVoitY,textureFeu,tournerVoit,textureTuto,&tutoBool);
            break;

        }
        afficherImage2(renderer);
        ImageNb2=0;
        AfficherColiderVoiture(image,renderer,VoiturePoint,Centre2);
        //AfficherColiderSec(VoitureSec,renderer);
        AfficherValidRect(ValidRectPoint,renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    cleanUp2();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}