#include "head.h"

bool identity_matrix(int (*arr)[3], int len1, int len2);
bool identity_matrix_two(int *matrix, int size);
void matrix_multiply(int *m1, int *m2, int *r,
        int x, int y, int z);
void print_matrix(int *matrix, int row, int column);

int main(int argc, char *argv[]) {

    int arr[][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}};
    
    bool result = identity_matrix(arr, 3, 3);
    std::cout<<result<<std::endl;
    int *matrix = (int *)arr[0];
    bool result2 = identity_matrix_two(matrix, 3);
    std::cout<<result2<<std::endl;

    int m1[3][2] = {{2, -6},
        {3, 5},
        {1, -1}};

    int m2[2][4] = {{4, -2, -4, -5},
                    {-7, -3, 6, 7}};

    int m3[3][4];
    
    matrix_multiply((int *)m1[0], (int *)m2[0], (int *)m3[0],
            3, 2, 4);
    
    print_matrix((int *)m1[0], 3, 2);
    print_matrix((int *)m2[0], 2, 4);
    print_matrix((int *)m3[0], 3, 4);
    return 0;
}

bool identity_matrix(int (*arr)[3], int len1, int len2) {

    if(len1 != len2) {
        return false;
    }
    
    for(int i = 0; i<len1; ++i) {
        for(int j = 0; j < len1; ++j) {
            if (i == j) { 
                if (*(*(arr+i)+j) != 1) {
                    return false;
                }
            }
            else {
                if(*(*(arr+i)+j) != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool identity_matrix_two(int *matrix, int size) {
    for(int row = 0;  row < size; ++row) {
        for(int column = 0; column < size; ++column) {
            if(*matrix++ != (row==column) ) {
                return false;
            }
        }
    }
    return true;
}

void matrix_multiply(int *m1, int *m2, int *r,
        int x, int y, int z) {

    //as for z!
    for(int i = 0; i < x; ++i) {
        for(int j = 0; j < z; ++j) {
            int sum = 0;
            for(int k = 0; k < y; ++k) {
                sum += *(m1+y*i+k) * *(m2+z*k+j);
            }
            *(r+i*z+j) = sum;
        }
    }
}

void print_matrix(int *matrix, int row, int column) {
    for(int i = 0; i < row; ++i) {
        for(int j = 0; j < column; ++j) {
            std::cout<<"matrix["<<i<<"]["<<j<<"] = "<<*matrix++<<", ";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}
