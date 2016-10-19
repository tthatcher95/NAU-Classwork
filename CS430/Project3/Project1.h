#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
