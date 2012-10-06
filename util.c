#include<string.h>
/**
 *
 * Author: Juan Mompeán Esteban
 * Date: 06-10-12
 * Funciones de propósito general.
 *
 */

// a definition mentioning inline
inline int max(int a, int b) {
  return a > b ? a : b;
}

inline int min(int a, int b) {
  return a < b ? a : b;
}

// a definition mentioning inline
inline double dmax(double a, double b) {
  return a > b ? a : b;
}

inline double dmin(double a, double b) {
  return a < b ? a : b;
}
/**
 * 
 * Cuenta el número de
 *
 */
int count(char * string, char caracter) {
  int count = 0;
  char * next = strchr(string, caracter);
  while (next != NULL) {
    count++;
    next = strchr(next + 1, caracter);
  }
  return count;
}
