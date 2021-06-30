#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COMMAND_SIZE 1024*1024

#define TRUE 1
#define FALSE 0

char* _line = "==========================================================";

char* get_CPP_File(char* name) {
	char* buffer = (char*) malloc((strlen(name) + 4) * sizeof(char));
	sprintf(buffer, "%s.cpp", name);
	return buffer;
}

char* getExecutableFile(char* name) {
	char* get_CPP_File(char* name);
	char* buffer = (char*) malloc((strlen(name) + 2) * sizeof(char));
	sprintf(buffer, "./%s", name);
	return buffer;
}

void RunProgram(char* program) {
	int run_result = system(program);	
	printf("\n%s\nEXIT_CODE : %d\n\n", _line, run_result);
}


int main(int c, char ** args) {
	/* args [_main, cxx file, output, run_wc]*/

	if (c < 2) {
		printf("%s <FILE> <RUN_WC? [any]>\n", args[0]);
		return -1;
	}

	char* file = args[1];
	int run_wc = (c > 2 ? TRUE : FALSE);

	char* executable_file = getExecutableFile(file);
	char* cpp_file = get_CPP_File(file);

	if (run_wc == TRUE) {
		printf("[*] - Running Program [./%s] ...\n", executable_file);
		printf("%s\n", _line);
		RunProgram(executable_file);
		return 1;
	}

	char* command = (char*) malloc(COMMAND_SIZE * sizeof(char));
	sprintf(command, "clang++ %s -o %s -lGLU -lGL -lglfw -lGLEW -pthread -Wint-to-pointer-cast -Wall -Wformat=0 -Wunused-const-variable", 
		get_CPP_File(file), getExecutableFile(file));

	printf("[*] - Executing build command for CPP File [%s] , output file : [%s] ...\n", cpp_file, file);
	int build_result = system(command);

	if (build_result != 0) {
		printf("[*] - Failed to build [%s], Build result : [%d]\n", cpp_file, build_result);
		return 0;
	}

	printf("[+] - Built successfully, Executing [%s] ...\n", executable_file);
	printf("%s\n", _line);
	
	int run_result = system(executable_file);

	printf("%s\n", _line);
	printf("[*] - Program Exited with code [%d]\n", run_result);

	return 1;
}



