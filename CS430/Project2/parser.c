
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ascii_tracer.c"


//#include "Project1.c"
int line = 1;

int num_objects = 200;
double** Store_Object;
int num_iter = 0;


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
  return (char*)strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  fscanf(json, "%f", &value);
  // Error check this..
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


void read_scene(int height, int width, char* filename, char *outputfile) {
  int c;
  Store_Object = malloc(sizeof(double)*height);
  objArray object;
  object.Array = NULL;
  object.objNumObj = 0;

  //for(int i = 0; i < width; i++){
    //Store_Object[i] = malloc(sizeof(double)*width);
  //}

  FILE* json = fopen(filename, "r");

  if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }

  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects

  while (1) {
    c = fgetc(json);
    int iterations = 0;
    if (c == ']') {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return;
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

      // +1 Objects in the Array
      object.objNumObj += 1;
      // Realloc for the new array space
      object.Array = realloc(object.Array, sizeof(Object)*object.objNumObj);
      // Set type
      object.Array[object.objNumObj - 1].type = value;

      if (strcmp(value, "camera") == 0) {
        Store_Object[num_iter][0] = 0; //Case = 0 'camera'
      } else if (strcmp(value, "sphere") == 0) {
        Store_Object[num_iter][0] = 1; //case = 1 'sphere'
      } else if (strcmp(value, "plane") == 0) {
        Store_Object[num_iter][0] = 2; //case = 2 'plane'
      } else {
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
    printf("%c\n", c);
    printf("%s\n", key);
	  skip_ws(json);
	  expect_c(json, ':');
	  skip_ws(json);
    //char* key = next_string(json);
    printf("Right before storing widht\n");
    if ((strcmp(key, "width") == 0) || (strcmp(key, "height") == 0) || (strcmp(key, "radius") == 0)) {
      // store the val of camera.width, camera.height, or sphere.radius
      double value = next_number(json);
        if(value <= 0) {
          fprintf(stderr, "Error the width - height - and radius must be greater than 0!\n");
        } else {
	         if (strcmp(key, "width") == 0) {
      //Width = 1
      object.Array[object.objNumObj - 1].camera.width = value;
    //  Store_Object[num_iter][1] = value;

    }
        //height = 2
	  else if (strcmp(key, "height") == 0) {
        object.Array[object.objNumObj - 1].camera.height = value;
        //Store_Object[num_iter][2];

        }
        //Radius = 3
	  else if(strcmp(key, "radius") == 0) {
          object.Array[object.objNumObj - 1].sphere.radius = value;
          //Store_Object[num_iter][3];
        }
      }
          //color = 4
	  } else if ((strcmp(key, "color") == 0) || (strcmp(key, "position") == 0) || (strcmp(key, "normal") == 0)) {
          double* value = next_vector(json);
          if((strcmp(key, "color") == 0)) {
          object.Array[object.objNumObj - 1].color[0] = value[0];
          object.Array[object.objNumObj - 1].color[1] = value[1];
          object.Array[object.objNumObj - 1].color[2] = value[2];
          //Store_Object[num_iter][4];
        }

          else if (strcmp(key, "position") == 0) {
            object.Array[object.objNumObj - 1].sphere.center[0] = value[0];
            object.Array[object.objNumObj - 1].sphere.center[1] = value[1];
            object.Array[object.objNumObj - 1].sphere.center[2] = value[2];
          }
          else if(strcmp(key, "normal") == 0) {

            object.Array[object.objNumObj - 1].plane.normal[0] = value[0];
            object.Array[object.objNumObj - 1].plane.normal[1] = value[1];
            object.Array[object.objNumObj - 1].plane.normal[2] = value[2];
          }
	  } else {
	    fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
		    key, line);
	    //char* value = next_string(json);
	  }
	  skip_ws(json);
	 }
  }
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
	return;
      } else {
	fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
	exit(1);
      }  num_iter+= 1;
    }
