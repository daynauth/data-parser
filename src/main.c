#include <stdio.h>
#include "endpoints.h"
#include "parser.h"

/**
 * @brief Read the file as a string
 * 
 * @param filename A const string for the filename
 * @param buffer Pass the address of a char pointer aka a double pointer to a char, *this 
 * needs to be freed by the caller*
 * @return int TODO: return an error code instead of exiting the program
 */
int read_file(const char * filename, char ** buffer){
    FILE * file = fopen(filename, "r");
    if(file == NULL){
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    int seek = fseek(file, 0, SEEK_END);
    if(seek != 0){
        fprintf(stderr, "Could not seek to end of file\n");
        exit(EXIT_FAILURE);
    }


    long size = ftell(file);
    rewind(file);

    *buffer = (char *)malloc(size + 1);
    if(*buffer == NULL){
        fprintf(stderr, "Could not allocate memory for file\n");
        exit(EXIT_FAILURE);
    }

    size_t bytes_read = fread(*buffer, 1, size, file);
    if(bytes_read != size){
        fprintf(stderr, "Could not read file\n");
        exit(EXIT_FAILURE);
    }

    (*buffer)[size] = '\0';

    fclose(file);
    return 1;
}

int main(void){
    char * jsonstr;
    read_file("./tests/ex3.json", &jsonstr);

    if(jsonstr == NULL){
        fprintf(stderr, "Could not read file\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", jsonstr);


    JsonParser * parser = Parser_New(jsonstr);
    Json * element = Parser_Parse(parser);
    Json_print(element);
    printf("\n");
    // test();

    Parser_free(parser);

    free(jsonstr);

    return 0;
}

