#include <stdio.h>

int func(int i)
{
    if(i>(0x01<<31-1))
    {
        return i;
    }
    if(i%10000==0)
    {
        printf("func %d\n", i);
    }
    
    func(i+1);

    return i;
}


int main(int argc, char *argv[])
{
    func(1);
    return 0;
}
