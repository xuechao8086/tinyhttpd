#include <time.h>
#include <sys/time.h>

#include <iostream>
const bool debug = false;

class PregancyTime {
    public:
        PregancyTime();
        
        ~PregancyTime();
        
        void calc();
        void display(void) const;

    private:
        const char *format;
        std::string s_beg;
        std::string s_end;

        time_t beg;
        time_t end;
        
        int days;
        int weeks;
        int total_days;
};

PregancyTime::PregancyTime() {
    struct tm tm;
    format = "%Y-%m-%d %H:%M:%S";
    s_beg = "2015-08-19 00:00:00";
    strptime(s_beg.c_str(), format, &tm);
    if(debug) {
        std::cout<<"begin"<<std::endl;
        std::cout<<"tm_sec="<<tm.tm_sec<<std::endl;
        std::cout<<"tm_mday="<<tm.tm_mday<<std::endl;
        std::cout<<"tm_wday="<<tm.tm_wday<<std::endl;
    }
    
    beg = mktime(&tm);

    time(&end);
    
    struct tm *p_tm = gmtime(&end);

    char buf[1024] = "\0";
    strftime(buf, 1024, "%Y-%m-%d %H:%M:%S", p_tm);
    free(p_tm);
    s_end = buf; 
    if(debug) {
        std::cout<<"end"<<std::endl;
        std::cout<<"tm_sec="<<p_tm->tm_sec<<std::endl;
        std::cout<<"tm_min="<<p_tm->tm_min<<std::endl;
        std::cout<<"tm_hour="<<p_tm->tm_hour<<std::endl;
        std::cout<<"buf = "<<buf<<std::endl;
    }

    days = 0;
    weeks = 0;
}


PregancyTime::~PregancyTime() {}

void PregancyTime::calc() {
    total_days = (end-beg)/3600/24;
    weeks = total_days/7;
    days = total_days%7;
    display();
}

void PregancyTime::display(void) const{
    std::cout<<"from "<<s_beg.substr(0, 10)<<" ";
    std::cout<<"to "<<s_end.substr(0, 10)<<", ";
    std::cout<<weeks<<" weeks ";
    std::cout<<days<<" days, ";
    std::cout<<"total "<<total_days<<" days";
    std::cout<<"(less then "<<weeks+1<<" weeks)";
    std::cout<<std::endl;
}

int main(int argc, char *argv[]) {
    PregancyTime pt;
    pt.calc();

    return 0;
}

