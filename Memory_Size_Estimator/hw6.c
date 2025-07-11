#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include "access_test.h"
#include <time.h>


int main(int argc, char** argv)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); // Lock to CPU core 0
    //pass in what is max size user is ok with allocated in MB
    if(argc != 4)
    {
        printf("USAGE: %s max_mb_allowed samples sample_size\n",argv[0]);
        exit(1);
    }


    int max_mb_allowed = atoi(argv[1]);
    int samples = atoi(argv[2]);
    int sample_size = atoi(argv[3]);
    printf("Memory Allocated: %d bytes\n", (max_mb_allowed*1024*1024));
    printf("For each measurement, %d samples with %d records per sample\n", samples, sample_size);
    

     
    MemBenchmark* mem_info = (MemBenchmark*)malloc(sizeof(MemBenchmark));
    find_cache_size(mem_info, max_mb_allowed, samples, sample_size);
    find_cache_block(mem_info, max_mb_allowed, sample_size);
    test_memory_access_time(mem_info, max_mb_allowed, samples, sample_size);
    test_cache_access_time(mem_info, max_mb_allowed, samples, sample_size);

    // printf("%f\n",mem_info -> mem_access_stats.mean);
    // printf("%f\n",mem_info -> mem_access_stats.median);
    // printf("%f\n",mem_info -> cache_access_stats.mean);
    // printf("%f\n",mem_info -> cache_access_stats.median);

    TimeStats_d* cache_stats = &(mem_info -> cache_access_stats);
    TimeStats_d* memory_stats = &(mem_info -> mem_access_stats);

    printf("RESULTS:\n\n");
    printf("\tCACHE SIZE:\n");
    printf("\t\t%dMB\n", mem_info->cache_size );

    printf("\tCACHE BLOCK:\n");
    printf("\t\t%dB\n", mem_info->cache_block );

    printf("\tCACHE TIME STATS:\n");
    printf("\t\t%-10s: %6.6f microseconds\n","MEAN", cache_stats->mean);
    printf("\t\t%-10s: %6.6f microseconds\n","MEDIAN", cache_stats->median);
    printf("\t\t%-10s: %6.6f microseconds\n","SMALLEST", cache_stats->smallest);
    printf("\t\t%-10s: %6.6f microseconds\n","LARGEST", cache_stats->largest);

    printf("\tMEMORY TIMES STATS:\n");
    printf("\t\t%-10s: %6.6f microseconds\n","MEAN", memory_stats->mean);
    printf("\t\t%-10s: %6.6f microseconds\n","MEDIAN", memory_stats->median);
    printf("\t\t%-10s: %6.6f microseconds\n","SMALLEST", memory_stats->smallest);
    printf("\t\t%-10s: %6.6f microseconds\n","LARGEST", memory_stats->largest);
    
    if(mem_info->cache_size == 0.0)printf("No stats retrieved-- not enough memory allocated/too few tests\n");
    return 0;

    free(mem_info);
}