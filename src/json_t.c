#include "json_t.h"
#include <string.h>

Json * Json_init(){
    Json * json = malloc(sizeof(Json));
    return json;
}

void Json_print(Json * json){
    json_print(stdout, json);
}



void json_object_t_print(FILE * fp, struct json_object_t *jo){
    if(jo == NULL)
        return;

    for(int i = 0; i < jo->length; i++){
        struct json_object_kv_t kv = jo->elements[i];
        fprintf(stdout, "\"%s\": ", kv.key);
        json_print(stdout, kv.element);
        if(i < jo->length - 1){
            fprintf(stdout, ", ");
        }
    }
}

void json_print(FILE * fp, json_element_t * element){
    if(element == NULL)
        return;

    switch(element->type){
        case JSON_TYPE_STRING:
            fprintf(fp, "\"%s\"", element->value.string.string);
            break;
        case JSON_TYPE_NUMBER:
            fprintf(fp, "%f", element->value.number);
            break;
        case JSON_TYPE_OBJECT:
            fprintf(fp, "{ ");
            // fprintf(fp, "Length: %d\n", element->value.object->length);
            json_object_t_print(fp, element->value.object);
            fprintf(fp, " }");
            break;
        default:
            fprintf(stderr, "Unknown type");
            exit(1);
            break;
    }
}

static void json_object_resize(struct json_object_t * object){
    if(object->length < object->capacity)
        return;

    object->capacity *= 2;
    object->elements = realloc(object->elements, sizeof(struct json_object_kv_t) * object->capacity);
}

struct json_object_t * json_object_create(void){
    struct json_object_t * object = malloc(sizeof(struct json_object_t));
    object->length = 0;
    object->capacity = 10;
    object->elements = malloc(sizeof(struct json_object_kv_t) * object->capacity);
    return object;
}

int json_object_destroy(struct json_object_t * object){
    if(object == NULL)
        return 0;

    for(int i = 0; i < object->length; i++){
        free(object->elements[i].key);
        free(object->elements[i].element);
    }

    free(object->elements);
    free(object);
    return 1;
}

void json_object_t_add_element(struct json_object_t * jo, json_char_t * key, struct json_element_t * element){
    json_object_resize(jo);

    struct json_object_kv_t kv = {.key = key};
    kv.element = malloc(sizeof(struct json_element_t));
    memcpy(kv.element, element, sizeof(struct json_element_t));

    jo->elements[jo->length] = kv;
    jo->length++;
}