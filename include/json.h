#ifndef JSON_H
#define JSON_H

typedef enum json_type_{
    json_number,
    json_string,
    json_object,
    json_array,
    json_literal
} json_type;

struct json_element;

typedef struct json_member_{
    char * key;
    struct json_element * value;
} json_member;


typedef struct json_element{
    json_type type;
    union{
        double number;
        char * string;
        struct object { json_member * members;} object;
    } value;
} json_element;

typedef struct {
    json_element *element;
} json;


json * new_json();
void json_add_element(json * j, json_element * element);
void json_print(json * j);
void json_free(json * j);

json_element * new_json_number_element(double number);
json_element * new_json_string_element(const char * string, int length);
json_element * new_json_object_element(json_member * members);

#endif