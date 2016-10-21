static inline void v3_add(double* a, double* b, double* c) {
  c[0] = a[0] + b[0];
  c[1] = a[1] + b[1];
  c[2] = a[2] + b[2];
}


static inline void v3_subtract(double* a, double* b, double* c) {
  c[0] = a[0] - b[0];
  c[1] = a[1] - b[1];
  c[2] = a[2] - b[2];
}


static inline void v3_scale(double* a, double s, double* c) {
  c[0] = s * a[0];
  c[1] = s * a[1];
  c[2] = s * a[2];
}


static inline double v3_dot(double* a, double* b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


static inline void v3_cross(double* a, double* b, double* c) {
  c[0] = a[1]*b[2] - a[2]*b[1];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - a[1]*b[0];
}

size_t percentile_in_range(size_t min, size_t max, size_t val) {
  return (val - min)/(max - min);
}

static inline void v3_mult(double* a, double* b, double* output) {
  output[0] = b[0] * a[0];
  output[1] = b[1] * a[1];
  output[2] = b[2] * a[2];
}
