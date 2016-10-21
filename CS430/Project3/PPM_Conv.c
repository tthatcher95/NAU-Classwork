#include "Structures.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void write_file(FILE* fh, Pixel** buffer, int height, int width){
  fprintf(fh, "P%i\n%i %i\n%i\n", 3, width, height, 255);
  int i;
  for(int i = 0; i < (height*width); i++) {
    Pixel* pix = buffer[i];
    fprintf(fh, "%i\n%i\n%i\n", pix->r, pix->g, pix->b);
  }
}
