#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define MAXFILAS 20
#define MAXCOLS  29
#define inf 999999
#define num 98989
#define NODOS 240
#define MAXARREGLO 240
#define MAXI 4


bool estadoTablero=true;

float v_timer;
float v_timerClyde;
float v_timerPacman;
float v_timerFants;

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

void reinciar(void);
pthread_t hilos[5];
pthread_mutex_t sem; 
bool estadoFantasma = false;

struct info{
  int xf;
  int yf;
  int ff;
  int dirf;
};

struct NodoFW{
    int indice;
    int numNodo;
    int posx;
    int posy;
    int Arr_Pos[MAXI];
};

struct NodoFW arr_nodofw[MAXARREGLO];
int NEXT[NODOS][NODOS];
int DIST[NODOS][NODOS];
int CAMINITO[100];

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
int vidas = 5;
int NumCoordenadas[32] = {1,2,17,27,10,8,4,15,6,26,13,3,17,5,1,22,13,19,3,5,6,12,9,26,15,15,17,18,8,1,18,2};
char mapa[MAXFILAS][MAXCOLS];

char mapa_original[MAXFILAS][MAXCOLS]={ 
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
  for(int i=0;i<MAXFILAS;i++){
    for(int j=0;j<MAXCOLS;j++){
      if(mapa[i][j]=='c' || mapa[i][j]=='|'){ 
          return 0;
          break;
      }
    }
  }
  return 1;
}

void llenarTablero(){
  for(int i=0;i<MAXFILAS;i++){
    for(int j=0;j<MAXCOLS;j++){
      mapa[i][j]=mapa_original[i][j];
    }
  }
}



void llenarArreglo(){
  int i=0,nodo,a,n;
  while(i < MAXARREGLO){
    for(int j=0;j < MAXFILAS;j++){
      for(int k=0;k < MAXCOLS;k++){
        nodo = (j*(MAXCOLS))+k;
        if(mapa[j][k]!='~'){
          arr_nodofw[i].indice=i;
          arr_nodofw[i].numNodo=nodo;
          arr_nodofw[i].posx=j;
          arr_nodofw[i].posy=k;
          mapa[j-1][k]!='~' ? (arr_nodofw[i].Arr_Pos[0] = ((j-1)*(MAXCOLS))+k) 
                            : (arr_nodofw[i].Arr_Pos[0] = num); 
          mapa[j+1][k]!='~' ? (arr_nodofw[i].Arr_Pos[1] = ((j+1)*(MAXCOLS))+k)
                              : (arr_nodofw[i].Arr_Pos[1] = num); 
          mapa[j][k-1]!='~' ? (arr_nodofw[i].Arr_Pos[2] = (j*(MAXCOLS))+k-1)
                              : (arr_nodofw[i].Arr_Pos[2] = num); 
          mapa[j][k+1]!='~' ? (arr_nodofw[i].Arr_Pos[3] = (j*(MAXCOLS))+k+1)
                              : (arr_nodofw[i].Arr_Pos[3] = num); 
      i++;
      }

    }
  }
}
  for(int i = 0; i < MAXARREGLO; i++){
    for(int j = 0; j < MAXARREGLO; j++){
      if(DIST[i][j]==inf){
        NEXT[i][j]= -1;
      }else{
        NEXT[i][j]=j;
      }
    }
  } 
}

int retornarpos(int n){
  int i=0,p;
  while(i<MAXARREGLO){
    p= arr_nodofw[i].numNodo;
    if(p==n){
      return i;
    }
    i++;
  }
}

void inicializar_matriz_distancia(){
  int p,pos;
  for(int i=0;i<MAXARREGLO;i++){
    for(int j=0;j<MAXARREGLO;j++){
      (i==j) ?  (DIST[i][j]=0) : (DIST[i][j]=inf); 
    }
  }

  for(int i=0;i<MAXARREGLO;i++){
    for(int a=0;a<MAXI;a++){
        pos=arr_nodofw[i].Arr_Pos[a];
        if(pos!=num)
          p=retornarpos(pos);
          DIST[p][i]=1;
    }
  }
}

void FW(){  
  for(int k = 0; k < MAXARREGLO; k++) {  
    for(int i = 0; i < MAXARREGLO; i++) {  
      for(int j = 0; j < MAXARREGLO; j++) {  

        if(DIST[i][k] == inf || DIST[k][j] == inf)  
            continue;  
  
        if (DIST[i][j] > DIST[i][k] + DIST[k][j]) {  
                    DIST[i][j] = DIST[i][k] + DIST[k][j];  
                    NEXT[i][j] = NEXT[i][k];  
                }  
            }  
        }  
    }  
}

