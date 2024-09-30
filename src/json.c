
#include <stdio.h>
#include <string.h>

#include "json.h"

//help functions go here
static void print_string(FILE * fp, const char * string){
    fprintf(fp, "\"%s\"", string);
}

static void print_tab(FILE * fp, int size){
    for(int i = 0; i < size; i++){
        fprintf(fp, "%c", WS);
    }
}

static void print_bool(FILE * fp, int boolean){
    if(boolean == 0)
        fprintf(fp, "false");
    else
        fprintf(fp, "true");
}

void json_element_print(json_element * element, FILE * fp){
    if(element == NULL)
        return;

    print_string(fp, element->key);
    fprintf(fp, " %c ", COLON);

    switch(element->type){
        case json_type_number:
            fprintf(fp, "%f", element->value.number);
            break;
        case json_type_string:
            print_string(fp, element->value.string);
            break;
        case json_type_bool:
            print_bool(fp, element->value.boolean);
        default:
            break;
    }  
}


json_element * new_json_element(json_type type){
    json_element * element = malloc(sizeof(json_element));
    element->type = type;
    return element;
}


// Json Element functions
// Create an empty json element
json_element json_element_init(){
    json_element element;
    return element;
}

json_element json_element_init_with_key(const char * key){
    json_element element = {.key = (char *)key};
    return element;
}

json_element json_element_number(const char* key, double number){
    return (json_element){.type = json_type_number, .key = (char *)key, .value.number = number};
}

json_element json_element_string(const char * key, const char * string){
    return (json_element){.type = json_type_string, .key = (char *)key, .value.string = (char*)string};
}

json_element json_element_bool(const char * key, int boolean){
    return (json_element){.type = json_type_bool, .key = (char *)key, .value.boolean = boolean};  
}

// Working with Json Objects

// Create an empty object
json_object  json_object_init(){
    json_object object = {.length = 0, .element = NULL};
    return object;
}


void json_object_print(json_object *jo){
    fprintf(stdout, "%c", LEFT_PAREN);
    for(int i = 0; i < jo->length; i++){
        json_element_print(&jo->element[i], stdout);
        if(i < jo->length - 1){
            fprintf(stdout, "%c ", COMMA);
        }
    }

    fprintf(stdout, "%c", RIGHT_PAREN);
}

void json_object_pretty_print(json_object *jo, int tab_size){
    
}

void json_object_add_element(json_object * jo, json_element * element){
    if(jo->element == NULL){
        _malloc(jo->element, sizeof(json_element) * 10); // TODO: fix hard coded array size
    }

    jo->element[jo->length++] = *element;
}

void json_object_add_string(json_object * jo, const char * string, const char * value){
    json_element element = json_element_string(string, value);
    json_object_add_element(jo, &element);
}

void json_object_add_number(json_object * jo, const char * string, double value){
    json_element  element = json_element_number(string, value);
    json_object_add_element(jo, &element);
}

void json_object_add_bool(json_object * jo, const char * string, int boolean){
    json_element  element = json_element_bool(string, boolean);
    json_object_add_element(jo, &element);
}