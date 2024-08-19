#include "utils/headers/sequential.h"
#include "utils/headers/test.h"


int main(void) {
    #if !DEBUG
        printf("----%d-----\n",INST_SIZE);
    #endif

    int seeds[] = SEEDS;
    for(int seed_index = 0; seeds[seed_index] > 0; seed_index++){
        uintmax_t start,end;
        int* data = (int*) malloc(INST_SIZE*sizeof(int));
        new_instance(data,seeds[seed_index]);
        start = clock();

        merge_sort(data,0,INST_SIZE-1);

        #if DEBUG == 1 
            printf("CHECKING INTEGRITY...\n");
            if(check_integrity(data,INST_SIZE)){
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
    return 0;
}
