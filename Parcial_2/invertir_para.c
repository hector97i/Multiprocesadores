#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define num_threads 12

int main()
{
  FILE *image, *outputImage, *lecturas;
  image = fopen("f10.bmp", "rb");           //Imagen original a transformar
  outputImage = fopen("f10para.bmp", "wb"); //Imagen transformada
  long ancho;
  long alto;
  unsigned char aux = 0;

  unsigned char r, g, b; //Pixel
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
    *(arr_out + j) = *(arr_in + j);
    *(arr_blur + j) = *(arr_in + j);
    j++;
  }
  printf("%d\n", j);
  int count = 0;
////#pragma omp parallel
//{
////#pragma omp for ordered
#pragma omp parallel for schedule(static)
  for (int i = 0; i < (ancho + n) * alto * 3; i += 3)
  {

    b = *(arr_in + i);
    g = *(arr_in + i + 1);
    r = *(arr_in + i + 2);
    unsigned char pixel = 0.21 * r + 0.72 * g + 0.07 * b;
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
  printf("FINISH GRAY CONVERT\n");
////#pragma omp for ordered
#pragma omp parallel for schedule(static)
  for (int i = 0; i < alto; i++)
  {
    for (int j = 0; j < (ancho * 3); j += 3)
    {
      *(arr_out + (i * ((ancho * 3) + n)) + j) = *(arr_in + (i * ((ancho * 3) + n)) + (ancho * 3) - j);
      *(arr_out + (i * ((ancho * 3) + n)) + j + 1) = *(arr_in + (i * ((ancho * 3) + n)) + (ancho * 3) - j - 1);
      *(arr_out + (i * ((ancho * 3) + n)) + j + 2) = *(arr_in + (i * ((ancho * 3) + n)) + (ancho * 3) - j - 2);
    }
  }
  aux = 0;
  int mask = 11;
  printf("FINISH SHIFT CONVERT\n");

#pragma omp parallel for schedule(guided)
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
  //}
  printf("FINISH BLUR CONVERT\n");
  for (int i = 0; i < (ancho + n) * alto * 3; i++)
  {
    fputc(*(arr_blur + i), outputImage);
  }
  t2 = omp_get_wtime();
  tiempo = t2 - t1;
  printf("%lf segundos\n", tiempo);

  fclose(image);
  fclose(outputImage);
  return 0;
}
