#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// need for unicode
#include <wchar.h>
#include <locale.h>


#include "parser.h"

static inline int match(int ch, int pattern, const char * message){
    return ch == pattern? : fprintf(stderr, "%s\n", message);
}

static inline int end_of_string(int ch){
    return ch == EOF || ch == '\0';
}

static inline int is_whitespace(int ch){
    return ch == WS1 || ch == WS2 || ch == WS3 || ch == WS4;
}

static inline int is_integer(int ch){
    return ch >= '0' && ch <= '9';
}

static inline int is_json_integer(int ch){
    return is_integer(ch) || ch == '-';
}

static inline int is_json_string_literal(char * string){
    return strcmp(string, "true") == 0 || strcmp(string, "false") == 0 || strcmp(string, "null") == 0;
}

/**
 * Determine if the character is a json character
 * TODO: implement test for unicode characters
 */
static inline int is_json_char(const char ch){
    return isalpha(ch);
}

static inline int is_json_literal(const char ch){
    return ch == 't' || ch == 'f' || ch == 'n';
}

static int get_length_of_string(const char * string){
    int count = 0;
    while(!end_of_string(string[count]))count++;

    return count;
}

Token_iterator * TokIter_New(const char * token_string){
    Token_iterator * iter = malloc(sizeof(Token_iterator));
    iter->token_string = token_string;
    iter->current = 0;
    iter->length = get_length_of_string(token_string);

    return iter;
}

int TokIter_PeekNext(Token_iterator * iter){
    return iter->token_string[iter->current];
}

int TokIter_Last(Token_iterator * iter){
    return iter->token_string[iter->length - 1];
}

int TokIter_GetIndex(Token_iterator * iter){
    return iter->current;
}

int TokIter_HasNext(Token_iterator * iter){
    return iter->current < iter->length;
}

int TokIter_GrabNext(Token_iterator * iter){
    return iter->token_string[iter->current++];
}

void TokIter_Free(Token_iterator * iter){
    if(iter != NULL)
        free(iter);
}

int TokIter_Previous(Token_iterator * iter){
    return iter->token_string[iter->current - 1];
}


JsonParser * Parser_New(const char * token_string){
    JsonParser * parser = malloc(sizeof(JsonParser));
    parser->iter = TokIter_New(token_string);
    parser->json = Json_init();
    return parser;
}

static int Parser_parse_value(JsonParser * self, json_element_t * element);

static int Parser_parse_number(JsonParser * self, json_element_t * element){
    int pos = TokIter_GetIndex(self->iter) - 1;

    while(isdigit(TokIter_PeekNext(self->iter))){
        TokIter_GrabNext(self->iter);
    }

    if(TokIter_PeekNext(self->iter) == '.'){
        TokIter_GrabNext(self->iter);
        while(isdigit(TokIter_PeekNext(self->iter))){
            TokIter_GrabNext(self->iter);
        }
    }

    int length = TokIter_GetIndex(self->iter) - pos;
    char * number = malloc(length + 1);
    strncpy(number, self->iter->token_string + pos, length);
    number[length] = '\0';

    double num = atof(number);
    free(number);

    element->type = JSON_TYPE_NUMBER;
    element->value.number = num;

    return RESULT_OK;
}

static int Parser_parse_value(JsonParser * self, json_element_t * element){
    int ch = TokIter_GrabNext(self->iter);
    switch(ch){
        case '\"':
            break;
        case '{':
            break;
        default:
            if(isdigit(ch)){
                Parser_parse_number(self, element);
            }
            else {
                return INVALID_JSON_CHARACTER;
            }
            
    }
    return RESULT_OK;
}

json_element_t * Parser_Parse(JsonParser * self){
    int outcome = Parser_parse_value(self, self->json);
    if(outcome != RESULT_OK){
        switch(outcome){
            case INVALID_JSON_CHARACTER:
                fprintf(stderr, "Invalid json character");
                break;
            default:
                fprintf(stderr, "Unknown error");
                break;
        }
    }
    return self->json;
}

int Parser_free(JsonParser * self){
    if(self == NULL)
        return 0;

    TokIter_Free(self->iter);
    free(self);
    return 1;
}


static Token_iterator * iter;



/**
 * Parse the json string for acii characters
 * TODO: implement a unicode parser
 */
static void parse_json_string(json_char_t ** key){
    int start = TokIter_GetIndex(iter);

    while(is_json_char(TokIter_PeekNext(iter))){
        TokIter_GrabNext(iter);
    }

    int length = TokIter_GetIndex(iter) - start;
    *key = malloc(length + 1);
    strncpy(*key, iter->token_string + start, length);
    (*key)[length] = '\0';
}

