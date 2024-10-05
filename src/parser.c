#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// need for unicode
// #include <wchar.h>
// #include <locale.h>

#include "json.h"
#include "parser.h"

static int end_of_string(int ch){
    return ch == EOF || ch == '\0';
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

void TokITer_Free(Token_iterator * iter){
    if(iter != NULL)
        free(iter);
}


static Token_iterator * iter;

/**
 * Determine if the character is a json character
 * TODO: implement test for unicode characters
 */
static int is_json_char(const char ch){
    return isalpha(ch);
}

/**
 * Parse the json string for acii characters
 * TODO: implement a unicode parser
 */
static void parse_json_string(char ** key){
    int start = TokIter_GetIndex(iter);

    while(is_json_char(TokIter_PeekNext(iter))){
        TokIter_GrabNext(iter);
    }

    int length = TokIter_GetIndex(iter) - start;
    *key = malloc(length + 1);
    strncpy(*key, iter->token_string + start, length);
    (*key)[length] = '\0';
}

static int match(int ch, int pattern, const char * message){
    return ch == pattern? : fprintf(stderr, "%s\n", message);
}


//unicode check
// setlocale(LC_CTYPE, "");
// wchar_t temp = 0x03A9;
// fwprintf(stdout, L"%lc\n", temp);
static void parse_json_object(){
    //get the key
    char * key = NULL;
    match(TokIter_GrabNext(iter), QUOTATION, "Opening quotation not found");
    parse_json_string(&key);
    match(TokIter_GrabNext(iter), QUOTATION, "Closing quotation not found");
    
    //check for seperator
    match(TokIter_GrabNext(iter), COLON, "Seperator not found");

    //get the value
    //check for closing brace
}

void parse_json(const char* string){
    iter = TokIter_New(string);

    int ch = TokIter_GrabNext(iter);

    switch(ch){
        case '{':
            parse_json_object();
            break;
        default:
            break;
    }

    TokITer_Free(iter);
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
