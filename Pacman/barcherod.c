/*
Instituto Tecnológico de Costa Rica
IC4700 - Lenguajes de Programación
II Semestre 2020
Profesor: Eddy Ramírez
Proyecto3 - Estructurado
Estudiantes: Te Chen Huang 
             María José Barquero Pérez
			       Kendall Rodríguez Mora
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


#define maxfil 20 
#define maxcol  29
#define inf 999999
#define num 98989
#define nodos 240
#define maxarreglo 240
#define maxi 4

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



pthread_mutex_t semc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semf = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t semp = PTHREAD_MUTEX_INITIALIZER; 

//En esta estructura se almacenará los datos de los fantasmas
struct info_fant{
  int xf;  //Posición x de fantasma
  int yf;  //Posición y de fantasma
  int id;  //Id de fantasma (0-3)
  int dirf; //Dirección de fantasma
};

//up=0,down=1,left=1,right=2;
enum Direction {up,down,left,right};

//En esta estructura se almacenará los datos de los fantasmas
struct info_fant B = {420,180,0,right};  //Blinky
struct info_fant I = {390,270,1,up};     //Inky
struct info_fant P = {420,270,2,down};   //Pinky
struct info_fant C = {450,270,3,left};   //Clyde

//En esta estructura se almacenará los datos de aquellos nodos que están conectados con al menos un nodo
struct NodoFW{
    int indice; //Índice de nodo (0-579)
    int numNodo; //Número nodo (0-239) en total hay 240 nodos que están conectados con al menos uno
    int posx; //Posición x
    int posy; //Posición y 
    int Arr_Pos[maxi]; //Un arreglo que contienen con cuáles nodos está relacionado
};

char mapa[maxfil][maxcol]; //Matriz del tablero
int matriz_recorrido[nodos][nodos]; //Matriz de recorrido
int dist[nodos][nodos]; //Matriz de distancia
int Camino[100]; //Arreglo que se va a almacenar la ruta más corta entre Pacman y Pinky
int inteligencia[100]; //Arreglo que se va a almacenar la ruta de blinky y Clyde
struct NodoFW arr_nodofw[maxarreglo]; //Arreglo donde se va a almacenar los 240 nodos (que está conectado con al menos uno)

int CantiSemilla;
char TipoSemilla[12];
int x=420,y=510,ax,ay;                         //Posiciones donde empieza pacman
float speed,speedClyde,speedFants,speedPacman; //Velocidades
int CantMovimiento=0,moveSpeed=30,vidas=5,dir = down;
bool done=false,escapan=false,estadoTablero=true,estadoFantasma=false;
//Posiciones donde se van colocar las semillas
int NumCoordenadas[32] = {1,2,17,27,10,8,4,15,6,26,13,3,17,5,1,22,13,19,3,5,6,12,9,26,15,15,17,18,8,1,18,2};

pthread_mutex_t semc;
pthread_mutex_t semf;
pthread_mutex_t semp;

//Mapa original
// c = cocos, ~ = pared, | = cocos, s = semillas 
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

//Función: Verificar tablero
//Dominio: No recibe ningún parámetro
//Codominio: Retorna 1 si el Pacman ya se comió todas las semillas y en el caso contrario 0
//En esta función se verifica si aún queda semilla en el tablero 
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

//Función: llenarTablero
//Dominio: No recibe ningún parámetro
//Codominio: En esta función se llena la matriz de mapa usando la matriz original
void llenarTablero(){
  for(int i=0;i<maxfil;i++){
    for(int j=0;j<maxcol;j++){
      mapa[i][j]=mapa_original[i][j];
    }
  }
}

//Función: llenarArreglo
//Dominio: No recibe ningún parámetro
//Codominio: En esta función se verifica cuáles son los nodos que están conectados con
//		       al menos un nodo, y se almacenará sus datos.  Por otro lado, se pone -1
// 			     en la matriz de recorrido cuando no se sabe cuál es la distancia entre los nodos
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

//Función: retornarpos
//Dominio: Recibe un número entero, que es el índice del nodo (0-579)
//Codominio: Retorna un número entero, que buscará cuál es el número que lo representa (0-239) 
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
:) Es mejor XD
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

//Función: inicializar_matriz_distancia
//Dominio: No recibe ningún parámetro
//Codominio: En esta función, inicializa la matriz de adyacencia
//           Se pone 0 cuando la distancia es del propio nodo
//           En este caso, se pone 1 en aquellos nodos que están conectados
//           Se pone inf, si no están relacionados
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

//Función: floyd_warshall
//Dominio: No recibe ningún parámetro
//Codominio: En este caso el conjunto de vértices esta numerados de 0 a 239 
// y existe dos opciones para recorrerlo, el primero sería solo utilizar los 
// vértices del conjunto 1 a k, y luego se verifica desde i hasta k+1, y 
// finalmente de j hasta k, y se verifica si la distancia actual dist[i][j] 
// es mayor que la distancia dist[i][k] + dist[k][j], si es verdad se actualiza en  DIST[i][j]
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

//Función: encontrar_ruta
//Dominio: Recibe dos números enteros ini y fin
//Codominio: Busca el camino más corto entre ini y fin y lo almacena en el arreglo camino
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

//Función: encontrar_ruta_inteligencia
//Dominio: Recibe dos números enteros ini y fin
//Codominio:Busca el camino más corto entre ini y fin y lo almacena en el arreglo camino
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

//Función: colocarSemillas
//Dominio: Recibe un número entero y la manera de como se va a distribuir las semillas (normal y aleatorio)
//Codominio: En el caso de normal, se utilizará el arreglo de posiciones para colocarlas, y en el otro se van ir generando
//           números aletorios e ir verificando si se puede colocarla o no 
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

//Función: dibujar_mapa
//Dominio: Recibe 3 BITMAP (roca,coco,semilla)
//Codominio: '~' representa pared, 'c' y '|' representan coco y 's' es semilla
//           Cada vez que el Pacman se come una semilla, el tablero se va a ir
//           actualizando y se reemplaza por un espacio 
void dibujar_mapa(ALLEGRO_BITMAP *r,ALLEGRO_BITMAP *c,ALLEGRO_BITMAP *s){
    for(int i = 0; i< maxfil; i++)
      for(int j = 0; j< maxcol; j++)
        if(mapa[i][j] == '~'){
            al_draw_bitmap_region(r,0,0,30,30,j*30,i*30,0);                         
        }else if(mapa[i][j] == 'c' ||  mapa[i][j] == '|'){
            al_draw_bitmap_region(c,0,0,30,30,j*30,i*30,0);
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

//Función: dibujar_pacman
//Dominio: No recibe ningún parámetro 
//Codominio: En esta función, se dibuja el Pacman según la dirección 
void dibujar_pacman(){
    if(dir == 0){ //arriba
        al_draw_bitmap(pacmanArriba,x,y,0);      
    }else if(dir == 1){ //abajo
        al_draw_bitmap(pacmanAbajo,x,y,0);
    }else if(dir == 2){ //Izq
        al_draw_bitmap(pacmanIzq,x,y,0);
    }else if(dir == 3){ //Der
        al_draw_bitmap(pacmanDer,x,y,0);
    }else{
        al_draw_bitmap(pacmanDer,x,y,0);
    }
}

//Función: dibujar_fantasma
//Dominio: Recibe un BITMAP que representa cuál es el fantasma, y las posiciones de los fantasmas
//Codominio: En esta función, dibujan los fantasmas según las posiciones
//           Sí el estado de los fantasmas es true, entonces se cambian al color azul para indicarle al
//           usuario que ya puede comerselos
void dibujar_fantasma(ALLEGRO_BITMAP *pm, int xf,int yf){
  if(estadoFantasma == true){
      al_draw_tinted_bitmap(pm, al_map_rgba_f(1, 0, 1, 1), xf, yf, 0);
    }else{
       al_draw_tinted_bitmap(pm, al_map_rgba_f(1, 1, 1, 1), xf, yf, 0);       
    }
}

//Función: volver_casita
//Domonio: Recibe la posición del fantasma y su identificación
//Codominio: En esta función según el id del fantasma va a calcular el inicio y destino del camino que debe recorrer
//           donde según cada fantasma se devuelven a la posición inicial, donde por ejemplo 78 es 6,14 para blinky, 
//           por medio del while se va a recorrer todo el camino hasta su destino, y se aumenta su velocidad en un 120%
//           esto lo realizamos multiplicando la velocidad original por 1.2. 
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

//Función: mover_inky
//Dominio: No recibe ningún parámetro 
//Codominio: Esta función permite que Inky se mueva aleatoriamente, para evitar que el fantasma se quede pegado
//           se pone en varios lugares del mapa un "|" esto permite que cambie su posición también
//           según la dirrección en la que se encuentre el fantasma va a realizar la validación de si puede subir, 
//           bajar, derecha o izquierda, con el rand()%4 lo que hacemos es elegir un número aleatorio entra 0 y 3
//           y así Inky se va a mover aleatoriamente
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

//Función: choque_camino
//Dominio: No recibe ningún parámetro
//Codominio: En esta función, se verifica si el Pacman logra perseguir y comer a los fantasmas.
void choque_camino(){
  if(CantMovimiento!=0){
    if ((B.xf == x && B.yf == y) || (B.xf == ax && B.yf == ay))
       volver_casita(B.xf,B.yf,0);
      
    if((I.xf == x && I.yf == y) || (I.xf == ax && I.yf == ay))
       volver_casita(I.xf,I.yf,1);
    
    if((P.xf == x && P.yf == y) || (P.xf == ax && P.yf == ay))
       volver_casita(P.xf,P.yf,2);

    if ((C.xf == x && C.yf == y) || (C.xf == ax && C.yf == ay))
       volver_casita(C.xf,C.yf,3);
    
  }else{
      estadoFantasma = false;
  }
}


//Función: inteligencia_fantasma
//Dominio: Recibe tres números enteros, el nodo inicio y nodo final, y el id de fantasma
//Codominio: En esta función, predice los movimientos de Pinky, e intenta bloquear la otra salida 
//           Primero se verifica si el pacman está en una posición vertical o horizontal, y después verifica
//           a cuál dirección se va mover para poder sacar la posición contraria
void inteligencia_fantasma(int inicio,int destino,int fantasma){
    int py=y,px=x;
    encontrar_ruta_inteligencia(destino,inicio);    
    int direccion,inicio1,destino1,contador=0;

    //Si x y y son iguales, se usará la posición anterior del Pacman para verificarlo
    if(arr_nodofw[inteligencia[0]].posy == arr_nodofw[inteligencia[1]].posy == arr_nodofw[inteligencia[1]].posx){
        destino = retornarpos((ay/30) * maxcol + (ax/30)); 
        py=ay; px=ax;
        encontrar_ruta_inteligencia(destino,inicio);
    }

    //Utilizando la ruta más corta de Pinky, y se verifica cuál sería la dirección que debe usar Blinky y Clyde para 
    //bloquear a Pacman en la otra salida
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

    //Calcular la ruta más corta para bloquear al Pacman en la otra salida
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

//Función: Mover_fantasmas
//Dominio: No recibe ningún parámetro
//Codominio: Es la función principal donde se encarga de que cada fantasma tenga sus características por aparte
//           cuando escapan es igual a true significa que deben escapar de pacman, por lo que se cambia el destino por volve a la casita
//           si no deben escapar, entonces segpun cada fantasma llama a la función correspondiente, clyde es el número 3, tiene una velocidad de un 80%
//           y tiene la misma inteligencia que blinky, blinky es el número 0 va a la misma velocidad que inky y pinky,
//           pinky es el número 1, y este siempre va a buscar a pacman por la ruta más corta, y por último inky es el número 2
//           llama a la función de mover_inky, con mutex permite que los fantasmas tengan la velocidad que deseamos 
void * mover_fantasmas(void *entrada){
     
    int xi = ((struct info_fant*)entrada)->xf;
    int yj = ((struct info_fant*)entrada)->yf;
    int fantasma = ((struct info_fant*)entrada)->id;

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

//Función: teclas
//Dominio: No recibe ningún parámetro
//Codominio: En esta función,lee las teclas y el Pacman se mueve según la dirección 
void * teclas(void * param){
    pthread_mutex_lock(&semp);  
    al_get_keyboard_state(&keyState);
    if(al_key_down(&keyState,ALLEGRO_KEY_DOWN)){
      dir = down;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_UP)){
      dir = up;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_RIGHT)){
      dir = right;
    }else if(al_key_down(&keyState,ALLEGRO_KEY_LEFT)){
      dir=left;         
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

//Función: perdio
//Dominio: No recibe ningún parámetro
//Codominio: En esta función,cada vez que el Pacman choca contra los fantasma, se devuelve a la cueva y la posición original
//           del Pacman y se le restará una vida. Cuando ya la vida es 0, el juego termina 
void perdio(){
  if(estadoFantasma==false){
    if (vidas == 0) done = true;

    if ((B.xf == x && B.yf == y)||(I.xf == x && I.yf == y)||(P.xf == x && P.yf == y)||(C.xf == x && C.yf == y) ||
        (B.xf == ax && B.yf == ay)||(I.xf == ax && I.yf == ay)||(P.xf == ax && P.yf == ay)||(C.xf == ax && C.yf == ay)){
         B.xf=420; B.yf=180; I.xf=390; I.yf=270;
         P.xf=420; P.yf=270; C.xf=450; C.yf=270;
         x = 420; y = 510;
         vidas -=1;     
    }
  }
}

//Función: allegro_funciones
//Dominio: No recibe ningún parámetro
//Codominio: Se encarga de iniciar todas las funciones relacionadas a la biblioteca allegro
//           Se crean los hilos, se cargan las imágenes que van a representar a los personajes
//           Se crea un while donde permite que el juego se vaya ejecutando, y cuando este termine
//           poder salirse del juego si perdemos 
void allegro_funciones(){
    float tiempo = 60.0;
    //Incializar 
    al_init();
    al_install_keyboard();
    al_install_audio();        
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_acodec_addon();
    
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
    
    //Importar las imagenes
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

    al_reserve_samples(1);
    ALLEGRO_SAMPLE * background = al_load_sample("background.ogg");
    al_play_sample(background, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, 0);
    
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
        ax = x; ay = y;
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
   		
   		//Reinciar el tablero
        if(verificar_tablero()==1){
           speedClyde*=1.1;
           speedFants*=1.1;  
           llenarTablero();
           colocarSemillas(CantiSemilla,TipoSemilla);
           B.xf=420; B.yf=180; I.xf=390; I.yf=270;
           P.xf=420; P.yf=270; C.xf=450; C.yf=270;
           x = 420; y = 510;
           CantMovimiento=0;
        }

        al_flip_display();
        al_clear_to_color(al_map_rgb(0, 0, 0));  
        
       }
    } 
    al_destroy_sample(background);
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

//Main
//Dominio: No recibe parámetros
//Codominio: Lee los datos del archivo txt donde vienen las variables para saber 
//           la velocidad de fantasmas y pacman, cantidad de semillas, y si estas se van a distrubir 
//           normal o aleatoriamente, antes de iniciar el juego se crean todos los caminos de las rutas 
//           más cortas y luego empieza el juego 
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
