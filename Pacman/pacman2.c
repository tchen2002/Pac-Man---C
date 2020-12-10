#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
ALLEGRO_BITMAP *pacman;
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

bool done = false, draw=true,active=false;
int x = 30, y = 30;  
int moveSpeed = 30;
int dir = DOWN; 
int state = 0;
int sourceX=0,sourceY=0;
struct info B = {420,180,0,RIGHT};
struct info I = {390,270,1,UP};
struct info P = {420,270,2,DOWN};
struct info C = {450,270,3,LEFT};

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

void dibujar_mapa(ALLEGRO_BITMAP *r,ALLEGRO_BITMAP *p){
    for(int i = 0; i< MAXFILAS; i++)
      for(int j = 0; j< MAXCOLS; j++)
        if(mapa[i][j] == '~'){
            al_draw_bitmap_region(r,0,0,30,30,j*30,i*30,0);                         
        }else if(mapa[i][j] == 'c' ||  mapa[i][j] == '|'){
            al_draw_bitmap_region(p,0,0,30,30,j*30,i*30,0);
            if(( (y/30) == i ) && ( (x/30) ==j))
                mapa[i][j] = ' ';
        }
}

void dibujar_pacman(){
    al_draw_bitmap_region(pacman,sourceX,sourceY,30,30,x,y,0);
}

void dibujar_fantasma(ALLEGRO_BITMAP *pm, int xf,int yf){
    al_draw_bitmap(pm,xf,yf,0);
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
    active = true;
    if(al_key_down(&keyState,ALLEGRO_KEY_DOWN)){
      dir = DOWN;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_UP)){
      dir = UP;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_RIGHT)){
      dir = RIGHT;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_LEFT)){
      dir=LEFT;         
    }else{
      active=false;
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


    if(active==true)
        sourceX += al_get_bitmap_width(pacman)/4;
    else sourceX =30;
    
    if(sourceX>=al_get_bitmap_width(pacman))    
      sourceX=0;
    draw=true;
}

void allegro_funciones(){
    
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_image_addon();
    pthread_mutex_init(&sem, 0);
    pthread_mutex_init(&sem, 0);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_TIMER* timerClyde = al_create_timer(1.0 / 48.0);
    ALLEGRO_TIMER* timerPacman = al_create_timer(1.0 / 200.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(880,600);
    al_set_window_title(disp,"Pac-Man"); //nombre de la ventana
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);

    ALLEGRO_BITMAP* bmp = al_create_bitmap(880,600);
    roca = al_load_bitmap("roca.jpg");
    punto = al_load_bitmap("punto.png");
    pacman = al_load_bitmap("pacman.png");
    blinky = al_load_bitmap("Blinky.png");
    inky = al_load_bitmap("Inky.png");
    pinky = al_load_bitmap("Pinky.png");
    clyde = al_load_bitmap("Clyde.png");
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_timer_event_source(timerPacman));
    al_register_event_source(queue, al_get_timer_event_source(timerClyde));

    ALLEGRO_EVENT event;
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
        switch(event.keyboard.keycode){
          case ALLEGRO_KEY_ESCAPE:
            done=true;
            break;
        }
      }else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
          done = true;
          break;
      }

      if(event.type == ALLEGRO_EVENT_TIMER){
        al_draw_bitmap(bmp , 0 , 0 , 0);
        dibujar_mapa(roca,punto);
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
    al_destroy_bitmap(pacman);
    al_destroy_bitmap(punto);
    al_destroy_bitmap(blinky);
    al_destroy_bitmap(inky);
    al_destroy_bitmap(pinky);
    al_destroy_bitmap(clyde);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}

int main(){
    allegro_funciones();
    return 0;
}