static void skip_whitespace(){
    while(is_whitespace(TokIter_PeekNext(iter))){
        TokIter_GrabNext(iter);
    }
}



//unicode check
// setlocale(LC_CTYPE, "");
// wchar_t temp = 0x03A9;
// fwprintf(stdout, L"%lc\n", temp);
static json_element_t parse_json_object(){
    //get the key
    json_char_t * key = NULL;
    match(TokIter_GrabNext(iter), QUOTATION, "Opening quotation not found");
    parse_json_string(&key);
    match(TokIter_GrabNext(iter), QUOTATION, "Closing quotation not found");
    
    //check for seperator
    match(TokIter_GrabNext(iter), COLON, "Seperator not found");

    //get the value
    skip_whitespace();
    json_element_t object_element = parse_value();

    struct json_object_t * object = json_object_create();
    json_object_t_add_element(object, key, &object_element);

    json_element_t element = {.type = JSON_TYPE_OBJECT, .value.object = object};

    //check for closing brace
    match(TokIter_GrabNext(iter), RIGHT_PAREN, "Closing brace not found");

    return element;
}

json_element_t parse_string(){

    int pos = TokIter_GetIndex(iter);

    while(TokIter_PeekNext(iter) != '\"'){
        TokIter_GrabNext(iter);
    }


    int length = TokIter_GetIndex(iter) - pos;
    match(TokIter_GrabNext(iter), QUOTATION, "Closing quotation not found");

    struct json_string_t string = {.length = length};
    string.string = malloc(length + 1);
    strncpy(string.string, iter->token_string + pos, length);
    string.string[length] = '\0';

    json_element_t element = {.type = JSON_TYPE_STRING, .value.string = string};

    return element;
}

json_element parse_literatl(){
    int pos = TokIter_GetIndex(iter);
    while(isalpha(TokIter_PeekNext(iter))){
        TokIter_GrabNext(iter);
    }

    int length = TokIter_GetIndex(iter) - pos;
    char * literal = malloc(length + 1);
    strncpy(literal, iter->token_string + pos, length);
    literal[length] = '\0';
    

    json_element element = {.type = json_type_literal};
    element.value.string = literal;

    return element;
}

json_element_t parse_number(){
    int pos = TokIter_GetIndex(iter) - 1;

    while(isdigit(TokIter_PeekNext(iter))){
        TokIter_GrabNext(iter);
    }

    if(TokIter_PeekNext(iter) == '.'){
        TokIter_GrabNext(iter);
        while(isdigit(TokIter_PeekNext(iter))){
            TokIter_GrabNext(iter);
        }
    }

    int length = TokIter_GetIndex(iter) - pos;
    char * number = malloc(length + 1);
    strncpy(number, iter->token_string + pos, length);
    number[length] = '\0';

    double num = atof(number);
    free(number);

    json_element_t element = {.type = JSON_TYPE_NUMBER, .value.number = num};

    return element;
}

json_element_t parse_value(){
    int ch = TokIter_GrabNext(iter);
    json_element_t element;

    switch(ch){
        case '\"':
            element = parse_string();
            break;
        case '{':
            element = parse_json_object();
            break;
        default:
            if(isdigit(ch)){
                element = parse_number();
            }
            else {
                error("Invalid json character");
            }
            
    }

    return element;
}

json_element_t parse_element(){
    skip_whitespace();
    json_element_t element = parse_value();
    skip_whitespace();
    return element;
}

void parse_json(const char* string){
    printf("%s\n", string); //debugging

    iter = TokIter_New(string);
    // json_element element = parse_element();
    json_element_t element = parse_element();
    // printf("Length: %d\n", element.value.object->length);
    // json_element_print(&element, stdout);
    json_print(stdout, &element);
    printf("\n");
    TokIter_Free(iter);
}



void test(void){
    json_object jo = {};

    json_object_add(&jo, "age", 23.1);
    json_object_add(&jo, "name", "bruce wayne");
    json_object_add(&jo, "tall", 0);


    json_object jo2 = {};
    json_object_add(&jo2, "address", "Gotham City");

    json_object_print(&jo);
    json_object_print(&jo2);


    json_object * jo3[] = {&jo, &jo2};
    size_t length = sizeof(jo3) / sizeof(jo3[0]);

    json_object jo4 = {};

    json_object_add_array(&jo4, "people", jo3, length);
    json_object_print(&jo4);
}
