#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define maxfil 20
#define maxcol  29
#define inf 999999
#define num 98989
#define nodos 240
#define maxarreglo 240
#define maxi 4

bool estadoTablero=true;

float speed;
float speedClyde;
float speedPacman;
float speedFants;

char TipoSemilla[12];
int CantiSemilla;

ALLEGRO_BITMAP *bmp;
ALLEGRO_BITMAP *roca;
ALLEGRO_BITMAP *punto;
ALLEGRO_BITMAP *pacmanArriba;
ALLEGRO_BITMAP *pacmanAbajo;
ALLEGRO_BITMAP *pacmanIzq;
ALLEGRO_BITMAP *pacmanDer;
ALLEGRO_BITMAP *semilla;
ALLEGRO_BITMAP *blinky;
ALLEGRO_BITMAP *inky;
ALLEGRO_BITMAP *pinky;
ALLEGRO_BITMAP *clyde;
ALLEGRO_KEYBOARD_STATE keyState;

bool estadoFantasma = false;
pthread_mutex_t semc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semf = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t semp = PTHREAD_MUTEX_INITIALIZER; 
bool escapan = false;

struct info{
  int xf;
  int yf;
  int id;
  int dirf;
};

struct NodoFW{
    int indice;
    int numNodo;
    int posx;
    int posy;
    int Arr_Pos[maxi];
};

struct NodoFW arr_nodofw[maxarreglo];
int matriz_recorrido[nodos][nodos];
int dist[nodos][nodos];
int Camino[100];
int inteligencia[100];

enum Direction {UP,DOWN,LEFT,RIGHT};

int CantMovimiento=0;
bool done = false, draw=true;
int x = 420, y = 510;  
int moveSpeed = 30;
int dir = DOWN; 
int state = 0;
struct info B = {420,180,0,RIGHT};
struct info I = {390,270,1,UP};
struct info P = {420,270,2,DOWN};
struct info C = {450,270,3,LEFT};
int vidas = 20;
int NumCoordenadas[32] = {1,2,17,27,10,8,4,15,6,26,13,3,17,5,1,22,13,19,3,5,6,12,9,26,15,15,17,18,8,1,18,2};
char mapa[maxfil][maxcol];
int ax,ay;
pthread_mutex_t semc;
pthread_mutex_t semf;
pthread_mutex_t semp;

char mapa_original[maxfil][maxcol]={ 
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
  "~|cccccccccc~~~~~cccccccccc|~",
  "~c~~~c~~~~~c~~~~~c~~~~~c~~~c~",
  "~c~~~c~~~~~c~~~~~c~~~~~c~~~c~",
  "~cccccccccccccccc|cccccccccc~",
  "~c~~~c~~c~~~~~~~~~~~c~~c~~~c~",
  "~ccccc~~|cccccccccccc~~|cccc~",
  "~c~~~c~~c~~~~   ~~~~c~~c~~~c~",
  "~c~~~c~~c~~~~   ~~~~c~~c~~~c~",
  "c|cccc~~c~~~~   ~~~~c~~cccccc",
  "~c~~~c~~c~~~~~~~~~~~c~~c~~~c~",
  "~c~~~c~~|ccccccccccc|~~c~~~c~",
  "~c~~~c~~~~~~c~~~c~~~~~~c~~~c~",
  "~cccc|~~ccccc~~~ccccc~~|cccc~",
  "~c~~~c~~c~~~~~~~~~~~c~~c~~~c~",
  "~c~~~ccccc|ccccccccccccc~~~c~",
  "~c~~~c~~~~c~~~~~~~~c~~~c~~~c~",
  "~c~~~c~~~~cccccccccc~~~c~~~c~",
  "~|ccccccccc~~~~~~~~cccccccc|~",
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
};

int verificar_tablero(){
  for(int i=0;i<maxfil;i++){
    for(int j=0;j<maxcol;j++){
      if(mapa[i][j]=='c' || mapa[i][j]=='|' || mapa[i][j]=='s'){ 
          return 0;
          break;
      }
    }
  }
  return 1;
}

