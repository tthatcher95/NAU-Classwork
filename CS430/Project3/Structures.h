#ifndef camera_kind
#define camera_kind 1
#define sphere_kind 2
#define plane_kind 3
#define light_kind 4

typedef struct {
  int kind;
  char *type;
  double* diffuse_color;
  double* specular_color;
  union {
    struct { //1
      double width;
      double height;
    } camera;
    struct { //2
      double* position;
      double radius;
    } sphere;
    struct { //3
      double* normal;
      double* position;
    } plane;
      struct {
        double* position;
        double* direction;
        double theta;
        double r0;
        double r1;
        double r2;
        double a0;
      } light;
  };
} Object;

typedef struct {
  unsigned char r, g, b;
} Pixel;
#endif
