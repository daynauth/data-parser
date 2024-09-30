#ifndef JSON_H
#define JSON_H

#include <stdlib.h>

#define LEFT_PAREN '{'
#define RIGHT_PAREN '}'
#define COLON ':'
#define COMMA ','
#define WS ' '


#define error(...) fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE);

#define _malloc(fp, size) ({ \
    fp = malloc((size)); \
    if(fp == NULL) {error("memory could not be allocated")} \
}) \

typedef enum json_type_{
    json_type_number,
    json_type_string,
    json_type_bool,
    json_type_object,
    json_type_array,
    json_type_literal
} json_type;

typedef double json_number;
typedef char * json_string;
typedef int json_bool;

struct json_object;
struct json_element;
struct json_elements;
struct json_member;

typedef struct json_object{
    struct json_element * element;
    int length;
}json_object;


typedef struct json_element{
    json_type type;
    char * key;
    union{
        json_number number;
        json_string string;
        json_bool boolean;
        json_object * object;
    } value;
} json_element;

typedef struct {
    json_element *element;
} json;



// Json Element functions
json_element json_element_init();
json_element json_element_init_with_key(const char * key);
json_element json_element_number(const char * key, double number);
json_element json_element_string(const char * key, const char * string);
json_element json_element_bool(const char * key, int boolean);
void json_element_print(json_element * element, FILE * fp);


// Functions for working with a json object ... eg. {"age" : 24}
// Creating an empty json object {}
json_object json_object_init();
void json_object_print(json_object *jo);
void json_object_add_element(json_object * jo, json_element * element);
void json_object_add_string(json_object * jo, const char * string, const char * value);
void json_object_add_number(json_object * jo, const char * string, double value);
void json_object_add_bool(json_object * jo, const char * string, int boolean);


#endif