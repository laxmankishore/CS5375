/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *trace_file_name;

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

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

//
void read_file_hit_miss(int num_of_blocks, int n_way, int sizeofBlock)
{

    int noOfSets = num_of_blocks / n_way;
    struct direct_mapped_cache
    {
        unsigned valid_field[num_of_blocks]; /* Valid field */
        unsigned dirty_field[num_of_blocks]; /* Dirty field; since we don't distinguish writes and \\
                                             reads in this project yet, this field doesn't really matter */
        uint64_t tag_field[num_of_blocks];   /* Tag field */
        int hits;                         /* Hit count */
        int misses;                       /* Miss count */
    };
    struct direct_mapped_cache d_cache;
   // Iterates through the num of blocks and updates all the field of d_cache
    for (int i = 0; i < num_of_blocks; i++)
    {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    char mem_request[20];
    // Accesing the pointers as it will be used to access memory efficiently
    FILE *fp;
    fp = fopen(trace_file_name, "r");
    uint64_t address;

    // Iterates till null
    while (fgets(mem_request, 20, fp) != NULL)
    {

        address = convert_address(mem_request);
        uint64_t block_addr = address >> (unsigned)log2(sizeofBlock);
        int setNumber = block_addr % noOfSets;
        uint64_t tag = block_addr >> (unsigned)log2(noOfSets);
        int start_Ind = ((int)setNumber) * n_way;

        int end_Ind = start_Ind + n_way - 1;

        int hitMade = 0;
        int is_Space_Empty = 0;
        int n_Temp = n_way;
        int ind = start_Ind;
        int i = 0;
        //measuring the num of hits
        while (n_Temp > 0)
        {
            i++;
            if (d_cache.valid_field[ind] && d_cache.tag_field[ind] == tag)
            {
                d_cache.hits += 1;
                hitMade = 1;
                break;
            }
            if (d_cache.valid_field[ind] == 0)
            {
                is_Space_Empty = 1;
            }

            ind += 1;
            n_Temp--;
        }
         //measuring the num of misses
        if (hitMade == 0)
        {
            d_cache.misses += 1;
            if (is_Space_Empty > 0)
            {
                n_Temp = n_way;
                ind = start_Ind;
                while (n_Temp > 0)
                {
                    if (d_cache.valid_field[ind] == 0)
                    {
                        d_cache.valid_field[ind] = 1;
                        d_cache.tag_field[ind] = tag;
                        break;
                    }

                    ind += 1;
                    n_Temp--;
                }
            }
            else
            {
             int rand_Ind = (rand() % (end_Ind - start_Ind + 1)) + start_Ind;
                d_cache.valid_field[rand_Ind] = 1;
                d_cache.tag_field[rand_Ind] = tag;
            }
        }
    }

   // Displaying the hits and misses in all the cases
    printf("==================================\n");
    printf("Cache Hits:    %d\n", d_cache.hits);
    printf("Cache Misses:  %d\n", d_cache.misses);
    printf("Cache Hit Rate : %0.2f%%\n", ((float)d_cache.hits / (float)(d_cache.hits + d_cache.misses)) * 100);
    printf("Cache Miss Rate : %0.2f%%\n", ((float)d_cache.misses / (float)(d_cache.hits + d_cache.misses)) * 100);
    printf("\n");
    fclose(fp);
}
// Show the cache of all different possibilities
void startProcess(int cacheSize, int num_of_blocks, int n_way, int sizeofBlock)
{
        read_file_hit_miss(num_of_blocks, n_way, sizeofBlock);
}



//fixed cache size of 32KB, test the fully-associative, 8-way set associative, 4-way set associative, and 2-way set associative cache with cache line size of 16 bytes, 32 bytes, and 128 bytes, respectively.
void Nway_map_32KB_cache()
{
    int continuepicking = 0;
    do
    {
        long cacheSize = 32 * 1024;
        int num_of_blocks = 0;
        int choice = 0;
        printf("For Cache size of 32KB\n");
        printf("Click 1 for 16 bytes cache line size\n");
        printf("Click 2 for 32 bytes cache line size\n");
        printf("Click 3 for 128 bytes cache line size : \n");
        scanf("%d", &choice);
        int selection = 0;
        printf("Select 1 for fully associative\n");
        printf("Select 2 for 2 way\n");
        printf("Select 4 for 4 way\n");
        printf("Select 8 for 8 way\n");
        scanf("%d", &selection);

        // checks for all the cases with cache of 32kb
        switch (choice)
        {
        case 1:
            printf("\n\n====== Executing for 16 BYTES cache line size ======= \n");
            num_of_blocks = (int)cacheSize / 16;
            switch (selection)
            {
            case 8:
                startProcess(32, num_of_blocks, 8, 16);
                break;
            case 4:
                startProcess(32, num_of_blocks, 4, 16);
                break;
            case 2:
                startProcess(32, num_of_blocks, 2, 16);
                break;
            case 1:
                startProcess(32, num_of_blocks, num_of_blocks, 16);
                break;
            default:
                break;
            }

            break;
        case 2:
            printf("\n\n====== Executing for 32 BYTES cache line size ====== \n");
            num_of_blocks = (int)cacheSize / 32;
            switch (selection)
            {
            case 8:
                startProcess(32, num_of_blocks, 8, 32);
                break;
            case 4:
                startProcess(32, num_of_blocks, 4, 32);
                break;
            case 2:
                startProcess(32, num_of_blocks, 2, 32);
                break;
            case 1:
                startProcess(32, num_of_blocks, num_of_blocks, 32);
                break;
            default:
                break;
            }

            break;
        case 3:
            printf("\n\n====== Executing for 128 BYTES cache line size ====== \n");
            num_of_blocks = (int)cacheSize / 128;
            switch (selection)
            {
            case 8:
                startProcess(32, num_of_blocks, 8, 128);
                break;
            case 4:
                startProcess(32, num_of_blocks, 4, 128);
                break;
            case 2:
                startProcess(32, num_of_blocks, 2, 128);
                break;
            case 1:
                startProcess(32, num_of_blocks, num_of_blocks, 1628);
                break;
            default:
                break;
            }

            break;
        }

        printf("Press 1 to continue, else exit: ");
        scanf("%d", &continuepicking);
    } while (continuepicking == 1);
}


// focuses on Q2 fixed cache line size of 64 bytes, test the fully-associative, 8-way set associative, 4way set associative, and 2-way set associative cache with the cache size of 16KB, 32KB and 64KB,
void Nway_map_64bytes()
{

    int continuepicking = 0;
    do
    {
        long cacheSize = 16 * 1024;
        int num_of_blocks = 0;
        int choice = 0;
        printf("For Cache line size of 64Bytes\n");
        printf("Click 1  for 16KB Cachesize\n");
        printf("Click 2  for 32KB Cachesize\n");
        printf("Click 3  for 64KB Cachesize\n");
        scanf("%d", &choice);
        int selection;
        printf("Select 1 for  fully associative\n");
        printf("Select 2 for  2 way execution\n");
        printf("Select 4 for  4 way execution\n");
        printf("Select 8 for  8 way execution\n");
        scanf("%d", &selection);
        // checks for all the cases with 64bytes
        switch (choice)
        {
        case 1:
            num_of_blocks = (int)cacheSize / 64;
            printf("\n\n==== Executing for 16kb cache size and 64 bytes cache line size/block size====\n");
            switch (selection)
            {
            case 8:
                startProcess(16, num_of_blocks, 8, 64);
                break;
            case 4:
                startProcess(16, num_of_blocks, 4, 64);
                break;
            case 2:
                startProcess(16, num_of_blocks, 2, 64);
                break;
            case 1:
                startProcess(16, num_of_blocks, num_of_blocks, 64);
                break;

            default:
                break;
            }

            break;
        case 2:
            printf("\n\n==== Executing for 32kb cache size and 64 bytes cache line size/block size=====\n");
            cacheSize = 32 * 1024;
            num_of_blocks = (int)cacheSize / 64;
            switch (selection)
            {
            case 8:
                startProcess(32, num_of_blocks, 8, 64);
                break;
            case 4:
                startProcess(32, num_of_blocks, 4, 64);
                break;
            case 2:
                startProcess(32, num_of_blocks, 2, 64);
                break;
            case 1:
                startProcess(32, num_of_blocks, num_of_blocks, 64);
                break;

            default:
                break;
            }
            break;
        case 3:
            printf("\n\n==== Executing for 64kb cache size and 64 bytes cache line size/block size======\n");
            cacheSize = 64 * 1024;
            num_of_blocks = (int)cacheSize / 64;
            switch (selection)
            {
            case 8:
                startProcess(64, num_of_blocks, 8, 64);
                break;
            case 4:
                startProcess(64, num_of_blocks, 4, 64);
                break;
            case 2:
                startProcess(64, num_of_blocks, 2, 64);
                break;
            case 1:
                startProcess(64, num_of_blocks, num_of_blocks, 64);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        printf("Select 1 to continue test cases,other key exit: ");
        scanf("%d", &continuepicking);
    } while (continuepicking == 1);
}

int main(int argc, char *argv[])
{
    trace_file_name = argv[2];
    int option = 0;
    printf("Select 1 to run Q 1\n");
    printf("Select 2 to run Q 2\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        Nway_map_32KB_cache();
        break;

    case 2:
        Nway_map_64bytes();
        break;
    }
}