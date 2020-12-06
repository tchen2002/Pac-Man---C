#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define MAXFILAS 20
#define MAXCOLS  31

ALLEGRO_BITMAP *bmp;
ALLEGRO_BITMAP *roca;
ALLEGRO_BITMAP *punto;

char mapa[MAXFILAS][MAXCOLS]={ 
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
  "~ccccccccccc~~~~~ccccccccccc~",
  "~c~~~c~~~~~c~~~~~c~~~~~c~~~c~",
  "~c~~~c~~~~~c~~~~~c~~~~~c~~~c~",
  "~ccccccccccccccccccccccccccc~",
  "~c~~~c~~c~~~~~~~~~~~c~~c~~~c~",
  "~ccccc~~ccccccccccccc~~ccccc~",
  "~c~~~c~~c~~~~   ~~~~c~~c~~~c~",
  "~c~~~c~~c~~~     ~~~c~~c~~~c~",
  "cccccc~~c~~~     ~~~c~~cccccc",
  "~c~~~c~~c~~~~~~~~~~~c~~c~~~c~",
  "~c~~~c~~ccccccccccccc~~c~~~c~",
  "~c~~~c~~~~~~c~~~c~~~~~~c~~~c~",
  "~ccccc~~ccccc~~~ccccc~~ccccc~",
  "~c~~~c~~c~~~~~~~~~~~c~~c~~~c~",
  "~o~~~ccccccccccccccccccc~~~c~",
  "~c~~~c~~~~c~~~~~~~~c~~~c~~~c~",
  "~c~~~c~~~~cccccccccc~~~c~~~c~",
  "~cccccccccc~~~~~~~~ccccccccc~",
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
};




int main(){
    enum Direction {UP,DOWN,LEFT,RIGHT};
    
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_KEYBOARD_STATE keyState;
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(880,600);
    al_set_window_title(disp,"Pac-Man");
    ALLEGRO_FONT* font = al_create_builtin_font();

    bool done = false, draw=true,active=false;
    int x = 30, y = 30;
    int moveSpeed = 15;
    int dir = DOWN;
    int state = 0;
    int sourceX=0,sourceY=0;


    if(!al_init_image_addon())
    {
        printf("couldn't initialize image addon\n");
        return 1;
    }
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP* bmp = al_create_bitmap(880,600);
    ALLEGRO_BITMAP* roca = al_load_bitmap("roca2.jpg");
    ALLEGRO_BITMAP* punto = al_load_bitmap("punto2.png");
    ALLEGRO_BITMAP* pacman = al_load_bitmap("pacman1.png");
    
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;
    al_hide_mouse_cursor(disp);
    al_start_timer(timer);
    if(!roca)
    {
    printf("couldn't load mysha\n");
    return 1;
    }
    if(!punto)
    {
    printf("couldn't load mysha\n");
    return 1;
    }

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
          if(mapa[(y-30)/30][x/30] != '~'){
            y-=moveSpeed;
          }else dir = 4;
        }
        if(dir == 1){ //down
          if(mapa[(y+30)/30][x/30] != '~'){
            y+=moveSpeed;
          }else dir = 4;
        } 
        if(dir == 2){ //left
          if(mapa[y/30][(x-30)/30] != '~'){
            x-=moveSpeed;
          }else dir = 4;
        } 
        if(dir == 3){ //right
          if(mapa[y/30][(x+30)/30] != '~'){
            x+=moveSpeed;
          }else dir = 4;
        }      
        
        //Rutine para atajo
        if(x <= -30)
          x=870;
        else if(x >= 870)
          x=-30;

        if(active==true){
            sourceX += al_get_bitmap_width(pacman)/4;
        }else{
          sourceX =28;
        }
        if(sourceX>=al_get_bitmap_width(pacman)){
          sourceX=0;
        }
        draw=true;
      }

      if(draw){

        //al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
        al_draw_bitmap(bmp , 0 , 0 , 0);
        for(int i = 0; i< MAXFILAS; i++)
          for(int j = 0; j< MAXCOLS; j++)
            if(mapa[i][j] == '~'){
                al_draw_bitmap_region(roca,0,0,30,30,j*30,i*30,0);                         
            }else if(mapa[i][j] == 'c'){
                al_draw_bitmap_region(punto,0,0,30,30,j*30,i*30,0);
                if(( (y/30) == i ) && ( (x/30) ==j)){
                  mapa[i][j] = ' ';
                }
            }
  
        al_draw_bitmap_region(pacman,sourceX,sourceY,28,28,x,y,0);
        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));  
       }
    }        
    
    al_destroy_bitmap(roca);
    al_destroy_bitmap(pacman);
    al_destroy_bitmap(punto);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}