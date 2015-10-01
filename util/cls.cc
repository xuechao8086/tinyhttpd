#include <iostream>

class Object {
    public:
        Object();
        ~Object();
        Object(int num);
        int getNumber();
        void setNumber(int num);
    private:
        int m_num;
};

Object::Object() {
    m_num = 0;
}

Object::~Object() {
}

Object::Object(int num) {
    m_num = num;
}

int Object::getNumber() {
    return m_num;
}

void Object::setNumber(int num) {
    m_num = num;
}

int main(int argc, char **argv) {
    Object *o = new Object(5);
    std::cout<<o->getNumber()<<std::endl;
    delete o;
    return 0;
}
