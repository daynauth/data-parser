#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

// need for unicode
// #include <wchar.h>
// #include <locale.h>


#include "parser.h"

static inline void copy_string(char * dest, const char * src, int length){
    strncpy(dest, src, length);
    dest[length] = '\0';
}



/**
 * @brief Compare two strings and return 1 if they are equal, 0 otherwise
 * 
 * @param string1 
 * @param string2 
 * @return int 
 */
static inline int string_is_equal(const char * string1, const char * string2){
    return strcmp(string1, string2) == 0;
}

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

/**
 * Check if character is 0 ... 9 or -
 */
static inline int is_json_integer(int ch){
    return is_integer(ch) || ch == '-';
}

static inline int is_json_string_literal(char * string){
    return string_is_equal(string, "true") || string_is_equal(string, "false") || string_is_equal(string, "null");
}

static void consume_token_while_true(Token_iterator * iter, int (*callback)(int)){
    while(callback(TokIter_PeekNext(iter))){
        TokIter_GrabNext(iter);
    }
}

static void skip_whitespace_token(Token_iterator * iter){
    consume_token_while_true(iter, is_whitespace);
}

/**
 * Determine if the character is a json character
 * TODO: implement test for unicode characters
 */
static inline int is_json_char(const int ch){
    return isalpha(ch) || isdigit(ch) || ch == '_' || ch == '-';
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
    iter->col = 1;
    iter->line = 1;

    return iter;
}
/**
 * @brief Take a look at the next character in the string without moving the iterator
 * 
 * @param iter 
 * @return int 
 */
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
    json_char_t ch = iter->token_string[iter->current++];

    if(ch == '\n'){
        iter->line++;
        iter->col = 0;
    }
    
    iter->col++;
    return ch;
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

static int JsonParser_parse_fraction(JsonParser * self, double * base){
    int count = 0;
    while(is_integer(TokIter_PeekNext(self->iter))){
        count++;
        *base = *base * 10 + (TokIter_GrabNext(self->iter) - '0');
    }

    *base = *base / pow(10, count);

    return RESULT_OK;
}


/**
 * @brief Parse the integer part of the number
 * integer :=> digit | onenine digits | '-' digit | '-' onenine digits
 * @param self 
 * @param base 
 * @return int 
 */
static int JsonParser_parse_integer(JsonParser * self, int * base){
    while(is_integer(TokIter_PeekNext(self->iter))){
        *base = *base * 10 + (TokIter_GrabNext(self->iter) - '0');
    }

    return RESULT_OK;
}

/**
 * @brief Parse the number element
 * Number :=> integer fraction exponent
 * 
 * @param self 
 * @param element 
 * @return int 
 */
static int JsonParser_parse_number_element(JsonParser * self, json_element_t * element){
    int integer = 0;
    int sign = 1;
    int ch = TokIter_Previous(self->iter);

    if(ch == '-'){
        sign = -1;
    }
    else{
        integer = ch - '0';
    }

    //parse the integer part
    MATCH_OK(JsonParser_parse_integer(self, &integer));

    // parse the fraction part
    double fraction = 0.0;
    if(TokIter_PeekNext(self->iter) == '.'){
        TokIter_GrabNext(self->iter);
        MATCH_OK(JsonParser_parse_fraction(self, &fraction));
    }


    int exponent = 1;
    if(TokIter_PeekNext(self->iter) == 'e' || TokIter_PeekNext(self->iter) == 'E'){
        TokIter_GrabNext(self->iter);
        //parse the exponent part
        if(TokIter_PeekNext(self->iter) == '-'){
            TokIter_GrabNext(self->iter);
            exponent = -1;
        }
        else if(TokIter_PeekNext(self->iter) == '+'){
            TokIter_GrabNext(self->iter);
        }


        MATCH_OK(JsonParser_parse_integer(self, &exponent));
        fraction = fraction * pow(10, exponent);
    }

    double number = (double)integer + fraction;
    number = number * sign;
    element->type = JSON_TYPE_NUMBER;
    element->value.number = number;

    return RESULT_OK;
}

