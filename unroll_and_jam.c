#include <stdio.h>
#include <omp.h>
#include "math.h"

#define n 9
FILE *fptr;

int main(int argc, char const *argv[]){

    double m[n][n], c[n][n];
    fptr=fopen("Multiplicacion2.txt","w");
    double t1,t2,tiempo;
    t1 = omp_get_wtime();

    //Inicialización de matrices con valores del 0 al n
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            m[i][j] = j;
            fprintf(fptr, "%f\t",m[i][j]);
            if(j == n - 1){
                    fprintf(fptr, "\n");
            }
        }
    }

    //Multiplicación de matrices con Unrolled & Jam
    for (int i = 0; i < n; i+=3){
        for (int j = 0; j < n; j++){
            c[i][j] = 0;
            c[i + 1][j] = 0;
            c[i + 2][j] = 0;
            for (int k = 0; k < n; k++){
                c[i][j] = c[i][j] + m[i][k] * m[k][j]; 
                c[i + 1][j]=c[i + 1][j] + m[i + 1][k] * m[k][j]; 
                c[i + 2][j]=c[i + 2][j] + m[i + 2][k] * m[k][j];               
            }
        }
    }

    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            fprintf(fptr, "%f\t", c[i][j]);
        }
        fprintf(fptr, "\n");
    }

    t2 = omp_get_wtime();
    tiempo = t2 -t1; 
    printf("Ejecución en (%lf)s\n",tiempo);
}



