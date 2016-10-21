#include "Structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int line = 1;

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  char error_check = fscanf(json, "%lf", &value);
  if(error_check != 1) {
    fprintf(stderr, "Incorrect Number\n");
    exit(1);
  }
  return value;
}

double* next_vector(FILE* json) {
  double* v = malloc(3*sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}


Object** read_scene(char* filename, Object** lights) {
  int lightindex = 0, index = 0;
  int c;
  FILE* json = fopen(filename, "r");

  if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }
  Object** objArray = malloc(sizeof(Object*)*128);
  Object** lights = malloc(sizeof(Object*)*128);

  objArray[127] = NULL;
  lights[127] = NULL;
  Object* object;

  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects

  while (1) {
    c = fgetc(json);
    if (c == ']') {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return NULL;
    }
    if (c == '{') {
      skip_ws(json);

      // Parse the object
      char* key = next_string(json);
      if (strcmp(key, "type") != 0) {
	fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
	exit(1);
      }

      skip_ws(json);

      expect_c(json, ':');

      skip_ws(json);

      char* value = next_string(json);

      object = malloc(sizeof(Object));

      if (strcmp(value, "camera") == 0) {
        object->kind = camera_kind;
        objArray[index++] = object;
      } else if (strcmp(value, "sphere") == 0) {
        object->kind = sphere_kind;
        objArray[index++] = object;
      } else if (strcmp(value, "plane") == 0) {
        object->kind = plane_kind;
        objArray[index++] = object;
      } else if(strcmp(value, "light") == 0) {
        object->kind = light_kind;
        lights[lightindex++] = object;
      }
        else {
	       fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
	       exit(1);
      }

      skip_ws(json);

      while (1) {
	// , }
	c = next_c(json);
	if (c == '}') {
	  // stop parsing this object
	  break;

	} else if (c == ',') {
	  // read another field
	  skip_ws(json);
	  char* key = next_string(json);
	  skip_ws(json);
	  expect_c(json, ':');
	  skip_ws(json);

	  if ((strcmp(key, "width") == 0) ||
	      (strcmp(key, "height") == 0) ||
	      (strcmp(key, "radius") == 0) ||
        (strcmp(key, "color") == 0) ||
        (strcmp(key, "theta") == 0) ||
        (strcmp(key, "radial-a2") == 0) ||
        (strcmp(key, "radial-a1") == 0) ||
        (strcmp(key, "radial-a0") == 0) ||
        (strcmp(key, "angular-a0") == 0)||
        (strcmp(key, "direction") == 0)) {
	         double value = next_number(json);

          if(strcmp(key, "width") == 0) {
            object->camera.width = value;

        } else if(strcmp(key, "height") == 0) {
            object->camera.height = value;

        } else if(strcmp(key, "radius") == 0) {
            object->sphere.radius = value;

        } else if(strcmp(key, "color") == 0) {
            object->diffuse_color = value;

        } else if(strcmp(key, "theta") == 0) {
            object->light.theta = value;

        } else if(strcmp(key, "direction") == 0) {
            object->light.direction = value;

        } else if(strcmp(key, "radial-a2") == 0) {
            object->light.r2 = value;

        } else if(strcmp(key, "radial-a1") == 0) {
            object->light.r1 = value;

        } else if(strcmp(key, "radial-a0") == 0) {
            object->light.r0 = value;

        } else if(strcmp(key, "angular-a0") == 0) {
            object->light.a0 = value;
        }
	  } else if ((strcmp(key, "diffuse_color") == 0) ||
		     (strcmp(key, "position") == 0) ||
		     (strcmp(key, "normal") == 0) || strcmp(key, "color") == 0) ||
          (strcmp(key, "specular_color") == 0)) {
          double* value = next_vector(json);

          if(strcmp(key, "diffuse_color") == 0) {
            object->diffuse_color = value;

        } else if(strcmp(key, "specular_color") == 0) {
            object->specular_color = value;

        } else if(strcmp(key, "color") == 0) {
            object->diffuse_color = value;

        } else if(strcmp(key, "position") == 0 && object->kind == plane_kind) {
            object->plane.position = value;

        } else if(strcmp(key, "position") == 0 && object->kind == sphere_kind) {
            object->sphere.position = value;

        } else if(strcmp(key, "position") == 0 && object->kind == light_kind) {
            object->light.position = value;

        } else if(strcmp(key, "normal") == 0) {
            object->plane.normal = value;
        }
	  } else {
	    fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
		    key, line);
	    //char* value = next_string(json);
	  }
	  skip_ws(json);
	} else {
	  fprintf(stderr, "Error: Unexpected value on line %d\n", line);
	  exit(1);
	}
      }
      skip_ws(json);
      c = next_c(json);
      if (c == ',') {
	// noop
	    skip_ws(json);
      } else if (c == ']') {
	        fclose(json);
          objArray[index] = NULL;
          lights[lightindex]
	        return objArray;
      } else {
	       fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
	       exit(1);
      }
    }
  }
}
