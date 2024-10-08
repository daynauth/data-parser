#ifndef JSON_T_H
#define JSON_T_H

#include <stdlib.h>
#include <stdio.h>

enum json_type_t {
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_BOOL,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_LITERAL
};

typedef int json_char_t;

struct json_string_t {
    int length;
    json_char_t * string;
};

struct json_object_t;
struct json_array_t;
struct json_element_t;

union json_value_t {
    struct json_object_t * object;
    struct json_array_t * array;
    struct json_string_t string;
    double number;
    int boolean;
};

typedef struct json_element_t {
    enum json_type_t type;
    union json_value_t value;
} json_element_t;

struct json_array_t {
    int length;
    struct json_element_t * elements;
};

struct json_object_kv_t {
    json_char_t * key;
    struct json_element_t * element;
};

struct json_object_t {
    int length;
    int capacity;
    struct json_object_kv_t * elements;
};

typedef struct json_element_t Json;
Json * Json_init();
void Json_print(Json * json);


void json_print(FILE * fp, json_element_t * element);

struct json_object_t * json_object_create(void);
int json_object_destroy(struct json_object_t * object);
void json_object_t_add_element(struct json_object_t * jo, json_char_t * key, struct json_element_t * element);

#endif