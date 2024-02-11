#include "SDL_stdinc.h"
#include "renderer.h"
#include "keys.c"
#include "tick.c"
#include <stdlib.h>
#include <sys/types.h>
#include "mutil.h"


typedef struct{ Uint32 colour; F mass;  F dy; I m; I sit;}Cell;
Cell *sand;
Cell newCell(I col, F mass, F dx, F dy){ 
   Cell c;
   c.m=0;
   c.colour=col;
   c.mass =mass;
   c.dy =dy;
   return c; 
}
Cell Wall;
I gCellN(I x, I y){ 
   R x+y*w;
}

I empty(Cell *c){if(c->colour==0){return  1;} return  0; }

V paint();

V swapCell(I a, I b){
   Cell tmp = sand[a];
   sand[a] = sand[b];
   sand[b] = tmp;
}
F SPEED = 1.0/60.0;
F G = 9.81; I t = 0; I mcol=0;
I sign(int x) { return (x > 0) - (x < 0); }

   I radius = 20;
#define empt .colour==0
#define nempt .colour!=0
V cellTick(I c, I falldir){ 
   if(sand[c].sit)return;
   sand[c].dy+=G*SPEED;
   I v = (int)sand[c].dy;
   I sig=sign(v);
   v*=sig;
   sand[c].sit=1;
   F i=0;
   FOR(v,{
      int C =c+(w*i*sig);
      int CN=c+(w*(i+1)*sig);

      if(!IN(CN, 1, w*h-1)){ sand[C].dy = 0;break; }
      if(sand[CN]empt){ swapCell(C, CN); }
      eif(sand[CN+falldir]empt ){ swapCell(C, CN+falldir);c+=falldir; }
      eif(sand[CN-falldir]empt ){ swapCell(C, CN-falldir);c-=falldir; }
      eif(sand[CN].dy+G*SPEED >=sand[C].dy){sand[C].dy= sand[CN].dy;}
      eif(true){sand[C].dy*=.8; }
   })
}

I decay=0;
V sandTick(){
   if(KEYS[SDLK_1]==2)decay=!decay;
   radius+=mouseWheelMoved*2; mouseWheelMoved=0;
   if(radius<2)radius=2;
   if(radius>200)radius=200;

   if(MKEYS[1] || MKEYS[3]){paint();}
   t++;
   I falldir = t % 2; if(falldir==0){falldir=-1;}
   I xx;
   FOR(w*h, { sand[i].sit=0; }) 

   if(decay){ FOR(h*w,{ if(rand()%1000==1)sand[i].colour=0; }) }
   FOR_RYX(h, w,  {
      xx = t%2 ?  x : w-(x) ;

      int i = xx+y*w;
      if(sand[i]nempt){ cellTick(i,1); }
   } )
   // FORR(w*(h-1), {
   //       if(sand[i]nempt){ cellTick(i,1); }
   // });
}

V paint(){
   mcol=(mcol+1) % 255;
   I centerX = mpos.x; I centerY = mpos.y;
   for (int y = centerY - radius; y <= centerY + radius; y++) {
      for (int x = centerX - radius; x <= centerX + radius; x++) {
         int distanceSquared = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);
         if (distanceSquared <= radius * radius) {
            if (x >= 0 && x < w&& y >= 0 && y < h) {
               if(MKEYS[1]>0 && sand[gCellN(x,y)]empt ){ sand[gCellN(x,y)] = newCell(hsv_to_int(HsvToRgb(mcol,255,255)), .1, 0,2); }
               if(MKEYS[3]>0){ sand[gCellN(x,y)].colour = 0; }
            }
         }
      }
   }
}

void sandRender(Uint32 *p){ FORYX(h,w,{ p[(x + y * w)] = (sand[gCellN(x,y)].colour);   }); }

V loadImg(char *img){
   SDL_Surface* imgSurface = IMG_Load(img);
   if (!imgSurface) {
      fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
      return;
   }
   imgSurfScaled= SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
   SDL_BlitScaled(imgSurface, NULL, imgSurfScaled, NULL);

   Uint32 * imgPixels = (Uint32 *)imgSurfScaled->pixels;
   int imgPitch = imgSurfScaled->pitch / sizeof(Uint32);
   

   SDL_FreeSurface(imgSurfScaled);
   SDL_FreeSurface(imgSurface);

   FORYX(h,w,{ sand[gCellN(x, y)]=newCell( imgPixels[x+y* imgPitch] , 1 , 0, 0); });
}

V sandInit(){
   sand = malloc(sizeof(Cell)*w*h);
   Wall = newCell(1, 100, 0, 0);
   addRenderFunction(sandRender);
   addTickFunction(sandTick);
   FORYX(h,w,{ sand[gCellN(x, y)]=newCell( 0  , 1 , 0, 0); });
}

V sandFree(){
   free(sand);
}
