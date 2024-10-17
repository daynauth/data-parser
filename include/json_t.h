#ifndef JSON_T_H
#define JSON_T_H

#include <stdlib.h>
#include <stdio.h>

#define LEFT_PAREN '{'
#define RIGHT_PAREN '}'
#define LEFT_BRACKET '['
#define RIGHT_BRACKET ']'
#define COLON ':'
#define COMMA ','
#define QUOTATION '\"'
#define WS1 ' '
#define WS2 '\t'
#define WS3 '\n'
#define WS4 '\r'


enum json_type_t {
    JSON_TYPE_NUMBER = 1,
    JSON_TYPE_STRING = 2,
    JSON_TYPE_BOOL = 3,
    JSON_TYPE_OBJECT = 4,
    JSON_TYPE_ARRAY = 5,
    JSON_TYPE_LITERAL = 6
};

typedef int json_char_t;

struct json_object_t;
typedef struct json_array_t json_array_t;
struct json_element_t;

struct json_string_t {
    int length;
    json_char_t * string;
};

union json_value_t {
    struct json_object_t * object;
    struct json_array_t * array;
    struct json_string_t * string;
    double number;
    int boolean;
};

typedef struct json_element_t {
    enum json_type_t type;
    union json_value_t value;
} json_element_t;

struct json_array_t {
    int length;
    int capacity;
    struct json_element_t ** elements;
};

struct json_object_kv_t {
    struct json_string_t * key;
    struct json_element_t * element;
};

struct json_object_t {
    int length;
    int capacity;
    struct json_object_kv_t * elements;
};

struct json_string_t * json_string_create(int length);
void json_string_destroy(struct json_string_t * js);


typedef struct json_element_t Json;
Json * Json_init();
void Json_destroy(Json * json);
void Json_print(Json * json);


void json_print(FILE * fp, json_element_t * element);

struct json_object_t * json_object_create(void);
int json_object_destroy(struct json_object_t * object);
void json_object_t_add_element(struct json_object_t * jo, struct json_string_t * key, struct json_element_t * element);


// Json Array Functions
json_array_t * json_array_init();
void json_array_add_element(json_array_t * array, json_element_t * element);
void json_array_destroy(json_array_t * array);
void json_array_print(FILE * fp, json_array_t * array);

#endif