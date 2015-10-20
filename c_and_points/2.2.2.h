#include <stdio.h>
#include <stdlib.h>

#include <iostream>

class BraceMatch
{
    public:
        BraceMatch();

    public:
        int input();
        int match();

    private:
        int _match(int *mark, int pos, char left, char right);
       
    private:
        enum type {PARENTHESIS , BRACE};
        int _mark[4] = {0, 0, 0, 0};
        char _val;
};
