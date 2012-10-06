/**
 *
 * Author: Juan Mompeán Esteban
 * Date: 06-10-12
 * Este programa trata de encontrar la mejor combinación
 * de jugadores a partir de un equipo dado. 
 * El equipo es de supermanager.acb.com
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<omp.h>
#include<string.h>
#include"util.h"

#define NUM_JUGADORES 11
#define NUM_BASES 36
#define NUM_ALEROS 108
#define NUM_PIVOTS 50
#define HELP " [-s máximo_dinero_sin_gastar]"

int * precios, * ePrecios;
char ** eNombres, ** nombres, ** jugadoresObligatorios;
double * eMedias, * eSube15, * sube15, * medias, PORCENTAJE_BROKER = 0.9, PORCENTAJE_MEDIA = 0.1;;
int njugadores, dineroTotal, dineroCaja, MAX_SIN_GASTAR = 50000, nobligatorios = 0;



int contarLineas(char * jugadores) {
  int lineas = 0;
  char * next = strchr(jugadores, '\n');
  while (next!=NULL) {
    lineas++;    
    next = strchr(next + 1, '\n');
  }
  return lineas;
}

char * leer(char * fichero) {
  FILE * f;
  f = fopen(fichero, "r");
  if (f!= NULL) {
    // obtain file size:
    fseek (f , 0 , SEEK_END);
    int lSize = ftell (f);
    rewind (f);

    // allocate memory to contain the whole file:
    char * buffer = (char*) malloc (sizeof(char)*lSize);

    // copy the file into the buffer:
    fread (buffer,1,lSize,f);
    fclose(f);
    return buffer;
  }
  return NULL;
}

void parsearJugadores(char * jugadores) {
  char * current = jugadores;
  int i;
  for (i=0; i < njugadores; i++) {
    char * sig = strchr(current, ';');
    nombres[i] = (char*) malloc (sizeof(char)* (sig - current + 1));
    memset(nombres[i], '\0', sizeof(char)* (sig - current + 1));
    strncpy(nombres[i], current, sig - current);
    current = sig + 1;
    sig = strchr(current, ';');
    medias[i] = strtod(current, NULL);
    current = sig + 1;
    sig = strchr(current, ';');
    sube15[i] = strtod(current, NULL);
    current = sig + 1;
    precios[i] = atoi(current);
    current = strchr(current, '\n') + 1;
  }
}

void parsearEquipo(char * equipo) {
  char * current = equipo;
  dineroTotal = atoi(current);
  current = strchr(current, '\n') + 1;
  dineroCaja = atoi(current);
  current = strchr(current, '\n') + 1;
  int i;
  for (i = 0; i < NUM_JUGADORES; i++) {
    char * sig = strchr(current, ';');
    eNombres[i] = (char*) malloc (sizeof(char)* (sig - current + 1));
    memset(eNombres[i], '\0', sizeof(char)* (sig - current + 1));
    strncpy(eNombres[i], current, sig - current);
    current = sig + 1;
    sig = strchr(current, ';');
    eMedias[i] = strtod(current, NULL);
    current = sig + 1;
    sig = strchr(current, ';');
    eSube15[i] = strtod(current, NULL);
    current = sig + 1;
    ePrecios[i] = atoi(current);
    current = strchr(current, '\n') + 1;
  }
}

double getValoracion(double * medias, double * sube15) {
	double resultado = 0;
	int i;
	for(i = 0; i < NUM_JUGADORES; i++) {
		if (medias[i] > 0) {
		  // Calcula un valor más estándar para la subida de la próxima semana
		  double distancia = (medias[i] - sube15[i]) / medias[i];
		  // La media
		  double media = medias[i];
		  // normaliza
		  distancia = distancia / dmax(distancia, media);
		  media = media / dmax(distancia, media);
		  // Valora tanto lo que va a subir en la próxima jornada como su media
			resultado += distancia * PORCENTAJE_BROKER + media * PORCENTAJE_MEDIA;
		}
	}
	return resultado;
}

int getCosteEquipo(int * precios) {
  int total = 0;
  int i;
  for (i = 0; i < NUM_JUGADORES; i++) {
    total += precios[i];
  }
  return total;
}

int estaEnEquipo(char * nombre, char ** equipo) {
  int i;
  for (i = 0; i < NUM_JUGADORES; i++) {
    if (strcmp(nombre, equipo[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

int contieneJugadoresObligatorios(int nobligatorios, char ** obligatorios, char ** equipo) {
  int i, j, contador = 0;
  for (i = 0; i < NUM_JUGADORES; i++) {
    for (j = 0; j < nobligatorios; j++) {
      if (strcmp(obligatorios[j], equipo[i]) == 0) {
        contador++;
        break;
      }
    }
  }
  return (contador == nobligatorios) ? 1 : 0;
}

int getDineroCaja(int * precios) {
  int i;
  int total = 0;
  for (i = 0; i < NUM_JUGADORES; i++) {
    if (precios[i] > 0)
      total+=precios[i];
  }
  return dineroTotal - total;
}


void copiarATemp(int ii, int jj, int kk, int i, int j, int k, double *tempMedias, double * tempSube15, int * tempPrecios, char ** tempNombres) {
  int index;
  for (index = 0; index < NUM_JUGADORES; index++) {
    tempMedias[index] = eMedias[index];
    tempSube15[index] = eSube15[index];
    tempPrecios[index] = ePrecios[index];
    tempNombres[index] = eNombres[index];
  }
  tempMedias[i] = medias[ii];
  tempMedias[j] = medias[jj];
  tempMedias[k] = medias[kk];
  tempSube15[i] = sube15[ii];
  tempSube15[j] = sube15[jj];
  tempSube15[k] = sube15[kk];
  tempPrecios[i] = precios[ii];
  tempPrecios[j] = precios[jj];
  tempPrecios[k] = precios[kk];
  tempNombres[i] = nombres[ii];
  tempNombres[j] = nombres[jj];
  tempNombres[k] = nombres[kk];
}

void solve() {
  int ii, jj, kk, i, j, k, jugadoresProcesados = 0, np, iam;
  double** mejorMedias, **mejorSube15, **tempMedias, **tempSube15;
  char *** mejorNombres, ***tempNombres;
  int ** mejorPrecios, **tempPrecios;
  #pragma omp parallel private(iam, ii, jj, kk, i, j, k) shared(mejorMedias, mejorSube15, mejorNombres, mejorPrecios, tempMedias, tempSube15, tempPrecios, nombres, jugadoresProcesados)
  {
    #if defined (_OPENMP)
      np = omp_get_num_threads();
      iam = omp_get_thread_num();
    #endif
    #pragma omp single
    {
      mejorMedias = (double **) malloc (sizeof(double*) * np);
      mejorSube15 = (double **) malloc (sizeof(double*) * np);
      mejorNombres = (char ***) malloc (sizeof(char**) * np);
      tempNombres = (char ***) malloc (sizeof(char**) * np);
      mejorPrecios = (int **) malloc (sizeof(int*) * np);
      tempMedias = (double **) malloc (sizeof(double*) * np);
      tempSube15 = (double **) malloc (sizeof(double*) * np);
      tempPrecios = (int **) malloc (sizeof(int*) * np);
    }
    printf("Hello from thread %d out of %d\n",iam,np);
    
    #pragma omp barrier
    
    mejorMedias[iam] = (double*) malloc (sizeof(double) * NUM_JUGADORES);
    mejorSube15[iam] = (double *) malloc (sizeof(double) *NUM_JUGADORES);
    mejorNombres[iam] = (char**) malloc (sizeof(char*) * NUM_JUGADORES);
    tempNombres[iam] = (char**) malloc (sizeof(char*) * NUM_JUGADORES);
    mejorPrecios[iam] = (int *) malloc (sizeof(int) * NUM_JUGADORES);
    tempMedias[iam] = (double *) malloc (sizeof(double) *NUM_JUGADORES);
    tempSube15[iam] = (double *) malloc (sizeof(double) *NUM_JUGADORES);
    tempPrecios[iam] = (int *) malloc (sizeof(int) *NUM_JUGADORES);
    
    for (k = 0; k < NUM_JUGADORES; k++) {
      mejorMedias[iam][k] = eMedias[k];
      mejorSube15[iam][k] = eSube15[k];
      mejorNombres[iam][k] = eNombres[k];
      mejorPrecios[iam][k] = ePrecios[k];
      tempNombres[iam][k] = eNombres[k];
    }

    #pragma omp for
    for (ii = 0; ii < njugadores; ii++) {
        if (medias[ii] == 0 | estaEnEquipo(nombres[ii], mejorNombres[iam])) {
          jugadoresProcesados++;
          continue; 
        }
      for (jj = 0; jj < njugadores; jj++) {
        if (ii == jj  | estaEnEquipo(nombres[jj], mejorNombres[iam]) | medias[jj] == 0) 
          continue;
        for (kk = 0; kk < njugadores; kk++) {
          if (ii == kk | jj == kk | medias[kk] == 0 | estaEnEquipo(nombres[kk], mejorNombres[iam]))
            continue;
          for (i = 0; i < NUM_JUGADORES; i++) {
            if ((i < 3 & ii > NUM_BASES) | ((i > 2 & i < 7) & (ii > NUM_ALEROS | ii < NUM_BASES))| ((i > 6 & i < 11) & ii < NUM_ALEROS))
              continue;
            for (j = 0; j < NUM_JUGADORES; j++) {
              if (i==j | ((j < 3 & jj > NUM_BASES) | ((j > 2 & j < 7) & (jj > NUM_ALEROS | jj < NUM_BASES)) | ((j > 6 & j < 11) & jj < NUM_ALEROS))) 
                continue;
              for (k = 0; k < NUM_JUGADORES; k++) {
                if (k==i | k==j | ((k < 3 & kk > NUM_BASES) | ((k > 2 & k < 7) & (kk > NUM_ALEROS | kk < NUM_BASES))| ((k > 6 & k < 11) & kk < NUM_ALEROS))) 
                  continue;
                copiarATemp(ii, jj, kk, i, j, k, tempMedias[iam], tempSube15[iam], tempPrecios[iam], tempNombres[iam]);
                if (contieneJugadoresObligatorios(nobligatorios, jugadoresObligatorios, tempNombres[iam]) & 
                    getDineroCaja(tempPrecios[iam]) < MAX_SIN_GASTAR & 
                    (getCosteEquipo(tempPrecios[iam]) <= dineroTotal) & 
                    (getValoracion(tempMedias[iam], tempSube15[iam]) > getValoracion(mejorMedias[iam], mejorSube15[iam]))) {
                  int index;
                  for (index = 0; index < NUM_JUGADORES; index++) {
                      mejorMedias[iam][index] = tempMedias[iam][index];
                      mejorSube15[iam][index] = tempSube15[iam][index];
                      mejorPrecios[iam][index] = tempPrecios[iam][index];
                    if (index == i) {
                      mejorNombres[iam][index] = nombres[ii];
                    } else if (index == j) {
                      mejorNombres[iam][index] = nombres[jj];
                    } else if (index == k) {
                      mejorNombres[iam][index] = nombres[kk];
                    } else {
                      mejorNombres[iam][index] = eNombres[index];
                    }
                  }
                }
              }
            }
          }
        }
      }
      jugadoresProcesados++;
      printf("Jugador %.2f%%\n", ((jugadoresProcesados*100.0) / njugadores));
    }
  
    #pragma omp single
    {
      // Coge el mejor de todos los buscados
      int i = 0;
      for (i = 1; i < np; i++) {
        if (contieneJugadoresObligatorios(nobligatorios, jugadoresObligatorios, mejorNombres[i]) & (getValoracion(mejorMedias[i], mejorSube15[i]) > getValoracion(mejorMedias[0], mejorSube15[0]))) {
          mejorMedias[0] = mejorMedias[i];
          mejorSube15[0] = mejorSube15[i];
          mejorNombres[0] = mejorNombres[i];
          mejorPrecios[0] = mejorPrecios[i];
        }
      }
      printf("\n\nY el equipo ganador...\n");
      for (i = 0; i < NUM_JUGADORES; i++) {
        printf("Nombre: %s;\t\tprecio: %d\t\tmedia: %f\t\tsube15: %f \n", mejorNombres[0][i], mejorPrecios[0][i], mejorMedias[0][i], mejorSube15[0][i]);
      }
      printf("\nCoste del equipo: %d; dinero en caja: %d\n", getCosteEquipo(mejorPrecios[0]), getDineroCaja(mejorPrecios[0]));
      printf("\nLo de equipo ganador era coña, eso se le preguntas a la güija :)\n");
      for (i=0; i<np;i++) {
        free(mejorNombres[i]);
        free(mejorMedias[i]);
        free(mejorSube15[i]);
        free(tempMedias[i]);
        free(tempSube15[i]);
        free(tempPrecios[i]);
        free(tempNombres[i]);
      }
      free (mejorNombres);
      free(mejorMedias);
      free(mejorSube15);
      free(tempMedias);
      free(tempSube15);
      free(tempPrecios);
      free(tempNombres);
    }
  }
  
  
}


void parsearArgumentos(int argc, const char * argv[]) {
  int i;
  for (i = 1; i < argc; i++) {
    if ((strlen(argv[i]) == 2) & (argv[i][0] == '-')) {
      switch(argv[i][1]) {
        case 'e':
          if (argc > (i+1)) {
            MAX_SIN_GASTAR = atoi(argv[i+1]);
            if (MAX_SIN_GASTAR < 1000) {
              printf("El mínimo permitido sin gastar es 1.000\n");
              exit(-1);
            }
            i++;
          }
          break;
        case 'h':
          printf("%s %s\n", argv[0], HELP);
          exit(-1);
          break;
        case 'b':
          if (argc > (i+1)) {
            PORCENTAJE_BROKER = strtod(argv[i+1], NULL);
            i++;
          }
          break;
        case 'm':
          if (argc > (i+1)) {
            PORCENTAJE_MEDIA = strtod(argv[i+1], NULL);
            i++;
          }
          break;
        case 'j':
          if (argc > (i+1)) {
            nobligatorios = count(argv[i+1], ';');
            jugadoresObligatorios = (char**) malloc (sizeof(char*) * nobligatorios);
            int j;
            char * current = (char*)argv[i+1], * anterior = (char*)argv[i+1];
            for (j = 0; j < nobligatorios; j++) {
              current = strchr(anterior, ';');
              jugadoresObligatorios[j] = (char *) malloc (sizeof(char) * (current-anterior+1));
              memset(jugadoresObligatorios[j], '\0', current-anterior+1);
              strncpy(jugadoresObligatorios[j], anterior, current - anterior);
              anterior = current + 1;
            }
            i++;
          }
          break;
      }
    }
  }
}

int main(int argc, const char* argv[]) {
  parsearArgumentos(argc, argv);
  char * jugadores = leer("jugadores");
  if (jugadores==NULL) {
    printf("Se produjo un error cargando los jugadores\n");
    exit(-1);
  } else {
    printf("Jugadores cargados\n");
  }
  char * equipo = leer("equipo");
  if (jugadores==NULL) {
    printf("Se produjo un error cargando el equipo\n");
    exit(-1);
  } else {
    printf("Equipo cargado\n");
  }
  njugadores = contarLineas(jugadores);
  printf("Número de jugadores: %d \n", njugadores);
  nombres = (char**) malloc (sizeof(char*) * njugadores);
  medias = (double *) malloc(sizeof(double) * njugadores);
  sube15 = (double *) malloc (sizeof(double) * njugadores);
  precios = (int *) malloc (sizeof(int) * njugadores);
  eNombres = (char**) malloc (sizeof(char*) * NUM_JUGADORES);
  eMedias = (double *) malloc(sizeof(double) * NUM_JUGADORES);
  eSube15 = (double *) malloc (sizeof(double) * NUM_JUGADORES);
  ePrecios = (int *) malloc (sizeof(int) * NUM_JUGADORES);
  parsearJugadores(jugadores);
  parsearEquipo(equipo);
  solve();
  int i;
  for (i=0; i < njugadores; i++) {
    free(nombres[i]);
  }
  free(nombres);
  free(medias);
  free(sube15);
  free(precios);
  for (i=0; i < NUM_JUGADORES; i++) {
    free(eNombres[i]);
  }
  free(eNombres);
  free(eMedias);
  free(eSube15);
  free(ePrecios);
  free(jugadores);
  free(equipo);
  return 0;
}
