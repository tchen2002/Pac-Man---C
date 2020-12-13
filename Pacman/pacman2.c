#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <pthread.h>

#define MAXFILAS 20
#define MAXCOLS  31

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

pthread_t hilos[5];
pthread_mutex_t sem;

struct info{
  int xf;
  int yf;
  int ff;
  int dirf;
};

enum Direction {UP,DOWN,LEFT,RIGHT};

bool done = false, draw=true;
bool estadoFantasma = true;
int x = 30, y = 30;  
int moveSpeed = 30;
int dir = DOWN; 
int state = 0;
struct info B = {420,180,0,RIGHT};
struct info I = {390,270,1,UP};
struct info P = {420,270,2,DOWN};
struct info C = {450,270,3,LEFT};
int NumCoordenadas[32] = {1,2,17,27,10,8,4,15,6,26,13,3,17,5,1,22,13,19,3,5,6,12,9,26,15,15,17,18,8,1,18,2};

char mapa[MAXFILAS][MAXCOLS]={ 
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

void delay(unsigned milliseconds){
    clock_t pause;
    clock_t start;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    start = clock();
    while((clock()-start)<pause);
}

/*
void comerSemillas(bool estadoFantasma){
    int i=0; 
    if(estadoFantasma == false){
         while(i < 5) { 
          delay(1000);
          ++i; 
      }   
    }

}
*/

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
            if(((y/30) == i) && ((x/30) ==j))
                //estadoFantasma = false;
                mapa[i][j] = ' ';
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
    //al_draw_bitmap(pm,xf,yf,0);
  if(estadoFantasma == false){
      al_draw_tinted_bitmap(pm, al_map_rgba_f(1, 0, 1, 1), xf, yf, 0);
    }else{
       al_draw_tinted_bitmap(pm, al_map_rgba_f(1, 1, 1, 1), xf, yf, 0);       
    }
}

void mover_fantasma(int xi,int yj,int direccion,int fantasma){
     if (fantasma == 0){       
        B.dirf = direccion;
        B.xf = xi;
        B.yf = yj;
     }
 
     if (fantasma == 1){       
        I.dirf = direccion;
        I.xf = xi;
        I.yf = yj;
     }
     
     if (fantasma == 2){      
        P.dirf = direccion;
        P.xf = xi;
        P.yf = yj;
     }
     
     if (fantasma == 3){      
        C.dirf = direccion;
        C.xf = xi;
        C.yf = yj;
     }
}

/*void * mover_binky(void *entrada){
   pthread_mutex_lock(&sem);

   pthread_mutex_unlock(&sem); 
}*/

void * mover_random(void *entrada){
    pthread_mutex_lock(&sem);
    int xi = ((struct info*)entrada)->xf;
    int yj = ((struct info*)entrada)->yf;
    int fantasma = ((struct info*)entrada)->ff;
    int direccion =((struct info*)entrada)->dirf;

    if(direccion == 0){ //up
      if(mapa[(yj-30)/30][xi/30] != '~'){
        yj-=moveSpeed;
        mover_fantasma(xi,yj,direccion,fantasma);
      }else{
        srand(time(NULL));
        direccion = rand()%4;
        mover_fantasma(xi,yj,direccion,fantasma);
      }
    }

    if(direccion == 1){ //down
      if(mapa[(yj+30)/30][xi/30] != '~'){
        yj+=moveSpeed;
        mover_fantasma(xi,yj,direccion,fantasma);
      }else{
        srand(time(NULL));
        direccion = rand()%4;
        mover_fantasma(xi,yj,direccion,fantasma);
      }
    } 
    if(direccion == 2){ //left
      if(mapa[yj/30][(xi-30)/30] != '~'){
        xi-=moveSpeed;
        mover_fantasma(xi,yj,direccion,fantasma);
      }else{ 
        srand(time(NULL));
        direccion = rand()%4;
        mover_fantasma(xi,yj,direccion,fantasma);
      }
    } 
    if(direccion == 3){ //right
      if(mapa[yj/30][(xi+30)/30] != '~'){
        xi+=moveSpeed;
        mover_fantasma(xi,yj,direccion,fantasma);
      }else{ 
        srand(time(NULL));
        direccion = rand()%4;
        mover_fantasma(xi,yj,direccion,fantasma);
      } 
    }
    pthread_mutex_unlock(&sem); 
}


void * mover_inky(void *entrada){
    pthread_mutex_lock(&sem);

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

    if(I.xf <= -30)
      I.xf=870;
    else if(I.xf >= 870)
      I.xf=-30;

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
    if(x <= -30)
      x=870;
    else if(x >= 870)
      x=-30;
}

void allegro_funciones(){

    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_image_addon();
    pthread_mutex_init(&sem, 0);
    pthread_mutex_init(&sem, 0);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);
    ALLEGRO_TIMER* timerClyde = al_create_timer(1.0 / 48);
    ALLEGRO_TIMER* timerPacman = al_create_timer(1.0 / 200);
    ALLEGRO_DISPLAY* disp = al_create_display(880,600);
    al_set_window_title(disp,"Pac-Man"); //nombre de la ventana
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP* bmp = al_create_bitmap(880,600);
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
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_timer_event_source(timerPacman));
    al_register_event_source(queue, al_get_timer_event_source(timerClyde));

    al_hide_mouse_cursor(disp);
    al_start_timer(timer);
    al_start_timer(timerClyde);
    al_start_timer(timerPacman);
    
    al_set_target_bitmap(bmp);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_set_target_bitmap(al_get_backbuffer(disp));


    while(!done){
      ALLEGRO_EVENT event;

      al_wait_for_event(queue,&event);
      if(event.type == ALLEGRO_EVENT_KEY_UP){
        puts("hola");
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
        if(event.timer.source == timerPacman){
          pthread_create(&hilos[0], NULL,teclas, NULL);
        }else{
            if(event.timer.source == timer){

              pthread_create(&(hilos[1]),NULL,mover_random,&B);
              pthread_create(&(hilos[2]),NULL,mover_inky,&I);
              pthread_create(&(hilos[3]),NULL,mover_random,&P);
            }else if(event.timer.source == timerClyde){
              
              pthread_create(&(hilos[4]),NULL,mover_random,&C);
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
    al_destroy_timer(timerPacman);
    al_destroy_timer(timerClyde);
    al_destroy_event_queue(queue);
}

int main(){
    FILE* ptr = fopen("ArchivoConfig.txt","r");     
    int VelocidadFant;
    int ValocidadPacman;
    char TipoSemilla[12];
    int CantiSemilla;
    fscanf(ptr,"%d",&VelocidadFant); 
    fscanf(ptr,"%d",&ValocidadPacman);
    fscanf(ptr,"%s",TipoSemilla); 
    fscanf(ptr,"%d",&CantiSemilla);
    colocarSemillas(CantiSemilla,TipoSemilla);
    allegro_funciones();

    return 0;
}
