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

    ALLEGRO_COLOR bolita = al_map_rgb(44,117,255);

    ALLEGRO_KEYBOARD_STATE keyState;
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);  //30 frames per segundo
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(880,600);
    al_set_window_title(disp,"Pac-Man");
    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_COLOR electricBlue = al_map_rgb(44,117,255);

    bool done = false, draw=true;
    int x = 5, y = 5;
    int moveSpeed = 5;
    int dir = DOWN;
    int state = 0;


    if(!al_init_image_addon())
    {
        printf("couldn't initialize image addon\n");
        return 1;
    }
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    ALLEGRO_BITMAP* bmp = al_create_bitmap(880,600);
    ALLEGRO_BITMAP* roca = al_load_bitmap("roca2.jpg");
    ALLEGRO_BITMAP* punto = al_load_bitmap("punto2.png");
    
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue,al_get_display_event_source(disp));

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
      }

      if(event.type == ALLEGRO_EVENT_TIMER){
        al_get_keyboard_state(&keyState);
        if(al_key_down(&keyState,ALLEGRO_KEY_DOWN))
            y+=moveSpeed;
        else if(al_key_down(&keyState,ALLEGRO_KEY_UP))
            y-=moveSpeed;
        else if(al_key_down(&keyState,ALLEGRO_KEY_RIGHT))
            x+=moveSpeed;
        else if(al_key_down(&keyState,ALLEGRO_KEY_LEFT))
            x-=moveSpeed;         
        draw=true;

      }
      if(draw){
        al_draw_rectangle(x ,y , x+30 , y+30,electricBlue,2.0);
        al_flip_display();  
        al_clear_to_color(al_map_rgb(0, 0, 0));  
       }
    }
      

    
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}

