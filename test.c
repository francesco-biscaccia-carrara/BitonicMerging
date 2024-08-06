#include "utils/parallel.h"
#include "utils/test.h"


int main(void) {
    int seeds[] = SEEDS;
    for(int i=0;seeds[i]>0;i++){
        uintmax_t start,end;
        int* data = (int*) malloc(INST_SIZE*sizeof(int));
        new_instance(data,seeds[i]);
        start = clock();

        bitonic_sort(data,0,INST_SIZE-1,1);

    
        #if DEBUG == 1 
            printf("CHECKING INTEGRITY...\n");
            if(check_integrity(data,INST_SIZE)) return 1;
        #endif
        end = clock();
        free(data);
        printf("Elapsed time: %ju -%ju = %ju ticks\n",end,start,end-start);
        printf("Elapsed time: %8.6f seconds == %8.6e seconds \n",(double) (end-start)/CLOCKS_PER_SEC,(double)(end-start)/CLOCKS_PER_SEC);
    }
    return 0;
}
