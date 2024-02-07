#include "renderer.h"
#include "keys.c"
#include "tick.c"
#include <stdlib.h>
#include <sys/types.h>
#include "mutil.h"


typedef struct{ I colour; F mass;  F dy; I m; I sit;}Cell;
Cell sand[w*h];
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
      eif(sand[CN].dy>=sand[C].dy){sand[C].dy= sand[CN].dy;}
      eif(true){sand[C].dy*=.8; }
   })
}

V sandTick(){
  if(MKEYS[1] || MKEYS[3]){paint();}
  t++;
  I falldir = t % 2; if(falldir==0){falldir=-1;}
  I xx;
   FOR(w*h, { sand[i].sit=0; }) 

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
   I radius = 20;
   I centerX = mpos.x; I centerY = mpos.y;
   for (int y = centerY - radius; y <= centerY + radius; y++) {
      for (int x = centerX - radius; x <= centerX + radius; x++) {
         int distanceSquared = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);
         if (distanceSquared <= radius * radius) {
            if (x >= 0 && x < w&& y >= 0 && y < h) {
               if(MKEYS[1]>0 && sand[gCellN(x,y)]empt ){ sand[gCellN(x,y)] = newCell(hsv_to_int(HsvToRgb(mcol,255,255)), .1, 0,0); }
               if(MKEYS[3]>0){ sand[gCellN(x,y)].colour = 0; }
            }
         }
      }
   }
}

void sandRender(Uint8 *p){
   FORYX(h,w,{
      p[(x + y * w) * 4 + 0] = (sand[gCellN(x,y)].colour) & 0xFF;         // Set blue component
      p[(x + y * w) * 4 + 1] = (sand[gCellN(x,y)].colour >> 8) & 0xFF;  // Set green component
      p[(x + y * w) * 4 + 2] = (sand[gCellN(x,y)].colour >> 16) & 0xFF; // Set red component
      p[(x + y * w) * 4 + 3] = (sand[gCellN(x,y)].colour >> 24) & 0xFF; // Set alpha component
   });
}

void sandInit(){
   Wall = newCell(1, 100, 0, 0);
   addRenderFunction(sandRender);
   addTickFunction(sandTick);
   FORYX(h,w,{ sand[gCellN(x, y)]=newCell( (rand()%30<.01*y-1)? hsv_to_int(HsvToRgb(rand()%255,255,255)): 0 , 1 , 0, 0); });
}
