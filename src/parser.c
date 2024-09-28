#include <stdio.h>
#include <stdlib.h>

static int pos = 0;
static int tok;

static int is_end(const char  * string){
    return string[pos] == EOF || string[pos] == '\0';
}

static int get_token(const char * string){
    return string[pos++];
}

static int ws(const int ch){
    return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

static void strip_ws(const char * string){
    while(ws(tok = get_token(string)));
}

static void parse_element(const char * string){

}

void parse(const char * string){
    printf("%s\n", string);

    while(1){
        tok = get_token(string);

        if(is_end(tok)){
            
        }
    }
    strip_ws(string);


}