void llenarTablero(){
  for(int i=0;i<maxfil;i++){
    for(int j=0;j<maxcol;j++){
      mapa[i][j]=mapa_original[i][j];
    }
  }
}

void llenarArreglo(){
  int i=0,nodo,a,n;
  while(i < maxarreglo){
    for(int j=0;j < maxfil;j++){
      for(int k=0;k < maxcol;k++){
        nodo = (j*(maxcol))+k;
        if(mapa[j][k]!='~'){
          arr_nodofw[i].indice=i;
          arr_nodofw[i].numNodo=nodo;
          arr_nodofw[i].posx=j;
          arr_nodofw[i].posy=k;
          mapa[j-1][k]!='~' ? (arr_nodofw[i].Arr_Pos[0] = ((j-1)*(maxcol))+k) 
                            : (arr_nodofw[i].Arr_Pos[0] = num); 
          mapa[j+1][k]!='~' ? (arr_nodofw[i].Arr_Pos[1] = ((j+1)*(maxcol))+k)
                              : (arr_nodofw[i].Arr_Pos[1] = num); 
          mapa[j][k-1]!='~' ? (arr_nodofw[i].Arr_Pos[2] = (j*(maxcol))+k-1)
                              : (arr_nodofw[i].Arr_Pos[2] = num); 
          mapa[j][k+1]!='~' ? (arr_nodofw[i].Arr_Pos[3] = (j*(maxcol))+k+1)
                              : (arr_nodofw[i].Arr_Pos[3] = num); 
      i++;
      }

    }
  }
}
  for(int i = 0; i < maxarreglo; i++){
    for(int j = 0; j < maxarreglo; j++){
      if(dist[i][j]==inf){
        matriz_recorrido[i][j]= -1;
      }else{
        matriz_recorrido[i][j]=j;
      }
    }
  } 
}

int retornarpos(int n){
  int i=0,p;
  while(i<maxarreglo){
    p= arr_nodofw[i].numNodo;
    if(p==n){
      return i;
    }
    i++;
  }
}

/*
int retornarpos(int min,int max,int buscado) {
  int pivote = (max + min) / 2;
  if(arr_nodofw[pivote].numNodo == buscado){ 
    return pivote; 
  }
  if(arr_nodofw[pivote].numNodo < buscado){
    return retornarpos(pivote, max,buscado); 
  }
  if(arr_nodofw[pivote].numNodo > buscado){
    return retornarpos(min, pivote,buscado);
  }
  return -1;  
}
*/

void inicializar_matriz_distancia(){
  int p,pos;
  for(int i=0;i<maxarreglo;i++){
    for(int j=0;j<maxarreglo;j++){
      (i==j) ?  (dist[i][j]=0) : (dist[i][j]=inf); 
    }
  }

  for(int i=0;i<maxarreglo;i++){
    for(int a=0;a<maxi;a++){
        pos=arr_nodofw[i].Arr_Pos[a];
        if(pos!=num)
          p=retornarpos(pos);
          dist[p][i]=1;
    }
  }
}

void floyd_warshall(){  
  for(int k = 0; k < maxarreglo; k++) {  
    for(int i = 0; i < maxarreglo; i++) {  
      for(int j = 0; j < maxarreglo; j++) {  

        if(dist[i][k] == inf || dist[k][j] == inf)  
            continue;  
  
        if(dist[i][j] > dist[i][k] + dist[k][j]){  
            dist[i][j] = dist[i][k] + dist[k][j];  
            matriz_recorrido[i][j] = matriz_recorrido[i][k];  
        }  
      }  
    }  
  }  
}

void encontrar_ruta(int ini,int fin){ 
  if(matriz_recorrido[ini][fin] == -1){
    Camino[0]=0;
  }
  Camino[0]=ini;
  int contador=1;
  while(ini!=fin){
    ini = matriz_recorrido[ini][fin];
    Camino[contador]=ini;
    contador++;
  }
}

