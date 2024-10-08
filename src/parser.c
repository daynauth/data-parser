#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// need for unicode
// #include <wchar.h>
// #include <locale.h>


#include "parser.h"

static inline int match(int ch, int pattern, const int error_code){
    return ch == pattern? RESULT_OK : error_code;
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

static void skip_whitespace_token(Token_iterator * iter){
    while(is_whitespace(TokIter_PeekNext(iter))){
        TokIter_GrabNext(iter);
    }
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

static int JsonParser_parse_value(JsonParser * self, json_element_t * element);

static int JsonParser_parse_number_element(JsonParser * self, json_element_t * element){
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

static int JsonParser_get_string_length(JsonParser * self, int start_position){
    while(is_json_char(TokIter_PeekNext(self->iter))){
        TokIter_GrabNext(self->iter);
    }

    return TokIter_GetIndex(self->iter) - start_position;
}

static struct json_string_t * JsonParser_parse_string(JsonParser * self){
    int pos = TokIter_GetIndex(self->iter);
    int length = JsonParser_get_string_length(self, pos);
    struct json_string_t * string = json_string_create(length);

    strncpy(string->string, self->iter->token_string + pos, string->length);
    string->string[string->length] = '\0';

    return string;
}

static int JsonParser_parse_string_element(JsonParser * self, json_element_t * element){
    struct json_string_t * string = JsonParser_parse_string(self);
    

    element->type = JSON_TYPE_STRING;
    element->value.string = string;

    return RESULT_OK;

}

static int JsonParser_parse_object(JsonParser * self, json_element_t * element){
    //get the key
    MATCH(TokIter_GrabNext(self->iter), QUOTATION, MISSING_QUOTATION);
    struct json_string_t * key = JsonParser_parse_string(self);
    MATCH(TokIter_GrabNext(self->iter), QUOTATION, MISSING_QUOTATION);
    
    //check for seperator
    MATCH(TokIter_GrabNext(self->iter), COLON, MISSING_COLON);

    //get the value
    skip_whitespace_token(self->iter);
    json_element_t * object_element = Json_init();
    JsonParser_parse_value(self, object_element);

    struct json_object_t * object = json_object_create();
    json_object_t_add_element(object, key, object_element);

    element->type = JSON_TYPE_OBJECT;
    element->value.object = object;

    return RESULT_OK;
}


static int JsonParser_parse_value(JsonParser * self, json_element_t * element){
    int ch = TokIter_GrabNext(self->iter);
    switch(ch){
        case '\"':
            JsonParser_parse_string_element(self, element);
            MATCH(TokIter_GrabNext(self->iter), QUOTATION, MISSING_QUOTATION);
            break;
        case '{':
            JsonParser_parse_object(self, element);
            MATCH(TokIter_GrabNext(self->iter), RIGHT_PAREN, MISSING_RIGHT_BRACKET);
            break;
        default:
            if(isdigit(ch)){
                JsonParser_parse_number_element(self, element);
            }
            else {
                return INVALID_JSON_CHARACTER;
            }
            
    }
    return RESULT_OK;
}

static int Parser_parse_element(JsonParser * self, json_element_t * element){
    skip_whitespace_token(self->iter);
    return JsonParser_parse_value(self, element);
}

static void error_log(int error){
    if (error == RESULT_OK)
        return;
        
    switch(error){
        case INVALID_JSON_CHARACTER:
            fprintf(stderr, "Invalid json character\n");
            break;
        case MISSING_LEFT_BRACE:
            fprintf(stderr, "Missing left brace");
            break;
        case MISSING_RIGHT_BRACE:
            fprintf(stderr, "Missing right brace\n");
            break;
        case MISSING_COLON:
            fprintf(stderr, "Missing colon\n");
            break;
        case MISSING_COMMA:
            fprintf(stderr, "Missing comma\n");
            break;
        case MISSING_QUOTATION:
            fprintf(stderr, "Missing quotation\n");
            break;
        case MISSING_LEFT_BRACKET:
            fprintf(stderr, "Missing left bracket\n");
            break;
        case MISSING_RIGHT_BRACKET:
            fprintf(stderr, "Missing right bracket\n");
            break;
        default:
            fprintf(stderr, "Unknown error\n");
            break;
    }
    exit(1);
}

json_element_t * Parser_Parse(JsonParser * self){
    int outcome = Parser_parse_element(self, self->json);
    error_log(outcome);
    return self->json;
}

int Parser_free(JsonParser * self){
    if(self == NULL)
        return 0;

    TokIter_Free(self->iter);
    Json_destroy(self->json);

    free(self);
    return 1;
}
/**
 * Parse the json string for acii characters
 * TODO: implement a unicode parser
 */


//unicode check
// setlocale(LC_CTYPE, "");
// wchar_t temp = 0x03A9;
// fwprintf(stdout, L"%lc\n", temp);












