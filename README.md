# Pac-MAN


## Resumen Ejecutivo
Para este proyecto se pidió desarrollar el juego Pac-man con algunas va-
riantes respecto al juego original,donde este como lo conocemos va a tener
al héroe y a los cuatro fantasmas, donde Inky va a moverse aleatoriamente y
los tres restantes van a tener una inteligencia diferente, siempre van a tratar
de perseguir a Pacman buscando el camino más corto, estos tres fantasmas
van a tener esta función en común pero cada uno tiene caracterı́sticas únicas
que los diferencian.
Este proyecto fue desarrollado en C por lo que decidimos utilizar la bi-
blioteca Allegro 5 ya que esta se utiliza para desarrollar videojuegos y trae
muchas funciones que facilita por ejemplo mostrar en pantalla el mapa con
imágenes y los diferentes personajes. Esta biblioteca nos ayudó a lograr re-
conocer las teclas para permitir mover el pacman, poder representar a cada
fantasma con su imagen y para poder mostrar el mapa decidimos crear un
ciclo donde si es una C significa un coco, la S una semilla,  ̃ representa las
paredes y los espacios en blanco es la cueva de los fantasmas, de este modo
por medio del ciclo según cada sı́mbolo se van a representar en el tablero. El
pacman cuenta con 5 vidas y todos los personajes son hilos independientes.
Al igual que los proyectos anteriores se realizaron pruebas, pero estas
son con diferente cantidad de semillas si estas están distribuidas de manera
uniforme o aleatoria, también se muestran pruebas donde se puede ver la
velocidad de los personajes, cuando pacman come una semilla los fantasmas
cambian de color y escapan de pacman. Se puede concluir de estas pruebas
que según la cantidad de semillas que se ponga se va a tener más oportuni-
dades para pasar al siguiente nivel.


## Installation - Allegro5
Allegro 5 es una biblioteca libre de código abierto para la programación
de videojuegos desarrollada en lenguaje de programación C que cuenta con
muchas funciones muy útiles para gráficos, manipulación de imágenes, dis-
positivos de entrada y entre otras. Por otro lado, esa librerı́a es compatible
con diferentes sistema operativos incluyendo los de móviles IOS y Android.

Primero que todo, se debe agregar el PPA de Allegro:
```bash
sudo add-apt-repository ppa:allegro/5.2
```
Luego, se usa el siguiente comando para realizar la instalación:
```bash
sudo apt-get install liballegro*5.2 liballegro*5-dev
```

# Ejecución
```bash
make
```
```bash
./barcherod
```

# Video del proyecto
En ese link , se puede ver cuando el Pac-Man come la semilla, los fantas-
mas se cambian de color y salen huyendo a la cueva. Del mismo modo, al final
del video se puede ver como los fantasmas tratan de bloquear al Pac-Man.
https://www.youtube.com/watch?v=A0nuf2ZoDws&feature=youtu.be#Old_Drum


## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)