void encontrar_ruta_inteligencia(int ini,int fin){ 
  if(matriz_recorrido[ini][fin] == -1){
    inteligencia[0]=0;
  }
  inteligencia[0]=ini;
  int contador=1;
  while(ini!=fin){
    ini = matriz_recorrido[ini][fin];
    inteligencia[contador]=ini;
    contador++;
  }
}

void colocarSemillas(int Cant,char str[]){
  int i=0;
  int xx;
  int yy;
  if(strcmp(str,"normal")==0){
    while(i<Cant*2){
      xx= NumCoordenadas[i];
      yy=NumCoordenadas[++i];
      ++i;
      mapa[xx][yy] = 's';
    }    
  }else if(strcmp(str,"aleatorio")==0){
    srand(time(NULL));
    while(i<Cant){
       xx=rand()%19;
       yy=rand()%30;
       while(mapa[xx][yy] != 'c'){
         xx=rand()%19;
         yy=rand()%30;
       }
       mapa[xx][yy] = 's';
       ++i;
     }
   }
}

void dibujar_mapa(ALLEGRO_BITMAP *r,ALLEGRO_BITMAP *p,ALLEGRO_BITMAP *s){
    for(int i = 0; i< maxfil; i++)
      for(int j = 0; j< maxcol; j++)
        if(mapa[i][j] == '~'){
            al_draw_bitmap_region(r,0,0,30,30,j*30,i*30,0);                         
        }else if(mapa[i][j] == 'c' ||  mapa[i][j] == '|'){
            al_draw_bitmap_region(p,0,0,30,30,j*30,i*30,0);
            if(( (y/30) == i ) && ( (x/30) ==j))
                mapa[i][j] = ' ';
        }else if(mapa[i][j] == 's'){
            al_draw_bitmap_region(s,0,0,30,30,j*30,i*30,0);
            if(((y/30) == i) && ((x/30) ==j)){
                mapa[i][j] = ' ';
                estadoFantasma = true; 
                CantMovimiento =10;
        }              
    }
}


void dibujar_pacman(){
    if(dir == 0){
        al_draw_bitmap(pacmanArriba,x,y,0);      
    }else if(dir == 1){
        al_draw_bitmap(pacmanAbajo,x,y,0);
    }else if(dir == 2){
        al_draw_bitmap(pacmanIzq,x,y,0);
    }else if(dir == 3){
        al_draw_bitmap(pacmanDer,x,y,0);
    }else{
        al_draw_bitmap(pacmanDer,x,y,0);
    }
}

void dibujar_fantasma(ALLEGRO_BITMAP *pm, int xf,int yf){
  if(estadoFantasma == true){
      al_draw_tinted_bitmap(pm, al_map_rgba_f(1, 0, 1, 1), xf, yf, 0);
    }else{
       al_draw_tinted_bitmap(pm, al_map_rgba_f(1, 1, 1, 1), xf, yf, 0);       
    }
}

