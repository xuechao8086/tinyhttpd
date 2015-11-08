/*
 * todo:
 *  function args explain.
 *
 */
#include "head.h"

namespace declare {
    const int MAXTOKENS = 100;
    const int MAXTOKENLEN = 64;
    enum type_tag {IDENTIFIER, QUALIFIER, TYPE};
    
    struct token {
        char type;
        char string[MAXTOKENLEN];
    } stack[MAXTOKENS], that;

    int top = -1;
    
    #define pop stack[top--]
    #define push(s) stack[++top] = s

    enum type_tag classify_string(void) {
        char *s = that.string;
        if(!strcmp(s, "const")) {
            strcpy(s, "read-only");
            return QUALIFIER;
        }
        if(!strcmp(s, "volatile")) {
            strcpy(s, "volatile"); 
            return QUALIFIER;
        }

        const char *types[12] = {
            "void",
            "char",
            "signed",
            "unsigned",
            "short",
            "int",
            "long",
            "float",
            "double",
            "struct",
            "uinon",
            "enum"
        };
        for(int8_t i = 0; i < 12; ++i) {
            if(!strcmp(s, types[i])) {
                return TYPE;
            }
        }

        return IDENTIFIER;
    }

    void gettoken(void) {
        char *p = that.string;
        while((*p = getchar()) == ' '); //strip

        if(isalnum(*p)) {
            while(isalnum(*++p = getchar()));
            ungetc(*p, stdin);
            *p = '\0';

            that.type = classify_string();
            return;
        }

        if(*p == '*') {
            strcpy(that.string, "point to");
            that.type = '*';
            return;
        }

        that.string[1] = '\0';
        that.type = *p;
        return;
    }

    void read_to_first_identifer() {
        gettoken();
        while(that.type != IDENTIFIER) {
            push(that);
            gettoken();
        }

        std::cout<<that.string<<" is ";
        gettoken();
    }

    void deal_with_arrays() {
        while(that.type == '[') {
            std::cout<<"array ";
            gettoken();
            if(isdigit(that.string[0])) {
                std::cout<<"0.."<<(atoi(that.string)-1);
                gettoken();
            }
            gettoken();
            std::cout<<" of ";
        }
    }
    
    void deal_with_function_args() {
        while(that.type != ')') {
            gettoken();
        }
        gettoken();
        std::cout<<"function returning ";
    }

    void deal_with_pointers() {
        while(stack[top].type == '*') {
            std::cout<<stack[top--].string<<" ";
        }
    }

    void deal_with_declarator() {
        if(that.type == '[') {
            deal_with_arrays();
        }
        else if(that.type == '(') {
            deal_with_function_args();
        }

        deal_with_pointers();

        while(top>=0) {
            if(stack[top].type == '(') {
                pop;
                gettoken();
                deal_with_declarator();
            }
            else {
                std::cout<<pop.string<<" ";
            }
        }
    }

}

int main(int argc, char **argv) {
    using namespace declare;
    read_to_first_identifer();
    deal_with_declarator();
    std::cout<<std::endl;
    return 0;
}


