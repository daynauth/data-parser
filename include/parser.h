#ifndef PARSER_H
#define PARSER_H

#include "json.h"
#include "json_t.h"

#define RESULT_OK 0
#define INVALID_JSON_CHARACTER -1

typedef struct token_iterator{
    int current;
    int length;
    const char * token_string;
} Token_iterator;

typedef struct parser{
    Token_iterator * iter;
    Json * json;
} JsonParser;


Token_iterator * TokIter_New(const char * token_string);
int TokIter_GrabNext(Token_iterator * iter);
int TokIter_HasNext(Token_iterator * iter);
int TokIter_PeekNext(Token_iterator * iter);
int TokIter_Last(Token_iterator * iter);
int TokIter_Previous(Token_iterator * iter);
int TokIter_GetIndex(Token_iterator * iter);
void TokIter_Free(Token_iterator * iter);

JsonParser * Parser_New(const char * token_string);
json_element_t * Parser_Parse(JsonParser * self);
int Parser_free(JsonParser * self);

void test(void);
void parse_json(const char* string);
// json_element parse_value();
json_element_t parse_element();
json_element_t parse_value();

#endif
