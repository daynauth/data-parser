#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// need for unicode
// #include <wchar.h>
// #include <locale.h>

#include "json.h"
#include "parser.h"

static int pos = 0;
static int tok;

static int is_end(const char  * string){
    return string[pos] == EOF || string[pos] == '\0';
}

static int get_token(const char * string){
    return string[pos++];
}

static int peek_token(const char * string){
    return string[pos];
}

static int ws(const int ch){
    return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

static int strip_ws(const char * string){
    int ch;

    do{
        ch = get_token(string);
    }while(ws(ch));

    return ch;
}

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
static char * parse_json_string(const char * string){
    char * json_string;
    return json_string;
}

static void parse_json_object(const char * string){
    //get the key
    int ch = strip_ws(string);
    if(ch != '\"'){
        fprintf(stderr, "token not recongized\n");
    }

    char * key = parse_json_string(string);

    // int start = pos;

    //unicode check
    // setlocale(LC_CTYPE, "");
    // wchar_t temp = 0x03A9;
    // fwprintf(stdout, L"%lc\n", temp);

    //check for seperator
    //get the value
    //check for closing brace
}

void parse_json(const char* string){
    int ch = strip_ws(string);
    switch(ch){
        case '{':
            parse_json_object(string);
            break;
        default:
            break;
    }

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
