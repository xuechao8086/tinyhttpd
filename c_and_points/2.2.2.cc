#include "2.2.2.h"

BraceMatch::BraceMatch() {
    _val = NULL;
}

int BraceMatch::input() {
    while((_val = getchar()) != '\n') {
        switch(_val) {
            case '(':
                _mark[PARENTHESIS] += 1;
                break;
            case ')':
                _mark[PARENTHESIS] -= 1;
                break;
            case '{':
                _mark[BRACE] += 1;
                break;
            case '}':
                _mark[BRACE] -= 1;
                break;
            default:
                continue;
        }
    }
    return 0;
}

int BraceMatch::match() {
    _match(_mark, 0, '(', ')');
    _match(_mark, 1, '{', '}');
    return 0;
}

int BraceMatch::_match(int *mark, int pos, char left, char right) {
    _val = *(mark + pos);
    if(_val > 0) {
        std::cout<<"'"<<left<<"' > '"<<right<<"'"<<std::endl;
    }
    else if (_val == 0) {
        std::cout<<"'"<<left<<"' = '"<<right<<"'"<<std::endl;
    }
    else {
        std::cout<<"'"<<left<<"' < '"<<right<<"'"<<std::endl;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    std::cout<<"this program check brackets match perfect"<<std::endl;
    BraceMatch bm;
    bm.input();
    bm.match();
    return 0;
}
