

	#include <math.h>
	#include <stdint.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include "VectorMath.h"
	#include "ppm_conv.h"
	#include "Project1.h"



	const int TYPE_CAMERA = 0;

	// Plymorphism in C



	typedef struct {
	  int kind;
	  char *type;
	  double color[3];
	  union {
	    struct {
	      double center[3];
	      double width;
	      double height;
	    } camera;
	    struct {
	      double center[3];
	      double radius;
	    } sphere;
	    struct {
	      double normal[3];
	      double position[3];
	    } plane;
	  };
	} Object;

	typedef struct {
	  Object *Array;
	  size_t objNumObj;
	} objArray;

	static inline double sqr(double v) { return v * v; }

	static inline void normalize(double *v) {
	  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	  v[0] /= len;
	  v[1] /= len;
	  v[2] /= len;
	}

	double sphere_intersection(double *Ro, double *Rd, double *Center, double r) {
  double a = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
  double b = (2 * (Ro[0] * Rd[0] - Rd[0] * Center[0] + Ro[1] * Rd[1] -
                   Rd[1] * Center[1] + Ro[2] * Rd[2] - Rd[2] * Center[2]));
  double c = sqr(Ro[0]) - 2 * Ro[0] * Center[0] + sqr((Center[1]) + Ro[1]) -
                      2 * Ro[1] * Center[1] + sqr(Center[1]) + sqr(Ro[2]) -
                      2 * Ro[2] * Center[2] + sqr(Center[2]) - sqr(r);

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

	double plane_intersection(double *Ro, double *Rd, double *position,
	                          double *normal) {
	  // distance = dot(Po-Lo,N)/dot(L,N)
	  V3 temp = malloc(sizeof(double) * 3);
	  v3_subtract(Ro, position, temp);
	  double distance = v3_dot(normal, temp);

	  double denominator = v3_dot(normal, Rd);
	  distance = -(distance / denominator);
	  if (distance > 0)
	    return distance;

	  return 0;
	}

	int main(int argc, char* argv[]) {
	  if(argc < 5) {
	    fprintf(stderr, "Incorrect number of parameters: Must be heigt / wdith / json / ppm\n");
	    exit(1);

      FILE *output = fopen(argv[4], "wb");
 if (!output) {
   fprintf(stderr, "ERROR: Failed to open file %s\n", argv[4]);
   fclose(output);
   return -1;
}
	  }

	  int height = atoi(argv[1]);
	  int width = atoi(argv[2]);
	  char *file = argv[3];

	  //read_scene(height, width, file);

	  Pixel color = {.r = 0, .g = 255, .b = 0};
	  Pixel black = {.r = 0, .g = 0, .b = 0};

	  Object **objects;
	  objects = malloc(sizeof(Object *) * 3);
	  objects[0] = malloc(sizeof(Object));

	  objects[2] = NULL;

	  double cx = 0;
	  double cy = 0;
	  double h = 2;
	  double w = 2;

	  int M = height;
	  int N = width;

	  Pixel *buffer = malloc(M * N * sizeof(Pixel));

	  double pixheight = h / M;
	  double pixwidth = w / N;

	  printf("%d %d\n", M, N);

	  for (int y = 0; y < M; y += 1) {
	    for (int x = 0; x < N; x += 1) {
	      double Ro[3] = {0, 0, 0};
	      // Rd = normalize(P - Ro)
	      double Rd[3] = {cx - (w / 2) + pixwidth * (x + 0.5),
	                      -(cy - (h / 2) + pixheight * (y + 0.5)), 1};
	      normalize(Rd);

	      double best_t = INFINITY;
	      for (int i = 0; objects[i] != 0; i += 1) {
	        double t = 0;

	        switch (objects[i]->kind) {
	        case 0:
	          t = plane_intersection(Ro, Rd, objects[i]->plane.position,
	                                 objects[i]->plane.normal);
	          break;
	        case 1:
	          t = sphere_intersection(Ro, Rd, objects[i]->sphere.center,
	                                  objects[i]->sphere.radius);
	          break;
	        default: // error
	          exit(1);
	        } // end of switch

	        if (t > 0 && t < best_t)
	          best_t = t;
	      } // end of loop

	      if (best_t > 0 && best_t != INFINITY) {
	        buffer[y * M + x] = color;
	      } else {
	        buffer[y * M + x] = black;
	      }
	    }
	  }

	  FILE *output_file = fopen(argv[4], "w");

	  buffer_to_bin(buffer, M, N, output_file);
	  return 0;
	}
