#ifndef JSON_H
#define JSON_H

typedef enum json_type_{
    json_number,
    json_string,
    json_object,
    json_array,
    json_literal
} json_type;

typedef struct json_element_{
    union{
        double number;
    } value;
} json_element;

typedef struct {
    char string[255];
    json_element *element;
} json_member;

#endif