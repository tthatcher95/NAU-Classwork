#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Project1.c"

//Converts P3 to P3, or, P6 to P6
void same_type(char* buffer, size_t BUFFER_SIZE, FILE* input, FILE* output) {
	while (fread(buffer, 1, BUFFER_SIZE, input)) {
		fwrite(buffer, 1, BUFFER_SIZE, output);
	}

	fseek(input, 0, SEEK_SET);
	fclose(output);
}

//Converts a P3 image to P6 format
int P3_P6(char *buffer, size_t BUFFER_SIZE, FILE *input, FILE *output) {
	fgets(buffer, 256, input);
	char *token = strtok(buffer, " \n");
	while (token) {
		int binary_ascii = atoi(buffer);
		fputc(binary_ascii, output);
		token = strtok(NULL, " \n");
		}
	printf("Converted P3 to P6.\n");
	fclose(output);
	exit(0);
	}

//Converts a P6 image to P3 format
int P6_P3(char* buffer, size_t BUFFER_SIZE, FILE *input, FILE *output) {
	char num = 0;

	while (fread(buffer, 1, 1, input)) {
		fprintf(output, "%u\n", (unsigned char)buffer[0]);
	}
	printf("Converted P3 to P6.\n");
	fclose(output);
	exit(0);
}

int main(int argc, char* argv[]) {

	FILE *input, *output;
	const size_t BUFFER_SIZE = 255;
	char *binary;
	char *buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);
	char current_format;
	char *format = argv[1];
	char *input_file = argv[2];
	char *output_file = argv[3];


	if (argc < 4) {
		fprintf(stderr, "Error: Incorrect number of parameters\n");
		exit(0);
	}

	//Input file
	input = fopen(input_file, "r");

	if (input == NULL) {
		fprintf(stderr, "Error: The file does not exist\n");
		exit(0);
	}

	if (buffer == NULL) {
		fprintf(stderr, "Error: Buffer does not exist");
		exit(0);
	}

	//Output file
	output = fopen(output_file, "w");
	if (output == NULL) {
		fprintf(stderr, "Error: This file did not exist, created that file");
		exit(0);
	}

	current_format = fgetc(input);

	//Checks if it is a PPM file
	if (current_format != 'P') {
		fprintf(stderr, "Error: This is not a PPM file \n");
		exit(0);
	}

	current_format = fgetc(input);
	//if format value is the same 'magic' number then reads the same data
	//from input to output

	if (((current_format) == '3' && (strcmp(format, "3") == 0)) || ((current_format) == '6' && (strcmp(format, "6")) == 0)) {
		fseek(input, 0, SEEK_SET);
		printf("Converting to exisiting format\n");
		same_type(buffer, BUFFER_SIZE, input, output);
		return 0;
	}

	//Skip through the comments of the PPM file (if any)
	fgets(buffer, BUFFER_SIZE, input);
	fgets(buffer, BUFFER_SIZE, input);

	while (buffer[0] == '#') {
		fgets(buffer, BUFFER_SIZE, input);
	}

	//Get MAX_VAL from PPM file
	//Then realloc's memory acording to Width/Height

	int i = 0;

	while (buffer[i] != ' ') {
		i++;
	} buffer[i] = '\0';

	int width = atoi(buffer);
	int height = atoi(buffer + i + 1);

	fgets(buffer, BUFFER_SIZE, input);
	int MAX_VAL = atoi(buffer);

	if (MAX_VAL > 255) {
		fprintf(stderr, "Error: This file contains more than 1 byte per pixel");
	}

	if(MAX_VAL < 1) {
		fprintf(stderr, "Error: Minimum value cannot be less than one");
	}

	buffer = (char*)realloc(buffer, width*height * 3);

	fprintf(output, "P%s\n", format);
	fprintf(output, "%d %d\n", width, height);
	fprintf(output, "%d\n", MAX_VAL);

	if ((current_format == '6') && (strcmp(format, "3") == 0)) {
		P6_P3(buffer, BUFFER_SIZE, input, output);
		printf("%c\n", current_format);
	}

	if ((current_format == '3') && (strcmp(format, "6") == 0)) {
		P3_P6(buffer, BUFFER_SIZE, input, output);
		printf("%c\n", current_format);
	}

	fgets(buffer, BUFFER_SIZE, input);
	while (buffer[i] != ' ') {
		i++;
	} buffer[i] = '\0';

	fclose(input);
	return 0;
}