static inline int JsonParser_get_string_length(JsonParser * self, int start_position){
    consume_token_while_true(self->iter, is_json_char);
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

static int JsonParser_parse_literal(JsonParser * self, json_element_t * element){
    int pos = TokIter_GetIndex(self->iter) - 1;
    consume_token_while_true(self->iter, is_json_char);

    int length = TokIter_GetIndex(self->iter) - pos;
    char * literal = malloc(length + 1);

    copy_string(literal, self->iter->token_string + pos, length);

    if(!is_json_string_literal(literal)){
        free(literal);
        return INVALID_JSON_CHARACTER;
    }

    element->type = JSON_TYPE_LITERAL;
    element->value.string = json_string_create(length);
    memcpy(element->value.string->string, literal, length);

    free(literal);
    return RESULT_OK;
}

/**
 * @brief Parse single member of the json object
 * ws string ws ':' element
 * @param self 
 * @param object 
 * @return int 
 */
static int JsonParser_parse_member(JsonParser * self, struct json_object_t * object){
        MATCH(TokIter_GrabNext(self->iter), QUOTATION, MISSING_QUOTATION);
        struct json_string_t * key = JsonParser_parse_string(self);
        MATCH(TokIter_GrabNext(self->iter), QUOTATION, MISSING_QUOTATION);
        skip_whitespace_token(self->iter);

        //check for seperator
        MATCH(TokIter_GrabNext(self->iter), COLON, MISSING_COLON);

        //get the value
        skip_whitespace_token(self->iter);
        json_element_t * object_element = Json_init();
        MATCH_OK(JsonParser_parse_value(self, object_element));

        json_object_t_add_element(object, key, object_element);

        return RESULT_OK;
}

/**
 * @brief Parse the list of members in the json object
 * member | member ',' members
 * 
 * @param self 
 * @param object 
 * @return int 
 */
static int JsonParser_parse_members(JsonParser * self, struct json_object_t * object){
    MATCH_OK(JsonParser_parse_member(self, object));
    skip_whitespace_token(self->iter);

    while(TokIter_PeekNext(self->iter) == COMMA){
        TokIter_GrabNext(self->iter);
        skip_whitespace_token(self->iter);
        MATCH_OK(JsonParser_parse_member(self, object));
    }

    return RESULT_OK;
}

/**
 * @brief Parse the json object
 * object :=> '{' ws '}' | '{' members '}'
 * 
 * @param self 
 * @param element 
 * @return int 
 */

static int JsonParser_parse_object(JsonParser * self, json_element_t * element){
    struct json_object_t * object = json_object_create();

    skip_whitespace_token(self->iter);
    if(TokIter_PeekNext(self->iter) == QUOTATION){
        MATCH_OK(JsonParser_parse_members(self, object));
    }
    skip_whitespace_token(self->iter);

    element->type = JSON_TYPE_OBJECT;
    element->value.object = object;

    return RESULT_OK;
}

/**
 * @brief Parse the json array
 * object :=> '[' ws ']' | '[' elements ']'
 * 
 * @param self 
 * @param element 
 * @return int 
 */
static int JsonParser_parse_array(JsonParser * self, json_element_t * element){
    json_array_t * array = json_array_init();
    element->type = JSON_TYPE_ARRAY;
    element->value.array = array;

    while(1){
        skip_whitespace_token(self->iter);

        if(TokIter_PeekNext(self->iter) == RIGHT_BRACKET)
            break;

        json_element_t * array_element = Json_init();
        MATCH_OK(JsonParser_parse_value(self, array_element));
        json_array_add_element(array, array_element);

        skip_whitespace_token(self->iter);

        if(TokIter_PeekNext(self->iter) == COMMA){
            TokIter_GrabNext(self->iter);
        }
    }


    return RESULT_OK;
}


/**
 * @brief Parse all the json values
 *  object | array | string | number | 'true' | 'false' | 'null'
 * 
 * @param self 
 * @param element 
 * @return int 
 */
static int JsonParser_parse_value(JsonParser * self, json_element_t * element){
    int ch = TokIter_GrabNext(self->iter);
    switch(ch){
        case QUOTATION: // string
            MATCH_OK(JsonParser_parse_string_element(self, element));
            MATCH(TokIter_GrabNext(self->iter), QUOTATION, MISSING_QUOTATION);
            break;
        case LEFT_PAREN: // object
            skip_whitespace_token(self->iter);
            MATCH_OK(JsonParser_parse_object(self, element));
            skip_whitespace_token(self->iter);
            MATCH(TokIter_GrabNext(self->iter), RIGHT_PAREN, MISSING_RIGHT_BRACKET);
            break;
        case LEFT_BRACKET: // array
            skip_whitespace_token(self->iter);
            MATCH_OK(JsonParser_parse_array(self, element));
            skip_whitespace_token(self->iter);
            MATCH(TokIter_GrabNext(self->iter), RIGHT_BRACKET, MISSING_RIGHT_BRACKET);
            break;
        default:
            if(is_json_integer(ch)){ // number
                JsonParser_parse_number_element(self, element);
            }
            else if(is_json_literal(ch)){
                MATCH_OK(JsonParser_parse_literal(self, element));
            }
            else {
                return INVALID_JSON_CHARACTER;
            }
            
    }
    return RESULT_OK;
}

static int Parser_parse_element(JsonParser * self, json_element_t * element){
    skip_whitespace_token(self->iter);
    MATCH_OK(JsonParser_parse_value(self, element));
    skip_whitespace_token(self->iter);

    if(TokIter_HasNext(self->iter)){
        return INVALID_JSON_CHARACTER;
    }

    return RESULT_OK;
}

/**
 * @brief Print the error message to the console
 * 
 * @param iter 
 */
// TODO: only output the current line, also add color support
static void print_error_debug(Token_iterator * iter){
    fprintf(stderr, "Error: Parse error at line %d:\n", iter->line);
    fprintf(stderr, "%s\n", iter->token_string);
    for(int i = 0; i < iter->col - 2; i++){
        fprintf(stderr, "-");
    }
    fprintf(stderr, "^\n");
}

void parser_error_log(Token_iterator * iter, int error){
    if (error == RESULT_OK)
        return;
        
    print_error_debug(iter);
    switch(error){
        case INVALID_JSON_CHARACTER:
            fprintf(stderr, "Expecting \'EOF\' character, ");
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
            fprintf(stderr, "Expected \'\"\', ");
            break;
        case MISSING_LEFT_BRACKET:
            fprintf(stderr, "Missing left bracket\n");
            break;
        case MISSING_RIGHT_BRACKET:
            fprintf(stderr, "Missing \'%c\' ", RIGHT_PAREN);
            break;
        default:
            fprintf(stderr, "Unknown error\n");
            break;
    }

    if(TokIter_PeekNext(iter) != EOF)
        fprintf(stderr, "got \'%c\' instead.\n", TokIter_Previous(iter));
    else
        fprintf(stderr, "got \'EOF\' instead.\n");

    
    exit(1);
}

int Parser_Parse(JsonParser * self){
    int outcome = Parser_parse_element(self, self->json);
    // parser_error_log(self->iter, outcome);
    return outcome;
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












