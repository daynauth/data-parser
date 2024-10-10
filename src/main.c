#include <stdio.h>
#include "endpoints.h"
#include "parser.h"

int main(void){
    JsonParser * parser = Parser_New("{\"name\" : \"John}");
    Json * element = Parser_Parse(parser);
    Json_print(element);
    printf("\n");
    // test();

    Parser_free(parser);
    return 0;
}

