#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "json.h"

static json * j = NULL;
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

static void parse_element(const char * string){

}

void parse_number_element(const char * string){
    double number = 0;
    
    while(isdigit(tok)){
        number = number * 10 + (tok - '0');
        get_token(string);
    }
    if(tok == '.'){
        double fraction = 0;
        double weight = 0.1;
        tok = get_token(string);
        while(isdigit(tok)){
            fraction += weight * (tok - '0');
            weight *= 0.1;
            tok = get_token(string);
        }
        number += fraction;
    }
    json_element * element = new_json_number_element(number);
    json_add_element(j, element);
}

void parse_string_element(const char * string){
    char buffer[255];
    int i = 0;
    while(1){
        tok = get_token(string);
        if(tok == '\"'){
            break;
        }
        buffer[i++] = tok;
    }
    buffer[i] = '\0';
    json_element * element = new_json_string_element(buffer, i);
    json_add_element(j, element);
}


void parse(const char * string){
    printf("%s\n", string);
    int ch = ' ';
    j = new_json();

    while(1){
        if(is_end(string)){
            break;
        }

        ch = get_token(string);

        switch(ch){
            case '\"':
                parse_string_element(string);
                break;
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                break;
            default:
                if(isdigit(ch)){
                    parse_number_element(string);
                }
                break;
        }
    }

    json_print(j);
    json_free(j);
}