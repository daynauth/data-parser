#ifndef JSON_H
#define JSON_H

#include <stdlib.h>

#define LEFT_PAREN '{'
#define RIGHT_PAREN '}'
#define LEFT_BRACKET '['
#define RIGHT_BRACKET ']'
#define COLON ':'
#define COMMA ','
#define QUOTATION '"'
#define WS1 ' '
#define WS2 '\t'
#define WS3 '\n'
#define WS4 '\r'


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

// struct json_object;
struct json_element;

typedef struct json_object{
    struct json_element * element;
    int length;
}json_object;

typedef struct json_array{
    struct json_object **objects;
    int length;
} json_array;


typedef struct json_element{
    json_type type;
    char * key;
    union{
        json_number number;
        json_string string;
        json_bool boolean;
        json_object * object;
        json_array array;
    } value;
} json_element;

#define json_value(type, value) (json_element){.type = type, .value = value}




// Json Element functions
json_element json_element_init(void);
json_element json_element_init_with_key(const char * key);
json_element json_element_number(double number);
json_element json_element_string(const char * string);
json_element json_element_bool(int boolean);
json_element json_element_object_number(const char * key, double number);
json_element json_element_object_string(const char * key, const char * string);
json_element json_element_object_bool(const char * key, int boolean);
json_element json_element_object_object(const char * key, json_object * object);
json_element json_element_object_array(const char * key, json_object ** array, size_t length);
void json_element_print(json_element * element, FILE * fp);


// Functions for working with a json object ... eg. {"age" : 24}
// Creating an empty json object {}
json_object json_object_init(void);
void json_object_print(json_object *jo);
void json_object_add_element(json_object * jo, json_element * element);
void json_object_add_string(json_object * jo, const char * string, const char * value);
void json_object_add_number(json_object * jo, const char * string, double value);
void json_object_add_bool(json_object * jo, const char * string, int boolean);
void json_object_add_object(json_object * jo, const char * string, json_object * object);
void json_object_add_array(json_object * jo, const char * string, json_object ** array, size_t length);


#define json_object_add(jo, key, value) _Generic((value), \
    char *: json_object_add_string, \
    double: json_object_add_number, \
    int: json_object_add_bool, \
    json_object *: json_object_add_object \
)(jo, key, value)


#endif
