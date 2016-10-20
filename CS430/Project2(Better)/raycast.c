#include "Parser.h"
#include "PPM_Conv.h"
#include "Structures.h"
#include "PPM_Conv.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static inline double sqr(double v) { return v * v; }

static inline void normalize(double *v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

double plane_intersection(double* Ro, double* Rd, Object* objIntersect) {
  double* norm = objIntersect->plane.normal;
  double* pos = objIntersect->plane.position;
  double t;
  t = (norm[0]*Ro[0] + norm[1]*Ro[1] + norm[2]*Ro[2]) + pos[0] + pos[1] + pos[2];
  t = t/(norm[0]*Rd[0] + norm[1]*Rd[1] + norm[2]*Rd[2]);
  if(t < 0) {
    return -1;
  }
  return t;

}

double sphere_intersection(double *Ro, double *Rd, Object* objIntersect) {
  double r = objIntersect->sphere.radius;
  double* C = objIntersect->sphere.position;
  double a = sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]);
	double b =  2 * (Rd[0] * (Ro[0] - C[0]) + Rd[1] * (Ro[1] - C[1]) + Rd[2] * (Ro[2] - C[2]));
	double c = sqr(Ro[0] - C[0]) + sqr(Ro[1] - C[1]) + sqr(Ro[2] - C[2]) - sqr(r);

    double det = sqr(b) - 4 * a * c;
    if (det < 0)
      return -1;

    det = sqrt(det);

    double t0 = (-b - det) / (2 * a);
    if (t0 > 0)
      return t0;

    double t1 = (-b + det) / (2 * a);
    if (t1 > 0)
      return t1;

    return -1;
}

Object* cast_ray(double* t, Object** objArray, Object* exclude, double* Ro, double* Rd) {

  double best_t = INFINITY, current_t;
  Object* best_object = NULL;

  for(int i = 0; objArray[i] != NULL; i++) {
    if(objArray[i]->kind == plane_kind) {
      //printf("Before\n");
      current_t = plane_intersection(Ro, Rd, objArray[i]);
      //printf("After\n");
    } else if(objArray[i]->kind == sphere_kind) {
      //printf("Before\n");
      current_t = sphere_intersection(Ro, Rd, objArray[i]);
      //printf("After\n");
    } else if(objArray[i]->kind == camera_kind) {
      continue;

    } else {
      fprintf(stderr, "Unknown Encounter\n");
      exit(1);
    }
    if(current_t < best_t && current_t != -1) {
     best_t = current_t;
     best_object = objArray[i];
    }
  }
    if(best_t == INFINITY) {
      best_t = -1;
    }
   *t = best_t;
    return best_object;
}

double* get_color(double* Ro, double* Rd, Object** objArray) {
  double t = 0;
  double* color = malloc(sizeof(double)*3);
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;


  Object* object = cast_ray(&t, objArray, NULL, Ro, Rd);
  if(t == -1) {
    return color;
    }
  //printf("%lf %lf %lf\n", object->color[0], object->color[1], object->color[2]);
  //printf("%d", object->kind);
  //exit(0);
  color[0] = object->color[0];
  color[1] = object->color[1];
  color[2] = object->color[2];
  //Lighting work here

  return color;
}

Pixel** make_scene(Object** objects, int height, int width) {


  //Take from camera
  double cx = 0;
  double cy = 0;
  double h = 1;
  double w = 1;
  double* color;

  int M = height;
  int N = width;

  Pixel** buffer = malloc(M * N * sizeof(Pixel*));

  double pixheight = h / M;
  double pixwidth = w / N;

 //printf("%d %d\n", M, N);

  for (int y = 0; y < M; y++) {
    for (int x = 0; x < N; x++) {
      double Ro[3] = {0, 0, 0};
      // Rd = normalize(P - Ro)
      double Rd[3] = {cx - (w / 2) + pixwidth * (x + 0.5),
                      -(cy - (h / 2) + pixheight * (y + 0.5)), 1};
      normalize(Rd);
      //printf("Before\n");
      color = get_color(Ro, Rd, objects);
      //printf("After %d\n", y);

      Pixel* pix = malloc(sizeof(Pixel));

      pix->r = (unsigned char)color[0]*255;
      pix->g = (unsigned char)color[1]*255;
      pix->b = (unsigned char)color[2]*255;
      //Goes down 'y*N' times and over 'x' times
      buffer[y*N + x] = pix;
    } //printf("%d", y);
  } //printf("\nout");
  return buffer;
}

int main(int argc, char *argv[]) {

  if(argc != 5) {
    fprintf(stderr, "Incorrect Input: raycast height width input.json output.ppm\n");
  }

  int height, width;
  height = atoi(argv[1]);
  width = atoi(argv[2]);
  Pixel** buffer;
  Object** objArray;

  objArray = read_scene(argv[3]);
  //printf("Read = Okay\n");
  buffer = make_scene(objArray, height, width);
  //printf("make = OKay\n");
  FILE* fh = fopen(argv[4], "w");
  write_file(fh, buffer, height, width);
  //printf("PPM = Okay\n");
  exit(0);
}
