#include <stdio.h>
#include "endpoints.h"
#include "parser.h"

int main(void){
    JsonParser * parser = Parser_New("1");
    Json * element = Parser_Parse(parser);
    Json_print(element);
    // test();

    Parser_free(parser);
    return 0;
}

