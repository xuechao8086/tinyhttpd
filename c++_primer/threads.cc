#include "head.h"
class Foo {
    public:
        Foo():
            flag_(0),
            thread1_(std::bind(&Foo::threadFunc1, this)),
            thread2_(std::bind(&Foo::threadFunc2, this))
        {
            strcpy(name, "charliezhao");
            thread_pool_run(); 
            //stack_display();
            
        }
        ~Foo() {
            thread1_.join();
            thread2_.join();
        }
        
        void thread_pool_run(void);

    private:
        void threadFunc1(void);
        void threadFunc2(void); 
        void thread_pool_func(void);

    private:
        int flag_;
        std::mutex mutex_;
        std::condition_variable cond_;
        std::thread thread1_;
        std::thread thread2_;
    
        char name[1024];

        std::mutex mutex2_;
        std::stack<char *> stack_;
        std::queue<char *> queue_;
        std::thread *threads[10];
        std::vector<std::thread *> v_threads;
        
};

void Foo::thread_pool_func(void) {
    sleep(1);
    while(!(stack_.empty())) {
        mutex2_.lock();
        char *buf = stack_.top();
        stack_.pop();
        mutex2_.unlock();
        std::this_thread::sleep_for (std::chrono::seconds(1)); 
        std::cout<<std::this_thread::get_id()<<" "<<buf<<std::endl;;
    
    }
}

void Foo::thread_pool_run(void) {
    for(int i = 0; i < 1024; ++i) {
        char *buf = (char *)malloc(1024*sizeof(char));
        memset(buf, '\0', 1024);
        sprintf(buf, "stack buf %d", i);
        stack_.push(buf); 
    }
    //not use queue right now.  
    //for(int i = 0; i < 1024; ++i) {
    //    char *buf = (char *)malloc(1024*sizeof(char));
    //    memset(buf, '\0', 1024);
    //    sprintf(buf, "queue buf %d", i);
    //    queue_.push(buf);
    //} 

    for(int i = 0; i < 10; ++i) {
        std::thread *t = new std::thread(std::bind(&Foo::thread_pool_func, this));
        v_threads.push_back(t);
    }

    for(int i = 0; i < 10; ++i) {
        v_threads[i]->join();
        delete v_threads[i];
    }
}

void Foo::threadFunc1(void) {
    std::unique_lock<std::mutex> ul(mutex_);
    while(0 == flag_) {
        cond_.wait(ul);
    }
    std::cout<<"flag_ = "<<flag_<<std::endl;
}

void Foo::threadFunc2(void) {
    std::this_thread::sleep_for((std::chrono::milliseconds(3000)));  
    std::unique_lock<std::mutex> ul(mutex_);  
    flag_ = 100;
    cond_.notify_one();
}

int main(int argc, char *argv[]) {
    Foo f;
    sleep(10); 
    return 0;
}
