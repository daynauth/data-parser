#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "json.h"
#include "parser.h"

static int pos = 0;
static int tok;

static int is_end(const char  * string){
    return string[pos] == EOF || string[pos] == '\0';
}

static int get_token(const char * string){
    return tok = string[pos++];
}

static int peek_token(const char * string){
    return string[pos];
}

static int ws(const int ch){
    return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

static int strip_ws(const char * string){
    while(ws(tok = get_token(string)));
    return tok;
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

    json_object_add_array(&jo4, "people", &jo3, length);
    json_object_print(&jo4);
}
