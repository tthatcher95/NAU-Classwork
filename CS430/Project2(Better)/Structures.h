#ifndef camera_kind
#define camera_kind 1
#define sphere_kind 2
#define plane_kind 3

typedef struct {
  int kind;
  char *type;
  double* color;
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
  };
} Object;

typedef struct {
  unsigned char r, g, b;
} Pixel;
#endif
