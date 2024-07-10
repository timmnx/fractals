#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <SDL2/SDL.h>

#define X 800
#define Y 800

void SDL_SortieAvecErreur(const char *message);

double afficher(double a, double b, double c_re, double c_im, int m)
{
    double Re = a;
    double Im = b;
    bool loop = true;
    int n = 0;
    int borne = 10000;
    double Re_n, Im_n;
    while (loop)
    {
        Re_n = Re;
        Im_n = Im;
        Re = Re_n * Re_n - Im_n * Im_n + c_re;
        Im = 2 * Re_n * Im_n + c_im;
        n++;
        if (Re * Re + Im * Im > borne || n > m)
        {
            loop = false;
        }
    }
    double c = 0.;
    if (n < m)
    {
        double lambda = 10000;
        double alpha = -lambda / 767;
        c = 768 - (lambda / (n - alpha));
    }
    return c;
}

typedef struct
{
    /* data */
    int i_deb;
    int j_deb;
    int i_fin;
    int j_fin;
    double centre_x;
    double centre_y;
    double zoom;
    int ***color;
    int max_it;
    double c_re;
    double c_im;
} param;

void *compute(void *args)
{
    param *p = (param *)args;
    for (int i = p->i_deb; i < p->i_fin; i++)
    {
        for (int j = p->j_deb; j < p->j_fin; j++)
        {
            double re = ((double)i - p->centre_x) / p->zoom;
            double im = -((double)j - p->centre_y) / p->zoom;
            double c_coeff = afficher(re, im, p->max_it, p->c_re, p->c_im);
            if (c_coeff != 0)
            {
                p->color[i][j][0] = (int)c_coeff % 256;
                p->color[i][j][1] = 127 * (cosf(c_coeff / 64) + 1);
                p->color[i][j][2] = 127 * (sinf(c_coeff / 64) + 1);
            }
            else
            {
                p->color[i][j][0] = 0;
                p->color[i][j][1] = 0;
                p->color[i][j][2] = 0;
            }
        }
    }
    return NULL;
}

int main(void)
{
    // Variables
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialisation
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("ERREUR : Inititalisation SDL > %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Création de la fenêtre et du rendu
    window = SDL_CreateWindow("Fractales",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              X, Y,
                              0);
    if (window == NULL)
    {
        SDL_SortieAvecErreur("Creation fenetre echouee");
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_SortieAvecErreur("Creation rendu echoue");
    }

    /* ----------------------------------------------------------------------- */

    int max_it = 100;
    double centre_x = 700.;
    double centre_y = 400.;
    double zoom = 300.;

    bool changes = true;
    bool running = true;

    double *re_test = malloc(sizeof(double));
    double *im_test = malloc(sizeof(double));

    int ***color = malloc(X * sizeof(int **));
    for (int i = 0; i < X; i++)
    {
        color[i] = malloc(Y * sizeof(int *));
        for (int j = 0; j < Y; j++)
        {
            color[i][j] = malloc(3 * sizeof(int));
            color[i][j][0] = 0;
            color[i][j][1] = 0;
            color[i][j][2] = 0;
        }
    }

    pthread_t tab_id[4];
    param tab_args[4];

    double r = 1.;
    double d_r = .1;
    double theta = 180.;
    double d_theta = 1.;

    SDL_Event event;
    while (running)
    {
        // Process events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    centre_x = event.motion.x * (1 - 2) + 2 * centre_x;
                    centre_y = event.motion.y * (1 - 2) + 2 * centre_y;
                    zoom = 2 * zoom;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    centre_x = event.motion.x * (1 - 0.5) + 0.5 * centre_x;
                    centre_y = event.motion.y * (1 - 0.5) + 0.5 * centre_y;
                    zoom = 0.5 * zoom;
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    theta -= d_theta;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    theta += d_theta;
                }
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    if (r > 0)
                        r -= d_r;
                }
                else if (event.key.keysym.sym == SDLK_UP)
                {
                    r += d_r;
                }
            }
            changes = true;
        }
        if (changes)
        {
            if (theta > 180)
            {
                theta -= 360;
            }
            else if (theta < -180)
            {
                theta += 360;
            }

            tab_args[0] = (param){0, 0, X / 2, Y / 2, centre_x, centre_y, zoom, color, r * cos(theta * M_PI / 180), r * sin(theta * M_PI / 180), max_it};
            tab_args[1] = (param){X / 2, 0, X, Y / 2, centre_x, centre_y, zoom, color, r * cos(theta * M_PI / 180), r * sin(theta * M_PI / 180), max_it};
            tab_args[2] = (param){0, Y / 2, X / 2, Y, centre_x, centre_y, zoom, color, r * cos(theta * M_PI / 180), r * sin(theta * M_PI / 180), max_it};
            tab_args[3] = (param){X / 2, Y / 2, X, Y, centre_x, centre_y, zoom, color, r * cos(theta * M_PI / 180), r * sin(theta * M_PI / 180), max_it};
            for (int k = 0; k < 4; k++)
            {
                pthread_create(&tab_id[k], NULL, compute, &tab_args[k]);
            }
            for (int k = 0; k < 4; k++)
            {
                pthread_join(tab_id[k], NULL);
            }
            for (int i = 0; i < X; i++)
            {
                for (int j = 0; j < Y; j++)
                {
                    SDL_SetRenderDrawColor(renderer, color[i][j][0], color[i][j][1], color[i][j][2], 255);
                    SDL_RenderDrawPoint(renderer, i, j);
                }
            }
            SDL_RenderPresent(renderer);
            changes = false;
        }
    }

    /* ----------------------------------------------------------------------- */

    // Fin du programme
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(re_test);
    free(im_test);
    return EXIT_SUCCESS;
}

void SDL_SortieAvecErreur(const char *message)
{
    SDL_Log("ERREUR : %s > %s\n", message, SDL_GetError());
}
