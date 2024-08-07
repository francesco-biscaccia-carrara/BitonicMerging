#include "../headers/test.h"

int new_instance(int* data, int seed){
    if(data == NULL || seed < 0) return 1;
    srand(seed);

    for (int i = 0; i < INST_SIZE; i++) {
        data[i]=rand();
    }
    return 0;
}

int check_integrity(int* data,int size){
    for(int i=0;i<size-1;i++){
        if(data[i]>data[i+1]) return 1;
    }
    return 0;
}