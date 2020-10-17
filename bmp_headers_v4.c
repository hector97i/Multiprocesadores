#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#define NUM_THREADS 6

struct my_pixel{
  unsigned char r, g, b;
};

int main()
{
    omp_set_num_threads(NUM_THREADS);
    FILE *image, *outputImage, *lecturas;
    image = fopen("sample.bmp","rb");          //Imagen original a transformar
    outputImage = fopen("img5_dd.bmp","wb");    //Imagen transformada
    long ancho;
    long alto;
    unsigned char r, g, b;               //Pixel

    unsigned char xx[54];
    
    for(int i=0; i<54; i++) {
      xx[i] = fgetc(image);
      fputc(xx[i], outputImage);   //Copia cabecera a nueva imagen
    }
    

    ancho = (long)xx[20]*65536+(long)xx[19]*256+(long)xx[18];
    alto = (long)xx[24]*65536+(long)xx[23]*256+(long)xx[22];
    printf("largo img %li\n",alto);
    printf("ancho img %li\n",ancho);

    unsigned char* arr_in = (unsigned char*)malloc(2782080*sizeof(unsigned char));
    unsigned char* arr_out = (unsigned char*)malloc(2782080*sizeof(unsigned char));

  int j= 0;
  while(!feof(image)){
        *(arr_in + j) = fgetc(image);
        *(arr_out + j) = *(arr_in + j);
        j++;
    }
    printf("%d\n", j);
    int count = 0;
    #pragma omp parallel for schedule(guided)
    for(int i = 0; i < 2782080; i+=3){

            b = *(arr_in + i);
            g = *(arr_in + i + 1);
            r = *(arr_in + i + 2);
            unsigned char pixel = 0.21*r+0.72*g+0.07*b;
            *(arr_in + i) = pixel;
            *(arr_in + i + 1) = pixel;
            *(arr_in + i + 2) = pixel;
            count += 3;
            if(count == ancho*3){
                i+=2;
                count = 0;
            }

    }
    

    #pragma omp parallel for schedule(guided)
    for(int i = 0; i < alto; i++){
        for(int j = 0; j < (ancho*3); j+= 3){
            *(arr_out + (i*((ancho*3)+2))+j) = *(arr_in + (i*((ancho*3)+2))+ (ancho*3) - j);
            *(arr_out + (i*((ancho*3)+2))+j + 1) = *(arr_in + (i*((ancho*3)+2))+ (ancho*3 )- j - 1);
            *(arr_out + (i*((ancho*3)+2))+j + 2) = *(arr_in + (i*((ancho*3)+2))+ (ancho*3) - j - 2);
        }
    }
    
    
    #pragma omp parallel for schedule(guided)
    for(int i = 0; i < 2782080; i++){
        fputc(*(arr_out + i), outputImage);
    }
    

    fclose(image);
    fclose(outputImage);
    return 0;
}
