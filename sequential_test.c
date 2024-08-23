#include "utils/headers/sequential.h"
#include "utils/headers/test.h"


int main(void) {
    
    int seeds[] = SEEDS;
    int sizes[] = INST_SIZES;
    
    for(int size_index = 0; sizes[size_index]>0;size_index++){

        #if !DEBUG
            printf("----%d-----\n",sizes[size_index]);
        #endif

        for(int seed_index = 0; seeds[seed_index] > 0; seed_index++){
        uintmax_t start,end;
        int* data = (int*) malloc(sizes[size_index]*sizeof(int));
        new_instance(data,seeds[seed_index],sizes[size_index]);
        start = clock();

        merge_sort(data,0,sizes[size_index]-1);

        #if DEBUG == 1 
            printf("CHECKING INTEGRITY...");
            if(check_integrity(data,sizes[size_index])){
                free(data);
                return 1;
            }
            printf("PASSED!\n");
        #endif
        
        end = clock();
        free(data);
        #if !DEBUG
            printf("%8.6f\n",(double) (end-start)/CLOCKS_PER_SEC);
        #endif

        #if DEBUG
            printf("SEED: %4d\t",seeds[size_index]);
            printf("Elapsed time: %8.6f seconds\n",(double) (end-start)/CLOCKS_PER_SEC);
        #endif
       
    }

    }
    return 0;
}
