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
            printf("CHECKING INTEGRITY...\n");
            if(check_integrity(data,sizes[size_index])){
                free(data);
                return 1;
            }
        #endif
        
        end = clock();
        free(data);
        #if !DEBUG
            printf("%8.6f secs = %8.6e usecs\n",(double) (end-start)/CLOCKS_PER_SEC, (double)(end-start)/CLOCKS_PER_SEC*1.e6);
        #endif

        #if DEBUG
            printf("SEED: %4d\t",seeds[i]);
            printf("Elapsed time: %8.6f seconds = %8.6e useconds \n",(double) (end-start)/CLOCKS_PER_SEC, (double)(end-start)/CLOCKS_PER_SEC*1.e6);
        #endif
       
    }

    }
    return 0;
}
