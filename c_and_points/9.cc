#include "head.h"
int classify();
void my_strcpy(char *dest, const char *src, size_t n);
bool palindrome(const char *string);
void dollars(char *dest, const char *src);

class StringFormat {
    public:
        StringFormat(const char *format, const char *input);
        const char *process();
        void test_stack(); 
        ~StringFormat();
    
    private:
        const char *f;
        const char *i; 
        char *buffer;
        std::stack<char> st;
};

StringFormat::StringFormat(const char *format, const char *input) {
    f = format;
    i = input;
    buffer = (char *)malloc(sizeof(char)*1024);
    memset(buffer, (int)'\0', 1024);
}

StringFormat::~StringFormat() {
    free(buffer);
}

void StringFormat::test_stack() {
    std::stack<char> st;
    for(int i = 65; i < 97; ++i) {
        st.push((char)i);
    }
    while(!st.empty()) {
        std::cout<<"value = "<<st.top()<<std::endl;
        st.pop(); 
    }
}

const char * StringFormat::process() {
    int fl = strlen(f) - 1; //cann't use size_t;
    int il = strlen(i) - 1; //cann't use size_t;
    
    while(il >= 0) { //because for cmp;
        if(fl < 0) {
            break;
        }
        if (*(f + fl) == '#') { 
            st.push(*(i+il));
            --fl;
            --il;
        }
        else if (*(f+fl) == ',') {
            st.push(',');
            --fl;
        }
        else if(*(f+fl) == '.') {
            st.push('.');
            --fl;
        }
    }
    while(fl>=0) {
        if(*(f+fl) == '#') {
            st.push('_');
        }
        else if(*(f+fl) == '.') {
            st.push('.');
            st.push('0');
        }
        --fl;
    }

    bool mark_points = false;
    char val;

    for(int i = 0; !st.empty(); ++i) {
        val = st.top();
        
        if(mark_points) {
            if(val == '_') {
                val = '0';
            }
            else {
                mark_points = false;
            }
        }
        
        if(val == '.') {
            mark_points = true;
        }

        *(buffer+i) = val;
        st.pop();
    }
    
    return (const char *)buffer;
}


struct inputs {
    const char *format;
    const char *nums;
};


int main(int argc, char *argv[]) {
     inputs array[10]= {
         {"#####", "12345"},
         {"#####", "123"},
         {"##,###", "1234"},
         {"##,###", "123"},
         {"##,###", "1234567"},
         {"#,###,###.##", "123456789"},
         {"#,###,###.##", "1234567"},
         {"#,###,###.##", "123"},
         {"#,###,###.##", "1"},
         {"#####.#####", "1"}
    };
    
    StringFormat *sf[10];

    for(int8_t i = 0; i < 10; ++i) {
        sf[i] = new StringFormat(array[i].format, array[i].nums);
        std::cout<<"format:"<<array[i].format<<", input:"<<array[i].nums<<", result: "<<sf[i]->process()<<std::endl;
    }

    std::cout<<"========================"<<std::endl;
    
    return 0;

    sf[1]->test_stack();
    for(int8_t i = 0; i < 10; ++i) {
        delete sf[i];
    }


    std::string s("xuechao zhao");
    std::cout<<"s = "<<s<<std::endl;
    std::cout<<"s.length() = "<<s.length()<<std::endl;
    std::cout<<"s.size() = "<<s.size()<<std::endl;
    std::cout<<"s.max_size() = "<<s.max_size()<<std::endl;
    std::cout<<"s.capacity() = "<<s.capacity()<<std::endl;
    const char *sc = s.c_str();
    std::cout<<"sc = "<<sc<<std::endl;
    
    std::string s2 = s + " ting ting zhang";
    std::cout<<"s2 = "<<s2<<std::endl;
   
    
    std::string *ps = new std::string("hello, point of string");
    std::cout<<"ps = "<<ps<<", *ps = "<<*ps<<std::endl;
    std::cout<<"ps->length() = "<<ps->length()<<std::endl;
    std::cout<<"ps->replace(0, 5, \"HELLO\") = "<<ps->replace(0, 5, "HELLO")<<std::endl;


    delete ps;

    char buffer_dollars[1024] = "";
    strcat(buffer_dollars, "xuechao zhao");
    strcat(buffer_dollars, "tingting zhang");
    std::cout<<"buffer_dollars = "<<buffer_dollars<<std::endl;
 
    char dest[1024] = {""};
    const char *src[10] = {"",
                         "1",
                         "12",
                         "123",
                         "1234", 
                         "12345",
                         "123456",
                         "1234567",
                         "12345678",
                         "123456789"
    };
    for(int8_t i = 0; i < 10; ++i) {
        dollars(dest, src[i]);
        std::cout<<"dest = "<<dest<<std::endl;
        memset(dest, '\0', 1024);
    }

    const char *string[] = {"xuechao zhao",
        "123456 654321",
        "1233211" };

    for(int i = 0; i < 3; ++i) {
        std::cout<<"palindrome("<<string[i]<<") = "<<palindrome(string[i])<<std::endl;
    }

    char buffer[1024];
    const char *name = "xuechao zhao";
    my_strcpy(buffer, name, sizeof(name));

    std::cout<<"buffer = "<<buffer<<std::endl;

    classify();
    return 0;
}

int classify() {
    int info[] = { 0,//int isalnum(int c);
        0, //int isalpha(int c);
        0, //int isascii(int c);
        0, //int isblank(int c);
        0, //int iscntrl(int c);
        0, //int isdigit(int c);
        0, //int isgraph(int c);
        0, //int islower(int c);
        0, //int isprint(int c);
        0, //int ispunct(int c);
        0, //int isspace(int c);
        0, //int isupper(int c);
        0, //int isxdigit(int c);
    };

    // note, the dif between char and int.
    int val;
    //char val; 
    while((val=getchar()) != '\n' ) {
        if(isalnum(val)) {
            ++info[0];
        }
    }
    std::cout<<"alpha num = "<<info[0]<<std::endl;

    return 1;
}

void my_strcpy(char *dest, const char *src, size_t n) {
    for(char *pos = (char *)src; *pos && n; --n) {
        *dest++=*pos++;
    } 
    *dest = '\0';
}

bool palindrome(const char *string) {
    for(size_t i = 0, j = strlen(string)-1; i<j; ++i, --j) {
        if(*(string+i) != *(string +j)) {
            return false;
        }
    }
    return true;
}

void dollars(char *dest, const char *src) {
    size_t slen = strlen(src);
    if(slen == 0) {
        strcat(dest, "$0.00");
        return; 
    } else if(slen == 1) {
        strcat(dest, "$0.0");
        strcat(dest, src);
        return;
    } else if(slen == 2) {
        strcat(dest, "$0.");
        strcat(dest, src);
    } else {
        size_t sleft = slen - 2;
        size_t sextra = sleft%3;

        strncat(dest, "$", 1);
        if(sextra) {
            strncat(dest, src, sextra); 
            if(sleft > 2) {
                strncat(dest, ",", 1);
            }
            src += sextra;
        }

        for(size_t i = sleft - sextra, cnt = 1; i != 0; --i, ++cnt) {
            strncat(dest, src++, 1);
            if(cnt%3 == 0 && cnt < i) {
                strncat(dest, ",", 1);
            }
        }
        strncat(dest, ".", 1);
        strncat(dest, src, 2);
    }
}