void volver_casita(int xi,int yj,int fantasma){
    int inicio = retornarpos((yj/30) * maxcol + (xi/30));
    int destino;
    int velocidad = speedClyde*1.2;

    if(fantasma == 0){
        destino = retornarpos(6 * maxcol + 14);  
        encontrar_ruta(inicio,destino); 
        size_t largo = sizeof(Camino)/sizeof(Camino[0]);

        int i = 0;
        pthread_mutex_lock(&semf); 
        while (Camino[i] != 78){
            B.xf = arr_nodofw[Camino[i]].posy*30;
            B.yf = arr_nodofw[Camino[i]].posx*30;
            dibujar_fantasma(blinky,B.xf,B.yf);
            i++;
            sleep(1/velocidad);
         }    
         
        pthread_mutex_unlock(&semf);            
    }

    if(fantasma == 1){  
        destino = retornarpos(9 * maxcol + 13);  
        encontrar_ruta(inicio,destino); 
        size_t largo = sizeof(Camino)/sizeof(Camino[0]);

        int i = 0;
        pthread_mutex_lock(&semf); 
        while (Camino[i] != 115){
            I.xf = arr_nodofw[Camino[i]].posy *30;     //Esto es Y
            I.yf = arr_nodofw[Camino[i]].posx *30;     //Esto es X
            dibujar_fantasma(inky,I.xf,I.yf);
            i++;
            sleep(1/velocidad);
        }         
        pthread_mutex_unlock(&semf);         
    }

    if(fantasma == 2){      
        destino = retornarpos(9 * maxcol + 14);  
        encontrar_ruta(inicio,destino); 
        size_t largo = sizeof(Camino)/sizeof(Camino[0]);

        int i = 0;
        pthread_mutex_lock(&semf); 
        while (Camino[i] != 116){
            P.xf = arr_nodofw[Camino[i]].posy *30;     //Esto es Y
            P.yf = arr_nodofw[Camino[i]].posx *30;     //Esto es X
            dibujar_fantasma(pinky,P.xf,P.yf);
            i++;
            sleep(1/velocidad);
        }
        
        pthread_mutex_unlock(&semf);         
    }
     
    if(fantasma == 3){      
        destino = retornarpos(9 * maxcol + 15);  
        encontrar_ruta(inicio,destino); 
        size_t largo = sizeof(Camino)/sizeof(Camino[0]);

        int i = 0;
        pthread_mutex_lock(&semf); 
        while (Camino[i] != 117){
            C.xf = arr_nodofw[Camino[i]].posy *30;     //Esto es Y
            C.yf = arr_nodofw[Camino[i]].posx *30;     //Esto es X
            dibujar_fantasma(clyde,C.xf,C.yf);
            i++;
            sleep(1/velocidad);
        }
        pthread_mutex_unlock(&semf);
    }
}

void mover_inky(){
    if(mapa[(I.yf-30)/30][I.xf/30] == '|'){
      srand(time(NULL));
      I.dirf = rand()%4;
    }

    if(I.dirf == 0){ //up
      if(mapa[(I.yf-30)/30][I.xf/30] != '~'){
        I.yf-= moveSpeed;
      }else{
        srand(time(NULL));
        I.dirf = rand()%4;
      }
    }

    if(I.dirf == 1){ //down
      if(mapa[(I.yf+30)/30][I.xf/30] != '~'){
        I.yf+=moveSpeed;
      }else{
        srand(time(NULL));
        I.dirf = rand()%4;
      }
    } 

    if(I.dirf == 2){ //left
      if(mapa[I.yf/30][(I.xf-30)/30] != '~'){
        I.xf-=moveSpeed;
      }else{ 
        srand(time(NULL));
        I.dirf = rand()%4;
      }
    } 

    if(I.dirf == 3){ //right
      if(mapa[I.yf/30][(I.xf+30)/30] != '~'){
        I.xf +=moveSpeed;
      }else{ 
        srand(time(NULL));
        I.dirf = rand()%4;
      } 
    }

    if(I.xf <= 0)
      I.xf=840;
    else if(I.xf >= 840)
      I.xf=0;
}



void choque_camino(){
  if(CantMovimiento!=0){
    if ((B.xf == x && B.yf == y) || (B.xf == ax && B.yf == ay)){
       volver_casita(B.xf,B.yf,0);
    }
      
    if((I.xf == x && I.yf == y) || (I.xf == ax && I.yf == ay)){
       volver_casita(I.xf,I.yf,1);
    }

    if((P.xf == x && P.yf == y) || (P.xf == ax && P.yf == ay)){
       volver_casita(P.xf,P.yf,2);
    }

    if ((C.xf == x && C.yf == y) || (C.xf == ax && C.yf == ay)){
       volver_casita(C.xf,C.yf,3);
    }
  }else{
      estadoFantasma = false;
  }
}

