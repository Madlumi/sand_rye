#include "renderer.h"
#include "keys.c"
#include "tick.c"
#include <stdlib.h>
#include <sys/types.h>
#include "mutil.h"


typedef struct{ I colour; D mass; D dx; D dy; I m;}Cell;
Cell *sand[w*h];
Cell *newCell(I col, D mass, D dx, D dy){ 

   Cell *c =malloc(sizeof(Cell));
   c->m=0;
   c->colour=col;
   c->mass =mass;
   c->dx =dx;
   c->dy =dy;
   return c; 
}
Cell* Wall;
I gCellN(I x, I y){ 
   R x+y*w;
}
Cell *gCell(I n){
   //if(x<0 || y<0 ||y>=h || x>=w){ R Wall; }
   return sand[n];
}

I empty(Cell *c){if(c->colour==0){return  1;} return  0; }
 
V paint();

V swapCell(I a, I b){
   Cell *tmp = sand[a];
    sand[a] = sand[b];
    sand[b] = tmp;
}
CON D SPEED = 1.0/60;
CON D G = 9.81; I t = 0; I mcol=0;
I sign(int x) { return (x > 0) - (x < 0); }

#define empt ->colour==0
#define nempt ->colour!=0
V cellTick(I c, I falldir){ 


   sand[c]->dy+=G*SPEED;
   int vel =(int)sand[c]->dy;
   I d = sign(sand[c]->dy);
   
   vel *= d;
   FOR(vel-1,{
      if(c+(d*(i+1))*w>=w*h){break;}
      if(sand[c+(d*(i+1))*w]empt){
         swapCell(c+(d*(i+1))*w,c+(d*i)*w);
      }eif(sand[falldir+c+(d*(i+1))*w]empt){
         swapCell(falldir+ c+(d*(i+1))*w,c+(d*i)*w);
      }eif(sand[-falldir+c+(d*(i+1))*w]empt){
         swapCell(-falldir+ c+(d*(i+1))*w,c+(d*i)*w);
      }el{
        // sand[c+(d*(i+1))*w]->dy+=sand[c+(d*i)*w]->dy;
         sand[c+(d*i)*w]->dy=sand[c+(d*i+1)*w]->dy;
         break;
      }
   });
}

V sandTick(){
  if(MKEYS[1] || MKEYS[3]){paint();}
  t++;
  I falldir = t % 2; if(falldir==0){falldir=-1;}
  I x;
   
   FORR(w*(h-1), {
         if(sand[i]nempt){ cellTick(i,1); }
   });
}

V paint(){
   mcol=(mcol+2) % 255;
   I radius = 20;
   I centerX = mpos.x; I centerY = mpos.y;
   for (int y = centerY - radius; y <= centerY + radius; y++) {
      for (int x = centerX - radius; x <= centerX + radius; x++) {
         int distanceSquared = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);
         if (distanceSquared <= radius * radius) {
            if (x >= 0 && x < w&& y >= 0 && y < h) {
               if(MKEYS[1]>0){ sand[gCellN(x,y)]->colour = hsv_to_int(HsvToRgb(mcol,255,255)); }
               if(MKEYS[3]>0){ sand[gCellN(x,y)]->colour = 0; }
            }
         }
      }
   }
}

void sandRender(Uint8 *p){
   FORYX(h,w,{
//      p[(x + y * w) * 4 + 0] = (int)(sand[gCellN(x,y)]->dy*10) ;         // Set blue component
//      p[(x + y * w) * 4 + 1] = (int)(sand[gCellN(x,y)]->dy*10 );  // Set green component
//      p[(x + y * w) * 4 + 2] = (int)(sand[gCellN(x,y)]->dy*10 ); // Set red component
//      p[(x + y * w) * 4 + 3] = (sand[gCellN(x,y)]->colour >> 24) & 0xFF; // Set alpha component
      p[(x + y * w) * 4 + 0] = (sand[gCellN(x,y)]->colour) & 0xFF;         // Set blue component
      p[(x + y * w) * 4 + 1] = (sand[gCellN(x,y)]->colour >> 8) & 0xFF;  // Set green component
      p[(x + y * w) * 4 + 2] = (sand[gCellN(x,y)]->colour >> 16) & 0xFF; // Set red component
      p[(x + y * w) * 4 + 3] = (sand[gCellN(x,y)]->colour >> 24) & 0xFF; // Set alpha component
   });
}

void sandInit(){
   Wall = newCell(1, 100, 0, 0);
   addRenderFunction(sandRender);
   addTickFunction(sandTick);
   FORYX(h,w,{ sand[gCellN(x, y)]=newCell( (rand()%30<.01*y-1)? hsv_to_int(HsvToRgb(rand()%255,255,255)): 0 , 1 , 0, 0); });
}
