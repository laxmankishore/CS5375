#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
char *trace_file_name;
// creating a struct for L1cache
struct L1Cache{
    unsigned valid_field[1024];
    unsigned dirty_field[1024];
    uint64_t tag_field[1024];
    char data_field[1024][64];
    int hits;
    int misses;
};
// creates a struct for L2cache
struct L2Cache{
    unsigned valid_field[16384];
    unsigned dirty_field[16384];
    uint64_t tag_field[16384];
    char data_field[16384][64];
    int hits;
    int misses;
};
uint64_t convert_addr(char memory_addr[]){
    uint64_t binary = 0;
    int i = 0;
    for(int k=0;k<20;k++){
        k=k+2;
        }
    while (memory_addr[i] != '\n')
    {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0')
        {
            binary = (binary * 16) + (memory_addr[i] - '0');
        }
        else
        {
            if (memory_addr[i] == 'a' || memory_addr[i] == 'A')
            {
                binary = (binary * 16) + 10;
            }
            if (memory_addr[i] == 'b' || memory_addr[i] == 'B')
            {
                binary = (binary * 16) + 11;
            }
            if (memory_addr[i] == 'c' || memory_addr[i] == 'C')
            {
                binary = (binary * 16) + 12;
            }
            if (memory_addr[i] == 'd' || memory_addr[i] == 'D')
            {
                binary = (binary * 16) + 13;
            }
            if (memory_addr[i] == 'e' || memory_addr[i] == 'E')
            {
                binary = (binary * 16) + 14;
            }
            if (memory_addr[i] == 'f' || memory_addr[i] == 'F')
            {
                binary = (binary * 16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}
// data of L1 Cache
int Data_Cache_L1(uint64_t addr, int n_way, struct L1Cache *l1)
{
    uint64_t block_addr = addr >> (unsigned)log2(64);
    int setNumber = block_addr % 512;
    uint64_t tag = block_addr >> (unsigned)log2(512);
    int start_Ind = ((int)setNumber) * n_way;
    int n_temp = n_way;
    int ind = start_Ind;
    while (n_temp > 0){
        if (l1->valid_field[ind] && l1->tag_field[ind] == tag){
            return 1;
        }
        ind += 1;
        n_temp--;
    }
    return 0;
}

// data to L2 cache
int Data_Cache_L2(uint64_t addr, int n_way, struct L2Cache *l2)
{
    uint64_t block_addr = addr >> (unsigned)log2(64);
    int setNumber = block_addr % 2048;
    uint64_t tag = block_addr >> (unsigned)log2(2048);
    int start_Ind = ((int)setNumber) * n_way;
    int n_temp = n_way;
    int ind = start_Ind;
    // Iterates n_way times and updates the valid and tag fields
    while (n_temp > 0){
        if (l2->valid_field[ind] && l2->tag_field[ind] == tag) {
            return 1;
        }
        ind += 1;
        n_temp--;
    }
    return 0;
}

// writing data to L1 cache
void Write_to_L1Cache(uint64_t addr, int n_way, struct L1Cache *l1){
    uint64_t block_addr = addr >> (unsigned)log2(64);
    int setNumber = block_addr % 512;
    uint64_t tag = block_addr >> (unsigned)log2(512);
    int start_Ind = ((int)setNumber) * n_way;
    int n_temp = n_way;
    int ind = start_Ind;
    int is_Space_Empty = 0;
    int end_Ind = start_Ind + n_way - 1;
    while (n_temp > 0){
        if (l1->valid_field[ind] == 0){
            is_Space_Empty = 1; }
        ind++;
        n_temp--;}
    if (is_Space_Empty > 0){
        n_temp = n_way;
        ind = start_Ind;
        while (n_temp > 0){
            if (l1->valid_field[ind] == 0){
                l1->valid_field[ind] = 1;
                l1->tag_field[ind] = tag;
                break; }
            ind += 1;
            n_temp--;
        }}
    else {
        int randind = (rand() % (end_Ind - start_Ind + 1)) + start_Ind;
        l1->valid_field[randind] = 1;
        l1->tag_field[randind] = tag; }
}

// writing data to L2 cache
void Write_to_L2Cache(uint64_t addr, int n_way, struct L2Cache *l2){
    uint64_t block_addr = addr >> (unsigned)log2(64);
    int setNumber = block_addr % 2048;
    uint64_t tag = block_addr >> (unsigned)log2(2048);
    int start_Ind = ((int)setNumber) * n_way;
    int n_temp = n_way;
    int ind = start_Ind;
    int is_Space_Empty = 0;
    int end_Ind = start_Ind + n_way - 1;
    while (n_temp > 0){
        if (l2->valid_field[ind] == 0){
            is_Space_Empty = 1;}
        ind++;
        n_temp--;}
    if (is_Space_Empty > 0)
    { n_temp = n_way;
        ind = start_Ind;
        while (n_temp > 0){
            if (l2->valid_field[ind] == 0) {
                l2->valid_field[ind] = 1;
                l2->tag_field[ind] = tag;
                break;}
            ind += 1;
            n_temp--;}
    }
    else{
        int randind = (rand() % (end_Ind - start_Ind + 1)) + start_Ind;
        l2->valid_field[randind] = 1;
        l2->tag_field[randind] = tag;}
}

int main(int argc, char *argv[]){
    FILE *fp;
    trace_file_name = argv[2];
    char memory_request[20];
    uint64_t addr;
    struct L1Cache l1;
    struct L2Cache l2;
    int num_of_Blocks_l1 = 1024;
    int num_of_Blocks_l2 = 16384;
    int l1_n_way = 2;
    int l2_n_way = 8;
    int numberOfSetsl1 = 512;
    int numberOfSetsl2 = 2048;
    // Updates the valid_field,dirty and tag field of l1 cache for  all the num_of_Blocks_l1
    for (int i = 0; i < num_of_Blocks_l1; i++)
    {
        l1.valid_field[i] = 0;
        l1.dirty_field[i] = 0;
        l1.tag_field[i] = 0;
    }
    // Updates the valid_field,dirty and tag field of l2 cache for  all the num_of_Blocks_l2

    for (int i = 0; i < num_of_Blocks_l2; i++)
    {
        l2.valid_field[i] = 0;
        l2.dirty_field[i] = 0;
        l2.tag_field[i] = 0;
    }
    l1.hits = 0;
    l1.misses = 0;
    l2.hits = 0;
    l2.misses = 0;
    // opening the trace file
    fp = fopen(trace_file_name, "r");
    if (strncmp(argv[1], "direct", 6) == 0){

        // updates the hits in L1 cache and misses in L1 and Hits in L2 cache along with L2 misses
        while (fgets(memory_request, 20, fp) != NULL){
            addr = convert_addr(memory_request);
            int data_L1 = Data_Cache_L1(addr, l1_n_way, &l1);


            if (data_L1 == 1){
                l1.hits++;
                l2.hits++;
            }
            else{
                l1.misses++;
                int data_L2 = Data_Cache_L2(addr, l2_n_way, &l2);
                if (data_L2){
                    l2.hits += 1;
                }
                else{
                    l2.misses++;
                    Write_to_L2Cache(addr, l2_n_way, &l2);
                }
                Write_to_L1Cache(addr, l1_n_way, &l1);  }
        }
        printf("Cache type:    l1\n");
        printf("======================================\n");
        printf("Cache Hits:  %d\n", l1.hits);
        printf("Cache Misses:  %d\n", l1.misses);
        printf("Cache Hit Rate: %0.2f%%\n", ((float)l1.hits / (float)(l1.hits + l1.misses)) * 100);
        printf("Cache Miss Rate: %0.2f%%\n", ((float)l1.misses / (float)(l1.hits + l1.misses)) * 100);
        printf("\n");
        printf("\n======================================\n");
        printf("Cache type:  l2\n");
        printf("=========================================\n");
        printf("Cache Hits: %d\n", l2.hits);
        printf("Cache Misses: %d\n", l2.misses);
        printf("Cache Hit Rate: %0.2f%%\n", ((float)l2.hits / (float)(l2.hits + l2.misses)) * 100);
        printf("Cache Miss Rate: %0.2f%%\n", ((float)l2.misses / (float)(l2.hits + l2.misses)) * 100);
        printf("\n");
    }
    fclose(fp);
    return 0;
}