void inteligencia_fantasma(int inicio,int destino,int fantasma){
    int py=y;
    int px=x;
    encontrar_ruta_inteligencia(destino,inicio);    
    int direccion;
    int contador=0;
    int inicio1,destino1;

    if(arr_nodofw[inteligencia[0]].posy == arr_nodofw[inteligencia[1]].posy == arr_nodofw[inteligencia[1]].posx){
        destino = retornarpos((ay/30) * maxcol + (ax/30)); 
        py=ay;
        px=ax;
        encontrar_ruta_inteligencia(destino,inicio);
    }

    if(arr_nodofw[inteligencia[0]].posy == arr_nodofw[inteligencia[1]].posy){
        ((arr_nodofw[inteligencia[1]].posx - arr_nodofw[inteligencia[0]].posx) >0) ? (direccion=0) : (direccion=1);
        for(int i=1;arr_nodofw[inteligencia[0]].posy == arr_nodofw[inteligencia[i]].posy;i++) contador++;
    }else{
        ((arr_nodofw[inteligencia[1]].posy - arr_nodofw[inteligencia[0]].posy) >0) ? (direccion=2) :  (direccion=3);
        for(int i=1;arr_nodofw[inteligencia[0]].posx == arr_nodofw[inteligencia[i]].posx;i++) contador++;         
    }

    contador=contador%5;
       
    if(direccion==0){
        for(int i=0;i<=contador;i++){
            if(mapa[py/30][px/30]=='~'){
              px+=30;
              break;
            }else{
              px-=30;
            }
        }
    }else if(direccion==1){
        for(int i=0;i<=contador;i++){ 
            if(mapa[py/30][px/30]=='~'){
              px-=30;
              break;
            }else{
              px+=30;
            }
        }
    }else if(direccion==2){
        for(int i=0;i<=contador;i++){
            if(mapa[py/30][px/30]=='~'){
              py+=30;
              break;
            }else{
              py-=30;
            }
        }
    }else if(direccion==3){
        for(int i=0;i<=contador;i++){
            if(mapa[py/30][px/30]=='~'){
              py-=30;
              break;
            }else{
              py+=30;
            }
        }
    }

    if(fantasma==0){
        inicio1 = retornarpos((B.yf/30) * maxcol + (B.xf/30));
        destino1 = retornarpos((py/30) * maxcol + (px/30));
        if(destino1==549){
            destino1 = retornarpos((py/30) * maxcol + (px/30));
            encontrar_ruta_inteligencia(inicio1,destino);
        }else{
            encontrar_ruta_inteligencia(inicio1,destino1);
        }      
       	int ruta1 = inteligencia[1]; 
        B.xf = arr_nodofw[ruta1].posy *30;     //Esto es Y
        B.yf = arr_nodofw[ruta1].posx *30;     //Esto es X
    }

    if(fantasma==3){
        inicio1 = retornarpos((C.yf/30) * maxcol + (C.xf/30));
        destino1 = retornarpos((py/30) * maxcol + (px/30));
        if(destino1==549){
            destino1 = retornarpos((py/30) * maxcol + (px/30));
            encontrar_ruta_inteligencia(inicio1,destino);
        }else{
            encontrar_ruta_inteligencia(inicio1,destino1);
        }      
        int ruta1 = inteligencia[1]; 
        C.xf = arr_nodofw[ruta1].posy *30;     //Esto es Y
        C.yf = arr_nodofw[ruta1].posx *30;     //Esto es X
    }
}

