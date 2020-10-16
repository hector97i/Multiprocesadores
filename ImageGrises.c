#include <stdio.h>
#include <stdlib.h>
#include "omp.h"

int main()
{
    FILE *image, *outputImage, *lecturas;
    image = fopen("sample.bmp","rb");          //Imagen original a transformar
    outputImage = fopen("img2_dd.bmp","wb");    //Imagen transformada
    unsigned char r, g, b, pixel;               //Pixel
    omp_set_num_threads(100);
    for(int i=0; i<54; i++) fputc(fgetc(image), outputImage);   //Copia cabecera a nueva imagen
    
    int i = 0;
    
	#pragma omp parallel for schedule(guided)
	for(int i = 0; i < 927361; i++){                                        //Grises

		b = fgetc(image);
		g = fgetc(image);
		r = fgetc(image);
		i++;
		pixel = 0.2126*r + 0.7152*g + 0.0722*b;
		fputc(pixel, outputImage);
		fputc(pixel, outputImage);
		fputc(pixel, outputImage);
		//printf("%d\n", omp_get_thread_num());
    }

    fclose(image);
    fclose(outputImage);
    return 0;
}
