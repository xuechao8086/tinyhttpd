#include "head.h"
#define DEBUG 0

void dicenum();
void dicenum_register(int64_t);
void calcavg();
int print_out(int total, int same);
int calc_birthday(int num, int (*callback)(int total, int same));

int main(int argc, char *argv[])  {
    char bf[1024];
    sprintf(bf, "%d %s %d", 100, "xuechao", 50);
    std::cout<<"bf = "<<bf<<std::endl;



    for(int i = 50; i < 256; ++i) {
        calc_birthday(i, print_out);
    }
    return 0;
    calcavg();
    return 0;

    int64_t test_arr[] = {99999,
                          999999,
                          9999999,
                          99999999,
                          999999999,
    };

    for (int i = 0; i < 5; ++i) {
        dicenum_register(test_arr[i]);
    }

    return 0;
    
    int64_t begin = time(NULL);
    sleep((uint32_t) 10);
    
    time_t sec;
    time(&sec);

    struct tm *now = localtime(&sec);
    std::cout<<now->tm_hour<<":"<<now->tm_min<<std::endl;
    
    return 0;
    dicenum();
    dicenum_register(99999999);
    int64_t end = time(NULL);

    std::cout<<"cost time "<<end-begin<<"s"<<std::endl;
    
    return 0;
}

void dicenum() {
    int64_t beg = time(NULL);
    srand(time(NULL));
    //register int64_t r[6] = {0, 0, 0, 0, 0, 0};
    int64_t r[6] = {0, 0, 0, 0, 0, 0};
    
    std::cout<<"RAND_MAX = "<<RAND_MAX<<std::endl;

    for(int64_t i = 0; i < 999999999;  ++i) {
        for(int8_t j = 0; j < 6; ++j) {
            ++r[rand()%6];
        }
    }

    for(int8_t i = 0; i < 6; ++i) {
        std::cout<<i+1<<", "<<r[i]<<std::endl;
    }
    
    int64_t end = time(NULL);

    std::cout<<"dicenum cost "<<end-beg<<"s"<<std::endl;
}

void dicenum_register(int64_t max) {
    int64_t beg = time(NULL);
    srand(time(NULL));
    register int64_t r[6] = {0, 0, 0, 0, 0, 0};
    
    for(register int64_t i = 0; i < max;  ++i) {
        for(register int8_t j = 0; j < 6; ++j) {
            ++r[rand()%6];
        }
    }

    for(int8_t i = 0; i < 6; ++i) {
        std::cout<<i+1<<", "<<r[i]<<std::endl;
    }

    int64_t end = time(NULL);

    std::cout<<max<<", dicenum_register cost "<<end-beg<<"s"<<std::endl;

}

void calcavg() {
    FILE *fp = fopen("./16.dat", "r");
    
    char *lineptr = NULL;
    size_t n = 0;
    ssize_t cnt = 0;
    while((cnt = getline(&lineptr, &n, fp)) != -1) {
        std::cout<<lineptr;
        // how to convert to int?
        int64_t sum = 0;
        int c = 0;
        for(int i = 0, beg = 0; i < cnt; ++i) {
            if(*(lineptr+i) == ' ' || *(lineptr +i) == '\n') {
                // int j = i-1;
                // while(j > -1 && *(lineptr+j) != ' ') {
                //    --j;
                // }
                // int64_t num = strtol(lineptr+j+1,  NULL, 10);
                

                int32_t num = strtol(lineptr+beg, NULL, 10);
                sum += num;
                beg = i + 1;
                ++c;

                std::cout<<"num = "<<num<<std::endl;;
            }
        }
        float avg = (float) sum/c;
        std::cout<<"avg = "<<avg<<std::endl;
    } 
    free(lineptr);
    fclose(fp);
}

int calc_birthday(int cnt, int (*callback)(int total, int same)) {
    srand(time(NULL));
    int birthday[cnt]; 
    // memset(birthday, -1, cnt); //note: it is error!
    
    memset(birthday, -1, sizeof(birthday));
    #if DEBUG
    for(int i = 0; i < cnt; ++i) {
        std::cout<<birthday[i]<<" ";
    }
    std::cout<<std::endl;
    #endif
    for(int i = 0; i < cnt; ++i) {
        birthday[i] = random()%356;
        #if DEBUG
        std::cout<<birthday[i]<<" ";
        #endif
    }

    int same = 0;
    for(int i = 0; i < cnt; ++i) {
        for(int j = i+1; j < cnt; ++j) {
            if (birthday[i] == birthday[j]) {
                ++same;
            }
        }
    }

    callback(cnt, same);
    return same;
} 
    

int print_out(int total, int same) {
    std::cout<<"total = "<<total<<", same = "<<same<<std::endl;
}
