#include "Parser.h"
#include "PPM_Conv.h"
#include "Structures.h"
#include "PPM_Conv.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "VectorMath.h"

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
    if(objArray[i] == exclude) {
      continue;
    } else if(objArray[i]->kind == plane_kind) {
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

void get_intersection(double* val_intersect, double* Ro, double* Rd, double t) {
  val_intersect[0] = t*Rd[0] + Ro[0];
  val_intersect[1] = t*Rd[1] + Ro[1];
  val_intersect[2] = t*Rd[2] + Ro[2];
}

void get_sphere_normal(Object* object, double* val_intersect, double* normal){
    v3_subtract(val_intersect, object->sphere.position, normal);
    normalize(normal);
}

void get_plane_normal(Object* object, double* normal) {
   memcpy(normal, object->plane.normal, sizeof(double)*3);
   normalize(normal);
}
 //*---------------Begin Diffuse Lighting Equations------------*//
void get_diffuse(double* total_diffuse_color, double* normal, double* light_direction, Object* object, double* light_color) {
  double dot_product = v3_dot(light_direction, normal);

  if(dot_product <= 0) {
    dot_product = 0;
  } else {
    //printf("%lf\n", dot_product);
  }

  v3_scale(object->diffuse_color, dot_product, total_diffuse_color);
  v3_mult(light_color, total_diffuse_color, total_diffuse_color);
  //printf("%lf %lf %lf\n", total_diffuse_color[0], total_diffuse_color[1], total_diffuse_color[2]);

} //*--------------End Diffuse Lighting--------------*//

//*---------------------*Begin Specular Lighting*------------------*//
void get_specular(double* total_specular_color, double* normal, double* light_direction, Object* object, double* light_color, double* Rd) {
  double ns = 20.0; //"Shininess"
  double reflected_light_direction[3] = {0, 0, 0};
  double negative_Rd[3] = {0, 0, 0};
  double dot_product = 0;

  reflection_vector(light_direction, normal, reflected_light_direction);
  v3_scale(Rd, 1, negative_Rd);

  //Returns the bigger value of the two (makes vector non-negative)
  dot_product = max_val(v3_dot(reflected_light_direction, negative_Rd), 0);
  dot_product = pow(dot_product, ns);
  v3_scale(light_color, dot_product, total_specular_color);
  v3_mult(total_specular_color, object->specular_color, total_specular_color);
} //*------------End Specular---------*//

double* get_color(double* Ro, double* Rd, Object** objArray, Object** lights) {
  double t = 0;
  double* color = malloc(sizeof(double)*3);

  color[0] = 0;
  color[1] = 0;
  color[2] = 0;


  Object* object = cast_ray(&t, objArray, NULL, Ro, Rd);

  if(t == -1) {
    return color;
    }
  double intersection[3] = {0, 0, 0};
  double normal[3] = {0, 0 ,0};
  get_intersection(intersection, Ro, Rd, t);

  if(object->kind == sphere_kind) {
    get_sphere_normal(object, intersection, normal);
  } else if(object->kind == plane_kind) {
    get_plane_normal(object, normal);
  }

  //Lighting work here
  double current_diffuse_color[3] = {0, 0, 0};
  double total_diffuse_color[3] = {0, 0, 0};
  double current_specular_color[3] = {0, 0, 0};
  double total_specular_color[3] = {0, 0, 0};

  for(int i = 0; lights[i] !=  NULL; i++) {
    double current_light_direction[3] = {0, 0, 0};
    double current_light_color[3] = {0, 0, 0};
    double light_object_intersection[3] = {0, 0, 0};

    v3_subtract(lights[i]->light.position, intersection, current_light_direction);

    //Lightsource -> Intersection
    double magnitude = sqrt(sqr(current_light_direction[0]) + sqr(current_light_direction[1]) + sqr(current_light_direction[2]));
    normalize(current_light_direction);

    //ShadowTests
    Object* temp = cast_ray(&t, objArray, object, intersection, current_light_direction);
    //printf("%lf\n", t);
    if(t >= 0 && t < magnitude && temp->kind != plane_kind) {
      //printf("%lf %d\n", t, temp->kind);
      continue;
    } //else if(t >= 0 && t < magnitude && temp->kind == plane_kind){
      //printf("%d %d %lf %lf %lf %lf\n", object->kind, temp->kind, current_light_direction[0], current_light_direction[1], current_light_direction[2], t);
    //}
    //printf("%lf\n", t);
    v3_scale(current_light_direction, -1, light_object_intersection);
    if(lights[i]->light.direction != NULL) {
      double dot_product = v3_dot(light_object_intersection, lights[i]->light.direction);
      if((dot_product) < sin(lights[i]->light.theta*M_PI/180)) {
        printf("%lf %lf\n", dot_product, lights[i]->light.theta*M_PI/180);
        continue;
      }
      v3_scale(current_light_color, pow(dot_product, lights[i]->light.a0), current_light_color);
    }
    //Further away = less light number
    double radial_a = 1/(sqr(magnitude)*(lights[i]->light.r2) + (lights[i]->light.r1)*magnitude + lights[i]->light.r0);
    v3_scale(lights[i]->diffuse_color, radial_a, current_light_color);

    get_diffuse(current_diffuse_color, normal, current_light_direction, object, current_light_color);
    v3_add(current_diffuse_color, total_diffuse_color, total_diffuse_color);
    //printf("%lf %lf %lf\n", color[0], color[1], color[2]);
    get_specular(current_specular_color, normal, current_light_direction, object, current_light_color, Rd);
    v3_add(current_specular_color, total_specular_color, total_specular_color);

  }
  v3_add(color, total_diffuse_color, color);
  v3_add(color, total_specular_color, color);
  //printf("%lf %lf %lf\n", color[0], color[1], color[2]);
  return color;
}

Pixel** make_scene(Object** objects, Object** lights, int height, int width) {


  //Take from camera
  double cx = 0;
  double cy = 0;
  //Pixel Width/Height
  double h = objects[0]->camera.height;
  double w = objects[0]->camera.width;
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
      color = get_color(Ro, Rd, objects, lights);
      //printf("After %d\n", y);

      Pixel* pix = malloc(sizeof(Pixel));
      //mprintf("%lf %lf %lf\n", color[0], color[1], color[2]);
      pix->r = (unsigned char)clamp_color_value(color[0]*255);
      pix->g = (unsigned char)clamp_color_value(color[1]*255);
      pix->b = (unsigned char)clamp_color_value(color[2]*255);
      //printf("%d %d %d\n", pix->r, pix->g, pix->b);
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
  Object** lights = malloc(sizeof(Object*)*128);

  objArray = read_scene(argv[3], lights);
  //printf("Read = Okay\n");
  buffer = make_scene(objArray, lights, height, width);
  //printf("make = OKay\n");
  FILE* fh = fopen(argv[4], "w");
  write_file(fh, buffer, height, width);
  //printf("PPM = Okay\n");
  exit(0);
}