void encontrar_ruta(int ini,int fin){ 
  if(NEXT[ini][fin] == -1){
    CAMINITO[0]=0;
  }
  CAMINITO[0]=ini;
  int contador=1;
  while(ini!=fin){
    ini = NEXT[ini][fin];
    CAMINITO[contador]=ini;
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
    for(int i = 0; i< MAXFILAS; i++)
      for(int j = 0; j< MAXCOLS; j++)
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
                CantMovimiento+=40;
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
    int inicio = retornarpos((yj/30) * MAXCOLS + (xi/30));
    int destino;

    if (fantasma == 0){
         destino = retornarpos(6 * MAXCOLS + 14);  
         encontrar_ruta(inicio,destino); 
         printf("%d  -----   %d\n",inicio,destino);
         size_t largo = sizeof(CAMINITO)/sizeof(CAMINITO[0]);

         int i = 0;
         printf("HOLA\n");
         while (CAMINITO[i] != 78){

             B.xf = arr_nodofw[CAMINITO[i]].posy*30;
             B.yf = arr_nodofw[CAMINITO[i]].posx*30;
             printf("POS%d%d\n",B.xf,B.yf);
             dibujar_fantasma(blinky,B.xf,B.yf);
             i++;
         }
            
    }

    if (fantasma == 1){  
         destino = retornarpos(9 * MAXCOLS + 13);  
         encontrar_ruta(inicio,destino); 
         size_t largo = sizeof(CAMINITO)/sizeof(CAMINITO[0]);

         int i = 0;
         while (CAMINITO[i] != 115){
             I.xf = arr_nodofw[CAMINITO[i]].posy *30;     //Esto es Y
             I.yf = arr_nodofw[CAMINITO[i]].posx *30;     //Esto es X
             dibujar_fantasma(inky,I.xf,I.yf);
             i++;
         }
     }

    if (fantasma == 2){      
         destino = retornarpos(9 * MAXCOLS + 14);  
         encontrar_ruta(inicio,destino); 
         size_t largo = sizeof(CAMINITO)/sizeof(CAMINITO[0]);

         int i = 0;
         while (CAMINITO[i] != 116){
             P.xf = arr_nodofw[CAMINITO[i]].posy *30;     //Esto es Y
             P.yf = arr_nodofw[CAMINITO[i]].posx *30;     //Esto es X
             dibujar_fantasma(pinky,P.xf,P.yf);
             i++;
         }
     }
     
    if (fantasma == 3){      
         destino = retornarpos(9 * MAXCOLS + 15);  
         encontrar_ruta(inicio,destino); 
         size_t largo = sizeof(CAMINITO)/sizeof(CAMINITO[0]);

         int i = 0;
         while (CAMINITO[i] != 117){
             C.xf = arr_nodofw[CAMINITO[i]].posy *30;     //Esto es Y
             C.yf = arr_nodofw[CAMINITO[i]].posx *30;     //Esto es X
             dibujar_fantasma(clyde,C.xf,C.yf);
             i++;
         }
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
  //pthread_mutex_lock(&sem);
  //printf("%d",CantMovimiento);
  //put(CantMovimiento);

  if(CantMovimiento!=0){
    if (B.xf == x && B.yf == y){
      printf("BLINKY %d %d",B.xf,B.yf);
       volver_casita(B.xf,B.yf,0);
      }
      
    if (I.xf == x && I.yf == y){
       volver_casita(I.xf,I.yf,1);
    }
    if (P.xf == x && P.yf == y){
       volver_casita(P.xf,P.yf,2);
    }
    if (C.xf == x && C.yf == y){
       volver_casita(C.xf,C.yf,3);
    }
  }else{
      estadoFantasma = false;
  }
}

void * mover_random(void *entrada){
    pthread_mutex_lock(&sem);
    int xi = ((struct info*)entrada)->xf;
    int yj = ((struct info*)entrada)->yf;
    int fantasma = ((struct info*)entrada)->ff;

    int inicio = retornarpos((yj/30) * MAXCOLS + (xi/30));
    int destino = retornarpos((y/30) * MAXCOLS + (x/30));  

    encontrar_ruta(inicio,destino); 
    size_t largo = sizeof(CAMINITO)/sizeof(CAMINITO[0]);
   
    int ruta = CAMINITO[1];
      
    if (fantasma == 0){       
         B.xf = arr_nodofw[ruta].posy *30;     //Esto es Y
         B.yf = arr_nodofw[ruta].posx *30;     //Esto es X
    }

    if (fantasma == 1){       
         mover_inky();
    }

    if (fantasma == 2){      
         P.xf = arr_nodofw[ruta].posy *30;     //Esto es Y
         P.yf = arr_nodofw[ruta].posx *30;     //Esto es X
     }
     
     if (fantasma == 3){      
        C.xf = arr_nodofw[ruta].posy *30;     //Esto es Y
        C.yf = arr_nodofw[ruta].posx *30;     //Esto es X
     }

    pthread_mutex_unlock(&sem); 
  }

void * teclas(void * param){
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
    printf("%d\n",CantMovimiento );
    if(CantMovimiento>0){
      CantMovimiento-=1;
    }

    if(dir == 0){ //up
      if(mapa[(y-30)/30][x/30] != '~')
        y-=moveSpeed;
      else dir = 4;
    }
    if(dir == 1){ //down
      if(mapa[(y+30)/30][x/30] != '~')
        y+=moveSpeed;
      else dir = 4;
    } 
    if(dir == 2){ //left
      if(mapa[y/30][(x-30)/30] != '~')
        x-=moveSpeed;
      else dir = 4;
    } 
    if(dir == 3){ //right
      if(mapa[y/30][(x+30)/30] != '~')
        x+=moveSpeed;
      else dir = 4;
    }      
    
    //Rutina para atajo
    if(x <= 0)
      x=840;
    else if(x >= 840)
      x=0;
}

void perdio(){
  if(estadoFantasma==false){
    if (vidas == 0){
      done = true;
    }  
    printf("vida%d\n",vidas);
    if ((B.xf == x && B.yf == y)||(I.xf == x && I.yf == y)||(P.xf == x && P.yf == y)||(C.xf == x && C.yf == y)){
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

    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_image_addon();
    
    pthread_mutex_init(&sem, 0);
    pthread_mutex_init(&sem, 0);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 500);
    ALLEGRO_TIMER* timerClyde = al_create_timer(1.0 / v_timerPacman);
    ALLEGRO_TIMER* timerPacman = al_create_timer(1.0 / v_timerClyde);
    ALLEGRO_TIMER* timerFants = al_create_timer(1.0 / v_timerFants);
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
    al_register_event_source(queue, al_get_timer_event_source(timerClyde));
    al_register_event_source(queue, al_get_timer_event_source(timerPacman));
    al_register_event_source(queue, al_get_timer_event_source(timerFants));

    al_hide_mouse_cursor(disp);
    
    al_start_timer(timer);
   /* al_start_timer(timerClyde);
    al_start_timer(timerPacman);
    al_start_timer(timerFants);
*/
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
        dibujar_pacman();
        dibujar_fantasma(blinky,B.xf,B.yf);
        dibujar_fantasma(inky,I.xf,I.yf);
        dibujar_fantasma(pinky,P.xf,P.yf);
        dibujar_fantasma(clyde,C.xf,C.yf);
        perdio();
        int b = verificar_tablero();
        printf("RES%d\n",b);
        if(event.timer.source == timer){
          pthread_create(&hilos[0], NULL,teclas, NULL);
        /*}else{
            if(event.timer.source == timer){
*/
              pthread_create(&(hilos[1]),NULL,mover_random,&B);
              pthread_create(&(hilos[2]),NULL,mover_random,&I);
              pthread_create(&(hilos[3]),NULL,mover_random,&P);
  //          }else if(event.timer.source == timerClyde){
              
              pthread_create(&(hilos[4]),NULL,mover_random,&C);
    //        }else if(event.timer.source==timerFants){

              choque_camino();
       //     }

              if(verificar_tablero()==1){
                v_timer=v_timer*1.1;
                v_timerClyde=v_timer*1.1*0.8;
                v_timerFants=v_timer*1.1*1.2;  
                llenarTablero();
                colocarSemillas(13,"normal");
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
        }

        for(int i = 0; i<5; i++)
          pthread_join(hilos[i],NULL);
          
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
    al_destroy_timer(timerClyde);
    al_destroy_timer(timerPacman);
    al_destroy_timer(timerFants);
    al_destroy_event_queue(queue);

}

/*
void reinciar(){

    allegro_funciones();
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
}*/

int main(){
    FILE* ptr = fopen("ArchivoConfig.txt","r");     
    float VelocidadFant;
    float ValocidadPacman;
    char TipoSemilla[12];
    int CantiSemilla;
    fscanf(ptr,"%f",&VelocidadFant); 
    fscanf(ptr,"%f",&ValocidadPacman);
    fscanf(ptr,"%s",TipoSemilla); 
    fscanf(ptr,"%d",&CantiSemilla);
    v_timer=VelocidadFant;
    v_timerClyde=VelocidadFant*0.8;
    v_timerPacman=ValocidadPacman;
    v_timerFants=VelocidadFant*1.2;
    llenarTablero();
    colocarSemillas(CantiSemilla,TipoSemilla);
    llenarArreglo();
    inicializar_matriz_distancia();
    FW();
    allegro_funciones();    
    return 0;
}
