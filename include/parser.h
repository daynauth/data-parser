#ifndef PARSER_H
#define PARSER_H

#include "json.h"

#define RESULT_OK 0
#define INVALID_JSON_CHARACTER -1
#define MISSING_LEFT_BRACE -2
#define MISSING_RIGHT_BRACE -3
#define MISSING_COLON -4
#define MISSING_COMMA -5
#define MISSING_QUOTATION -6
#define MISSING_LEFT_BRACKET -7
#define MISSING_RIGHT_BRACKET -8

#define MATCH(ch, pattern, error_code) \
    do {                               \
        if ((ch) != (pattern)) {       \
            return (error_code);       \
        }                              \
    } while (0)

#define MATCH_OK(ch)                   \
({                                     \
    int _err = (ch);                   \
    MATCH(_err, RESULT_OK, _err);      \
})



typedef struct token_iterator{
    int current;
    int length;
    int col;
    int line;
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
int Parser_Parse(JsonParser * self);
int Parser_free(JsonParser * self);
void parser_error_log(Token_iterator * iter, int error);

#endif
