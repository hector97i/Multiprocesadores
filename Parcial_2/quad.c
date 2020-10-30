#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#define num_threads 12
#define MASK 11

unsigned char *gray(unsigned char *img, int ancho, int alto, int n);
unsigned char *shift(unsigned char *img, int ancho, int alto, int n);
unsigned char *blur(unsigned char *img, int ancho, int alto, int n, int mask);

int main()
{
    FILE *image, *outputImage, *lecturas;
    image = fopen("f1.bmp", "rb");           //Imagen original a transformar
    outputImage = fopen("f1quad.bmp", "wb"); //Imagen transformada
    long ancho;
    long alto;

    double t1, t2, tiempo;
    t1 = omp_get_wtime();
    omp_set_num_threads(num_threads);
    unsigned char xx[54];
    for (int i = 0; i < 54; i++)
    {
        xx[i] = fgetc(image);
        fputc(xx[i], outputImage); //Copia cabecera a nueva imagen
    }
    ancho = (long)xx[20] * 65536 + (long)xx[19] * 256 + (long)xx[18];
    alto = (long)xx[24] * 65536 + (long)xx[23] * 256 + (long)xx[22];

    long anchox3 = ancho * 3;
    int n = 0;
    if (anchox3 % 4 != 0)
    {
        anchox3 = anchox3 / 4;
        anchox3 = ((int)anchox3) * 4 + 4;
        n = anchox3 - ancho * 3;
    }
    printf("largo img %li\n", alto);
    printf("ancho img %li\n", ancho);

    unsigned char *arr_in = (unsigned char *)malloc((ancho + n) * alto * 3 * sizeof(unsigned char));
    unsigned char *arr_out = (unsigned char *)malloc((ancho + n) * alto * 3 * sizeof(unsigned char));
    unsigned char *arr_blur = (unsigned char *)malloc((ancho + n) * alto * 3 * sizeof(unsigned char));

    int j = 0;
    while (!feof(image))
    {
        *(arr_in + j) = fgetc(image);
        j++;
    }

    int altura_quad = alto / 4;
    int altura_ultimo = altura_quad + alto % 4;
    unsigned char *paux = arr_in;
    unsigned char *q1 = malloc(altura_quad * (ancho + n) * 3 * sizeof(unsigned char));
    unsigned char *q2 = malloc(altura_quad * (ancho + n) * 3 * sizeof(unsigned char));
    unsigned char *q3 = malloc(altura_quad * (ancho + n) * 3 * sizeof(unsigned char));
    unsigned char *q4 = malloc(altura_ultimo * (ancho + n) * 3 * sizeof(unsigned char));

    memcpy(q1, paux, altura_quad * (ancho + n) * 3 * sizeof(unsigned char));
    paux += altura_quad * (ancho + n) * 3;
    memcpy(q2, paux, altura_quad * (ancho + n) * 3 * sizeof(unsigned char));
    paux += altura_quad * (ancho + n) * 3;
    memcpy(q3, paux, altura_quad * (ancho + n) * 3 * sizeof(unsigned char));
    paux += altura_ultimo * (ancho + n) * 3;
    memcpy(q4, paux, altura_quad * (ancho + n) * 3 * sizeof(unsigned char));

    //printf("BEFORE FUNC\n");

    #pragma omp parallel
    {
        #pragma omp sections 
        {
            #pragma omp section
            {   
                unsigned char *grayRes;
                unsigned char *shiftRes;
                unsigned char *blurRes;

                grayRes = gray(q1, ancho, altura_quad, n);
                shiftRes = shift(grayRes, ancho, altura_quad, n);
                blurRes = blur(shiftRes, ancho, altura_quad, n, MASK);
                //blurRes = shiftRes;
                memset(q1, '\0', altura_quad * (ancho + n) * 3);
                
                memcpy(q1, blurRes, altura_quad * (ancho + n) * 3);
                
            }

            #pragma omp section
            {
                unsigned char *grayRes2;
                unsigned char *shiftRes2;
                unsigned char *blurRes2;

                grayRes2 = gray(q2, ancho, altura_quad, n);
                shiftRes2 = shift(grayRes2, ancho, altura_quad, n);
                blurRes2 = blur(shiftRes2, ancho, altura_quad, n, MASK);
                //blurRes = shiftRes;
                memset(q2, '\0', altura_quad * (ancho + n) * 3);
                memcpy(q2, blurRes2, altura_quad * (ancho + n) * 3);

            }

            #pragma omp section
            {   
                unsigned char *grayRes3;
                unsigned char *shiftRes3;
                unsigned char *blurRes3;
                grayRes3 = gray(q3, ancho, altura_quad, n);
                shiftRes3 = shift(grayRes3, ancho, altura_quad, n);
                blurRes3 = blur(shiftRes3, ancho, altura_quad, n, MASK);
                //blurRes = shiftRes;
                memset(q3, '\0', altura_quad * (ancho + n) * 3);
                memcpy(q3, blurRes3, altura_quad * (ancho + n) * 3);
               
            }

            #pragma omp section
            {
                unsigned char *grayRes4;
                unsigned char *shiftRes4;
                unsigned char *blurRes4;
                grayRes4 = gray(q4, ancho, altura_ultimo, n);
                shiftRes4 = shift(grayRes4, ancho, altura_ultimo, n);
                blurRes4 = blur(shiftRes4, ancho, altura_ultimo, n, MASK);
                //blurRes = shiftRes;
                memset(q4, '\0', altura_ultimo * (ancho + n) * 3);
                memcpy(q4, blurRes4, altura_ultimo * (ancho + n) * 3);

            }
        }
    }
    
    for(int i = 0; i<altura_quad * (ancho + n) * 3; i++){

        fputc(*(q1 + i), outputImage);
    }
    for(int i = 0; i<altura_quad * (ancho + n) * 3; i++){

        fputc(*(q2 + i), outputImage);
    }
    for(int i = 0; i<altura_quad * (ancho + n) * 3; i++){

        fputc(*(q3 + i), outputImage);
    }
    for(int i = 0; i<altura_ultimo * (ancho + n) * 3; i++){

        fputc(*(q4 + i), outputImage);
    }
    
    t2 = omp_get_wtime();
    tiempo = t2 - t1;
    printf("%lf segundos\n", tiempo);

    fclose(image);
    fclose(outputImage);
    return 0;
}

