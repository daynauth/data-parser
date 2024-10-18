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

void test_json(const char * path, int current, int total){
    char * jsonstr;
    read_file(path, &jsonstr);

    if(jsonstr == NULL){
        fprintf(stderr, "Could not read file\n");
        exit(EXIT_FAILURE);
    }


    JsonParser * parser = Parser_New(jsonstr);
    int outcome = Parser_Parse(parser);
    if(outcome == RESULT_OK){
        printf("Parse %d of %d Successfull\n", current, total);
        printf("%s\n", jsonstr);
        Json_print(parser->json);
        printf("\n");
    }else{
        fprintf(stderr, "Parse Failed for file %s \n", path);
        parser_error_log(parser->iter, outcome);
    }

    Parser_free(parser);

    free(jsonstr);
}

int main(void){
    char  * paths[] = {
        "./test_parsing/y_array_arraysWithSpaces.json",
        "./test_parsing/y_array_empty-string.json",
        "./test_parsing/y_array_empty.json",
        "./test_parsing/y_array_ending_with_newline.json",
        "./test_parsing/y_array_false.json",
        "./test_parsing/y_array_heterogeneous.json",
        "./test_parsing/y_array_null.json",
        "./test_parsing/y_array_with_1_and_newline.json",
        "./test_parsing/y_array_with_leading_space.json",
        "./test_parsing/y_array_with_several_null.json",
        "./test_parsing/y_array_with_trailing_space.json",
        "./test_parsing/y_number_0e+1.json",
        "./test_parsing/y_number_0e1.json",
        "./test_parsing/y_number_after_space.json",
        "./test_parsing/y_number_double_close_to_zero.json",
        "./test_parsing/y_number_int_with_exp.json",
        "./test_parsing/y_number_minus_zero.json",
        "./test_parsing/y_number_negative_int.json",
    };
    int total = sizeof(paths) / sizeof(paths[0]);
    for(int i = 0; i < total; i++)
        test_json(paths[i], i + 1, total);

    return 0;
}

