#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

int print_line_one();
int print_line_two();

int main(int argc, char *argv[]) {
    return print_line_two();
}

int rearrange(char *output, char const *input,
        int const n_columns, int const columns[])
{
    int output_col = 0;
    int len = strlen(input);
    
    for(int col = 0; col < n_columns; col += 2) {
        int nchars = columns[col+1] - columns[col] + 1;
        if(columns[col] >= len) { 
            continue;
        }
        if(output_col == MAX_INPUT - 1){
            break;
        }
        if(output_col+nchars > MAX_INPUT-1) {
            nchars = MAX_INPUT - output_col -1;
        }
        if(columns[col]+nchars-1 >=len) {
            nchars = len - columns[col];
        }

        strncpy(output+output_col, input+columns[col],
                nchars);
        output_col += nchars;

    }
    output[output_col] = '\0';
    return 0;
}

int print_line_one(){
    int ch;
    int line = 0;
    int at_beginning = 1;

    while( (ch = getchar()) != EOF) {
        if(at_beginning == 1) {
            at_beginning = 0;
            line += 1;
            std::cout<<line<<" ";
        }
        putchar(ch);
        if(ch == '\n') {
            at_beginning = 1;
        }
    }

    return EXIT_SUCCESS;
}

int print_line_two() {
    char ch;
    bool at_beginning = true;
    int line = 0;
    
    while (true) {
        std::cin.get(ch);
        if (ch == EOF) {
            break;
        }
        if(at_beginning) {
            at_beginning = false;
            line += 1;
            std::cout<<line<<" ";
        }
        std::cout<<ch;
        if(ch == '\n') {
            at_beginning = true;
        }
    }

    return 0;
}


