#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "json.h"

json * new_json(){
    json * j = malloc(sizeof(json));
    j->element = NULL;
    return j;
}

void json_add_element(json * j, json_element * element){
    j->element = element;
}

void json_print(json * j){
    if(j->element == NULL)
        return;

    switch(j->element->type){
        case json_number:
            printf("%f\n", j->element->value.number);
            break;
        case json_string:
            printf("%s\n", j->element->value.string);
            break;
        default:
            break;
    }
}

void json_free(json * j){
    if(j->element != NULL)
        free(j->element);
    free(j);
}

json_element * new_json_element(json_type type){
    json_element * element = malloc(sizeof(json_element));
    element->type = type;
    return element;
}

json_element * new_json_number_element(double number){
    json_element * element = new_json_element(json_number);
    element->value.number = number;
    return element;
}

json_element * new_json_string_element(const char * string, int length){
    json_element * element = new_json_element(json_string);
    element->value.string = malloc(length + 1);
    strncpy(element->value.string, string, length);
    element->value.string[length] = '\0';
    return element;
}

json_element * new_json_object_element(json_member * members){
    json_element * element = new_json_element(json_object);
    element->value.object.members = members;
    return element;
}