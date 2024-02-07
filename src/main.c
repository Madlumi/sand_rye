#include "SDL_mouse.h"
#include <math.h>
#include "SDL_rect.h"
#include "SDL_stdinc.h"
#include <SDL.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ucontext.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include "Colours.c"

#define IN(x,l,h) ((l)<=(x)&&(x)<=(h))
#include<unistd.h>
#define PI 3.14159265

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;
#define w 512
#define h 512
int running=1;
int t = 0;
#define keyn 512
#define mkeyn 24
bool KEYS[keyn];
SDL_Point mpos = {0,0};
int MKEYS[mkeyn];
int timesteps=1;
int cellN=w*h;




typedef struct{double x; double y;}force;
typedef struct{
   double v;
   double r;
   double d;
   double el;
   double x;
   double y;
   double mass;
   force f;
   int colour;
} Cell;


double get_angle(double F[2]) {
    double angle = atan2(F[1], F[0]) * 180 / PI; // Convert radians to degrees
    // Ensure the angle is between 0 and 360 degrees
    if (angle < 0) {
        angle += 360;
    }
    return angle;
}

Cell *cells[h*w];

Cell *newCell(int col,double x, double y){
   Cell *c =malloc(sizeof(Cell));
   c->x=x;
   c->y=y;
   c->v=0;
   c->d=0;
   c->el=1;
   c->r=2;
   c->mass=.01;

   c->colour=col;
   return c;
}
double G=9.81;
void render();

void quit(){ SDL_Quit(); printf("quiting\n"); running=0; }

void events(){
   SDL_Event e;
   SDL_GetMouseState(&mpos.x, &mpos.y);
   for(int i = 0; i < mkeyn; i++){if(MKEYS[i]>1){MKEYS[i]--;}}
   while(SDL_PollEvent(&e)) {
      if (e.type==SDL_KEYDOWN){if(!IN(e.key.keysym.sym,0,keyn-1)){printf("key: %d\n",e.key.keysym.sym);return;}KEYS[e.key.keysym.sym] = 1;}
      else if (e.type == SDL_KEYUP){if(!IN(e.key.keysym.sym,0,keyn-1)){return;}KEYS[e.key.keysym.sym] = 0;}
      else if (e.type == SDL_MOUSEBUTTONDOWN){if(!IN(e.button.button,0,mkeyn-1)){printf("key: %d\n",e.button.button);return;}MKEYS[e.button.button]=2;}
      else if (e.type == SDL_MOUSEBUTTONUP){if(!IN(e.button.button,0,mkeyn-1)){printf("key: %d\n",e.button.button);return;}MKEYS[e.button.button]=0;}
      else if (e.type == SDL_QUIT){quit();}
   }
   if(KEYS[SDLK_q]) {
      quit();  
   }
}

   int linedir=-1;
int mcol=0;
void paint(){
   for(int i = 0; i < cellN; i++) {
      if(cells[i]->colour==0){
         cells[i]->x=mpos.x;
         cells[i]->y=mpos.y;
         cells[i]->r=rand()%10+1;
         cells[i]->colour=hsv_to_int(HsvToRgb(mcol,255,255));
         break;
      }
   }
}
Cell *ROOM;
void grav(Cell *c){c->f.y+=c->mass*G;}

void collide(Cell *c,Cell *hit){
   double bounce = c->el* hit->el;
   double mass = c->mass+hit->mass;
   double yy =(c->f.y-hit->f.y)*bounce;
   c->f.y=-yy;
   hit->f.y=yy;
}
void move(Cell *c){
   ROOM->f.y=0;
   c->y+=c->f.y;
   if(c->y>=h | c->y<=0){
      collide(c,ROOM);
   }
}
void tick(){
   for(int i = 0; i < cellN; i++) {
      if(cells[i]->colour!=0){
         grav(cells[i]);
         move(cells[i]);
      }
   }
   mcol=(mcol+10)%255;
   t+=1;
   if(MKEYS[1] || MKEYS[3]){paint();}
}
void setPix(Uint8 *pix, int x, int y, int c){
        pix[(x + y * w) * 4 + 0] = (c & 0xFF);         // Set blue component
        pix[(x + y * w) * 4 + 1] = (c >> 8) & 0xFF;  // Set green component
        pix[(x + y * w) * 4 + 2] = (c >> 16) & 0xFF; // Set red component
        pix[(x + y * w) * 4 + 3] = (c >> 24) & 0xFF; // Set alpha component
}
void renderCell(Uint8 * pix, Cell * c){
   if(c->colour==0){return;}
   //draw a circle with:
   for (int i = c->x - c->r; i <= c->x + c->r; i++) {
        for (int j = c->y - c->r; j <= c->y + c->r; j++) {
            if ((i - c->x) * (i - c->x) + (j - c->y) * (j - c->y) <= c->r* c->r) {
                if (i >= 0 && i < w && j >= 0 && j < h) {
               setPix(pix ,i, j, c->colour );
                }
            }
        }
    }


}
void render(){
   if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
   Uint8 * pixels = surface->pixels;

   for (int i = 0; i<w*h*4;i++) {
      pixels[i]=0;
   }
   for(int i = 0; i < cellN; i++) {
      renderCell(pixels, cells[i]);
   }
   if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
   
   SDL_Texture *screenTexture = SDL_CreateTextureFromSurface(renderer, surface);

   SDL_RenderClear(renderer);
   SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
   SDL_RenderPresent(renderer);
   SDL_DestroyTexture(screenTexture);
}
void mainLoop(){
   for(int i = 0; i < timesteps; i++){
      events();
      tick();
   }
   render();
}
int main(int argc, char* argv[]) {
   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
   surface = SDL_CreateRGBSurface(0, w , h, 32, 0, 0, 0, 0);

      ROOM= newCell(0, 255,255);
   ROOM->el=1;
   
   for(int i = 0; i < cellN; i++) {
      cells[i]= newCell(0, 255,255);
   }
   for(int i = 0; i < 322; i++) { // init them all to false
      KEYS[i] = false;
   }
#ifdef __EMSCRIPTEN__
   emscripten_set_main_loop(mainLoop, 0, 1);
#else
   while(running) {        
      mainLoop();
      SDL_Delay(8);
   }
#endif 
}
