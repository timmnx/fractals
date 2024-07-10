FRACTAL :
Affiche une fractale de Julia, avec la relation de récurrence z_{n+1} = z_n^2 + c,
avec c étant défini dans le programme comme le nombre complexe de module r et
d'argument theta.

COMPILATION :
gcc -O3 fractals.c -pthread $(sdl2-config --cflags --libs) -o fractals

INTERACTION :
- Clique gauche : zoom avant sur le pointeur de la souris
- Clique droit : zoom arrière sur le pointeur de la souris
- Flèche HAUT : augmente r de d_r
- Flèche BAS : diminue r de d_r
- Flèche GAUCHE : diminue theta de d_theta
- Flèche DROIT : augmente theta de d_theta