unsigned char *gray(unsigned char *img, int ancho, int alto, int n)
{

    int count = 0;
    unsigned char r, g, b; //Pixel
    unsigned char *arr_in = malloc((ancho + n) * alto * 3 * sizeof(unsigned char));
    memcpy(arr_in, img, (ancho + n) * alto * 3);
    unsigned char pixel = 0;

    for (int i = 0; i < (ancho + n) * alto * 3; i += 3)
    {
        pixel = 0;
        b = *(arr_in + i);
        g = *(arr_in + i + 1);
        r = *(arr_in + i + 2);
        pixel = 0.21 * r + 0.72 * g + 0.07 * b;
        *(arr_in + i) = pixel;
        *(arr_in + i + 1) = pixel;
        *(arr_in + i + 2) = pixel;
        count += 3;
        if (count == ancho * 3)
        {
            i += 2;
            count = 0;
        }
    }

    return arr_in;
}

unsigned char *shift(unsigned char *img, int ancho, int alto, int n)
{

    unsigned char *arr_in = malloc((ancho + n) * alto * 3 * sizeof(unsigned char));
    unsigned char *arr_out = malloc((ancho + n) * alto * 3 * sizeof(unsigned char));
    memcpy(arr_in, img, (ancho + n) * alto * 3);
    memcpy(arr_out, img, (ancho + n) * alto * 3);

    for (int i = 0; i < alto; i++)
    {
        for (int j = 0; j < (ancho * 3); j += 3)
        {
            *(arr_out + (i * ((ancho * 3) + n)) + j) = *(arr_in + (i * ((ancho * 3) + n)) + (ancho * 3) - j);
            *(arr_out + (i * ((ancho * 3) + n)) + j + 1) = *(arr_in + (i * ((ancho * 3) + n)) + (ancho * 3) - j - 1);
            *(arr_out + (i * ((ancho * 3) + n)) + j + 2) = *(arr_in + (i * ((ancho * 3) + n)) + (ancho * 3) - j - 2);
        }
    }

    return arr_out;
}

unsigned char *blur(unsigned char *img, int ancho, int alto, int n, int mask)
{

    unsigned char *arr_out = malloc((ancho + n) * alto * 3 * sizeof(unsigned char));
    unsigned char *arr_blur = malloc((ancho + n) * alto * 3 * sizeof(unsigned char));
    memcpy(arr_out, img, (ancho + n) * alto * 3);
    memcpy(arr_blur, img, (ancho + n) * alto * 3);
    unsigned char aux = 0;
    aux = 0;

    for (int i = 0; i < alto; i++)
    {
        for (int j = 0; j < (ancho * 3); j += 3)
        {
            aux = 0.0;
            for (int k = -(mask / 2); k <= mask / 2; k++)
            {
                for (int l = -(mask * 3 / 2); l <= mask * 3 / 2; l += 3)
                {
                    aux += *(arr_out + ((i + k + 1) * ((ancho * 3) + n)) + j + l + 1) / (mask * mask);
                }
            }
            *(arr_blur + (i * ((ancho * 3) + n)) + j) = aux;
            *(arr_blur + (i * ((ancho * 3) + n)) + j + 1) = aux;
            *(arr_blur + (i * ((ancho * 3) + n)) + j + 2) = aux;
        }
    }

    return arr_blur;
}