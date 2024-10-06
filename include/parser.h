#ifndef PARSER_H
#define PARSER_H

#include "json.h"

typedef struct token_iterator{
    int current;
    int length;
    const char * token_string;
} Token_iterator;


Token_iterator * TokIter_New(const char * token_string);
int TokIter_GrabNext(Token_iterator * iter);
int TokIter_HasNext(Token_iterator * iter);
int TokIter_PeekNext(Token_iterator * iter);
int TokIter_Last(Token_iterator * iter);
int TokIter_Previous(Token_iterator * iter);
int TokIter_GetIndex(Token_iterator * iter);
void TokIter_Free(Token_iterator * iter);

void test(void);
void parse_json(const char* string);
json_element parse_value();

#endif
