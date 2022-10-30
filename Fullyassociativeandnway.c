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

void readFileAndDoCacheHitOrMiss(int noOfBlocks, int n_way, int sizeofBlock)
{
    // Every thing is ready , just call the file and do the hit
    int noOfSets = noOfBlocks / n_way;
    struct direct_mapped_cache
    {
        unsigned valid_field[noOfBlocks]; /* Valid field */
        unsigned dirty_field[noOfBlocks]; /* Dirty field; since we don't distinguish writes and \\
                                             reads in this project yet, this field doesn't really matter */
        uint64_t tag_field[noOfBlocks];   /* Tag field */
        int hits;                         /* Hit count */
        int misses;                       /* Miss count */
    };
    struct direct_mapped_cache d_cache;
    /* Initialization */
    for (int i = 0; i < noOfBlocks; i++)
    {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    char mem_request[20];
    FILE *fp;
    fp = fopen(trace_file_name, "r");
    uint64_t address;

    // printf("%s IS THE FILENAME", trace_file_name);
    while (fgets(mem_request, 20, fp) != NULL)
    {
        // printf("%s", mem_request);
        address = convert_address(mem_request);
        uint64_t block_addr = address >> (unsigned)log2(sizeofBlock);
        int setNumber = block_addr % noOfSets;
        uint64_t tag = block_addr >> (unsigned)log2(noOfSets);
        int startIndex = ((int)setNumber) * n_way;

        int endIndex = startIndex + n_way - 1;

        int hitMade = 0;
        int isAnySpaceEmpty = 0;
        int nwayTemp = n_way;
        int loopIndex = startIndex;
        int i = 0;
        while (nwayTemp > 0)
        {
            i++;
            if (d_cache.valid_field[loopIndex] && d_cache.tag_field[loopIndex] == tag)
            {
                d_cache.hits += 1;
                hitMade = 1;
                break;
            }
            if (d_cache.valid_field[loopIndex] == 0)
            {
                isAnySpaceEmpty = 1;
            }

            loopIndex += 1;
            nwayTemp--;
        }

        if (hitMade == 0)
        {
            d_cache.misses += 1;
            if (isAnySpaceEmpty > 0)
            {
                nwayTemp = n_way;
                loopIndex = startIndex;
                while (nwayTemp > 0)
                {
                    if (d_cache.valid_field[loopIndex] == 0)
                    {
                        d_cache.valid_field[loopIndex] = 1;
                        d_cache.tag_field[loopIndex] = tag;
                        break;
                    }

                    loopIndex += 1;
                    nwayTemp--;
                }
            }
            else
            {

                int randomIndex = (rand() % (endIndex - startIndex + 1)) + startIndex;
                d_cache.valid_field[randomIndex] = 1;
                d_cache.tag_field[randomIndex] = tag;
            }
        }
    }
    printf("==================================\n");
    printf("Cache Hits:    %d\n", d_cache.hits);
    printf("Cache Misses:  %d\n", d_cache.misses);
    printf("Cache Hit Rate : %0.9f%%\n", ((float)d_cache.hits / (float)(d_cache.hits + d_cache.misses)) * 100);
    printf("Cache Miss Rate : %0.9f%%\n", ((float)d_cache.misses / (float)(d_cache.hits + d_cache.misses)) * 100);
    printf("\n");
    fclose(fp);
}

void startProcess(int cacheSize, int noOfBlocks, int n_way, int sizeofBlock)
{
    // printf("The below output is for cache with %dKB and %d bytes and %d way associative ", cacheSize, blockSize, nway);
    readFileAndDoCacheHitOrMiss(noOfBlocks, n_way, sizeofBlock);
}



void ImplementNwayAssociativeMappingforQuestion1()
{
    int continuepicking = 0;
    do
    {
        long cacheSize = 32 * 1024;
        int noOfBlocks = 0;
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
        switch (choice)
        {
        case 1:
            printf("\n\n====== Executing for 16 BYTES cache line size ======= \n");
            noOfBlocks = (int)cacheSize / 16;
            switch (selection)
            {
            case 8:
                startProcess(32, noOfBlocks, 8, 16);
                break;
            case 4:
                startProcess(32, noOfBlocks, 4, 16);
                break;
            case 2:
                startProcess(32, noOfBlocks, 2, 16);
                break;
            case 1:
                startProcess(32, noOfBlocks, noOfBlocks, 16);
                break;
            default:
                break;
            }

            break;
        case 2:
            printf("\n\n====== Executing for 32 BYTES cache line size ====== \n");
            noOfBlocks = (int)cacheSize / 32;
            switch (selection)
            {
            case 8:
                startProcess(32, noOfBlocks, 8, 32);
                break;
            case 4:
                startProcess(32, noOfBlocks, 4, 32);
                break;
            case 2:
                startProcess(32, noOfBlocks, 2, 32);
                break;
            case 1:
                startProcess(32, noOfBlocks, noOfBlocks, 32);
                break;
            default:
                break;
            }

            break;
        case 3:
            printf("\n\n====== Executing for 128 BYTES cache line size ====== \n");
            noOfBlocks = (int)cacheSize / 128;
            switch (selection)
            {
            case 8:
                startProcess(32, noOfBlocks, 8, 128);
                break;
            case 4:
                startProcess(32, noOfBlocks, 4, 128);
                break;
            case 2:
                startProcess(32, noOfBlocks, 2, 128);
                break;
            case 1:
                startProcess(32, noOfBlocks, noOfBlocks, 1628);
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
void ImplementNwayAssociativeMappingforQuestion2()
{

    int continuepicking = 0;
    do
    {
        long cacheSize = 16 * 1024;
        int noOfBlocks = 0;
        int choice = 0;
        printf("For Cache line size of 64Bytes\n");
        printf("Click 1 for 16KB Cachesize\n");
        printf("Click 2 for 32KB Cachesize\n");
        printf("Click 3 for 64KB Cachesize\n");
        scanf("%d", &choice);
        int selection;
        printf("Select 1 for fully associative\n");
        printf("Select 2 for 2 way execution\n");
        printf("Select 4 for 4 way execution\n");
        printf("Select 8 for 8 way execution\n");
        scanf("%d", &selection);

        switch (choice)
        {
        case 1:
            noOfBlocks = (int)cacheSize / 64;
            printf("\n\n====== Executing for  16kb cache size and 64 bytes cache line size/block size======\n");
            switch (selection)
            {
            case 8:
                startProcess(16, noOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(16, noOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(16, noOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(16, noOfBlocks, noOfBlocks, 64);
                break;

            default:
                break;
            }

            break;
        case 2:
            printf("\n\n====== Executing for 32kb cache size and 64 bytes cache line size/block size======\n");
            cacheSize = 32 * 1024;
            noOfBlocks = (int)cacheSize / 64;
            switch (selection)
            {
            case 8:
                startProcess(32, noOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(32, noOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(32, noOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(32, noOfBlocks, noOfBlocks, 64);
                break;

            default:
                break;
            }
            break;
        case 3:
            printf("\n\n====== Executing for 64kb cache size and 64 bytes cache line size/block size=======\n");
            cacheSize = 64 * 1024;
            noOfBlocks = (int)cacheSize / 64;
            switch (selection)
            {
            case 8:
                startProcess(64, noOfBlocks, 8, 64);
                break;
            case 4:
                startProcess(64, noOfBlocks, 4, 64);
                break;
            case 2:
                startProcess(64, noOfBlocks, 2, 64);
                break;
            case 1:
                startProcess(64, noOfBlocks, noOfBlocks, 64);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        printf("Select 1 to continue the test cases, any other to exit: ");
        scanf("%d", &continuepicking);
    } while (continuepicking == 1);
}

int main(int argc, char *argv[])
{
    trace_file_name = argv[2];
    int option = 0;
    printf("Select 1 to run Question 1\n");
    printf("Select 2 to run Question 2\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        ImplementNwayAssociativeMappingQuestion1();
        break;

    case 2:
        ImplementNwayAssociativeMappingQuestion2();
        break;
    }
}