void * mover_fantasmas(void *entrada){
     
    int xi = ((struct info*)entrada)->xf;
    int yj = ((struct info*)entrada)->yf;
    int fantasma = ((struct info*)entrada)->id;

    int destino,inicio; 
    
    if(escapan != false){ 
        inicio = retornarpos((yj/30) * maxcol + (xi/30));
        destino = 116;
        encontrar_ruta(inicio,destino);  

    }else{
        inicio = retornarpos((yj/30) * maxcol + (xi/30));
        destino = retornarpos((y/30) * maxcol + (x/30));  
       encontrar_ruta(inicio,destino); 
    } 

    size_t largo = sizeof(Camino)/sizeof(Camino[0]);
   
    int ruta = Camino[1];


    if(fantasma == 3){    
         pthread_mutex_lock(&semc); 
         if (escapan != false) {
            C.xf = arr_nodofw[ruta].posy *30;     //Esto es Y
            C.yf = arr_nodofw[ruta].posx *30;     //Esto es X
         }else{
            inteligencia_fantasma(inicio,destino,3);
         }
         sleep(1/speedClyde);
         pthread_mutex_unlock(&semc);  
     }

    if(fantasma == 0){  
         pthread_mutex_lock(&semf); 
         if (escapan != false) {
            B.xf = arr_nodofw[ruta].posy *30;     //Esto es Y
            B.yf = arr_nodofw[ruta].posx *30;     //Esto es X
         }else{
            inteligencia_fantasma(inicio,destino,0);
         }
         sleep(1/speedFants);
         pthread_mutex_unlock(&semf);
    }

    if(fantasma == 1){ 
         pthread_mutex_lock(&semf);       
         mover_inky();
         sleep(1/speedFants);
         pthread_mutex_unlock(&semf);
    }

    if(fantasma == 2){  
         pthread_mutex_lock(&semf);     
         P.xf = arr_nodofw[ruta].posy *30;     //Esto es Y
         P.yf = arr_nodofw[ruta].posx *30;     //Esto es X
         sleep(1/speedFants);
         pthread_mutex_unlock(&semf);
    } 
}

void * teclas(void * param){
    pthread_mutex_lock(&semp);  
    al_get_keyboard_state(&keyState);
    if(al_key_down(&keyState,ALLEGRO_KEY_DOWN)){
      dir = DOWN;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_UP)){
      dir = UP;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_RIGHT)){
      dir = RIGHT;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_LEFT)){
      dir=LEFT;         
    }

    if(CantMovimiento>0){
      CantMovimiento-=1;
      escapan = true;
    }else{
      escapan = false;
    }

    if(dir == 0) //up
      (mapa[(y-30)/30][x/30] != '~') ? (y-=moveSpeed) : (dir = 4);

    if(dir == 1) //down
      (mapa[(y+30)/30][x/30] != '~') ? (y+=moveSpeed) : (dir = 4);
 
    if(dir == 2) //left
      (mapa[y/30][(x-30)/30] != '~') ? (x-=moveSpeed) : (dir = 4);
    
    if(dir == 3) //right
     (mapa[y/30][(x+30)/30] != '~') ? (x+=moveSpeed) : (dir = 4);
           
    //Rutina para atajo
    if(x <= 0)
      x=840;
    else if(x >= 840)
      x=0;

    sleep(1/speedPacman);
    pthread_mutex_unlock(&semp);
}

void perdio(){
  if(estadoFantasma==false){
    if (vidas == 0){
      done = true;
    }  

    if ((B.xf == x && B.yf == y)||(I.xf == x && I.yf == y)||(P.xf == x && P.yf == y)||(C.xf == x && C.yf == y) ||
        (B.xf == ax && B.yf == ay)||(I.xf == ax && I.yf == ay)||(P.xf == ax && P.yf == ay)||(C.xf == ax && C.yf == ay)){
         B.xf=420;
         B.yf=180;
         I.xf=390;
         I.yf=270;
         P.xf=420;
         P.yf=270;
         C.xf=450;
         C.yf=270;
         x = 420;
         y = 510;
         vidas -=1;     
      }
  }
}

void allegro_funciones(){
    float tiempo = 60.0;
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_image_addon();
    
    pthread_mutex_init(&semc,NULL);
    pthread_mutex_init(&semf,NULL);
    pthread_mutex_init(&semp,NULL);

    pthread_t hclyde;
    pthread_t hpacman;
    pthread_t hblinky;
    pthread_t hinky;
    pthread_t hpinky;

    ALLEGRO_TIMER* timer = al_create_timer(1.0/10);
    ALLEGRO_DISPLAY* disp = al_create_display(880,600);
    
    al_set_window_title(disp,"Pac-Man"); //nombre de la ventana
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    
    roca = al_load_bitmap("roca.jpg");
    punto = al_load_bitmap("punto.png");
    semilla = al_load_bitmap("Semilla.png");
    pacmanArriba = al_load_bitmap("pacmanArriba.png");
    pacmanAbajo = al_load_bitmap("pacmanAbajo.png");
    pacmanIzq = al_load_bitmap("pacmanIzq.png");
    pacmanDer = al_load_bitmap("pacmanDer.png");
    blinky = al_load_bitmap("Blinky.png");
    inky = al_load_bitmap("Inky.png");
    pinky = al_load_bitmap("Pinky.png");
    clyde = al_load_bitmap("Clyde.png");

    ALLEGRO_BITMAP* bmp = al_create_bitmap(880,600);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_hide_mouse_cursor(disp);
    al_start_timer(timer);
    al_set_target_bitmap(bmp);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_set_target_bitmap(al_get_backbuffer(disp));

    while(!done){
      ALLEGRO_EVENT event;

      al_wait_for_event(queue,&event);
      if(event.type == ALLEGRO_EVENT_KEY_UP){
        switch(event.keyboard.keycode){
          case ALLEGRO_KEY_ESCAPE:
            done = true;
        }
      }else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
          done = true;
          break;
      }

      if(event.type == ALLEGRO_EVENT_TIMER){
        al_draw_bitmap(bmp , 0 , 0 , 0);
        dibujar_mapa(roca,punto,semilla);
        ax = x;
        ay = y;
        dibujar_pacman();
        dibujar_fantasma(blinky,B.xf,B.yf);
        dibujar_fantasma(inky,I.xf,I.yf);
        dibujar_fantasma(pinky,P.xf,P.yf);
        dibujar_fantasma(clyde,C.xf,C.yf);
        perdio();
        choque_camino();
        if(event.timer.source == timer){
          pthread_create(&hpacman, NULL,teclas, NULL);
          pthread_create(&hclyde,NULL,mover_fantasmas,&C);
          pthread_create(&hblinky,NULL,mover_fantasmas,&B);
          pthread_create(&hinky,NULL,mover_fantasmas,&I);
          pthread_create(&hpinky,NULL,mover_fantasmas,&P); 
        }
   
        if(verificar_tablero()==1){
           speedClyde=speedClyde*1.1;
           speedFants=speedFants*1.1;  
           llenarTablero();
           colocarSemillas(CantiSemilla,TipoSemilla);
           B.xf=420;
           B.yf=180;
           I.xf=390;
           I.yf=270;
           P.xf=420;
           P.yf=270;
           C.xf=450;
           C.yf=270;
           x = 420;
           y = 510;
           CantMovimiento=0;
        }

        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));  
        
       }
    } 

    al_destroy_bitmap(roca);
    al_destroy_bitmap(pacmanArriba);
    al_destroy_bitmap(pacmanAbajo);
    al_destroy_bitmap(pacmanIzq);
    al_destroy_bitmap(pacmanDer);        
    al_destroy_bitmap(punto);
    al_destroy_bitmap(semilla);
    al_destroy_bitmap(blinky);
    al_destroy_bitmap(inky);
    al_destroy_bitmap(pinky);
    al_destroy_bitmap(clyde);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}

int main(){
    FILE* ptr = fopen("ArchivoConfig.txt","r");     
    float VelocidadFant;
    float VelocidadPacman;
    fscanf(ptr,"%f",&VelocidadFant); 
    fscanf(ptr,"%f",&VelocidadPacman);
    fscanf(ptr,"%s",TipoSemilla); 
    fscanf(ptr,"%d",&CantiSemilla);
    speed=VelocidadFant;
    speedClyde=VelocidadFant*0.8;
    speedPacman=VelocidadPacman;
    speedFants=VelocidadFant;
    llenarTablero();
    colocarSemillas(CantiSemilla,TipoSemilla);
    llenarArreglo();
    inicializar_matriz_distancia();
    floyd_warshall();
    allegro_funciones();    
    return 0;
}

