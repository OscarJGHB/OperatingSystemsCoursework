#define _GNU_SOURCE
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <math.h>

#include <time.h>
#include "access_test.h"
#include "measure_tools.h"

#define DEBUG 0
double find_avg_rand_time(int max,int samples, int sample_size )
{
    int i,j;
    struct timespec ts_start, ts_end;
    double* sample = (double*)malloc(samples*sizeof(double));
    double elapsed;

    

    //main event
    for(i = 0; i < samples; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts_start);
        for(j =0; j < sample_size; j++)
        {
        rand_range(0, max);
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);

        elapsed = (diff_timespec(&ts_end, &ts_start) / (float)(sample_size)) * 1000000; //nanoseconds div by 1mil to produce microseconds
        // storing in an array that will later be sorted to compute the median
        sample[i] = elapsed;
    }
    double rand_time = avg(sample, samples);
    free(sample);

    return rand_time;

}

void find_cache_size(MemBenchmark *mem_info, int max_mb, int samples, int sample_size)
{
    if(mem_info -> cache_size != 0)
    {
        return;
    }

    double* sample = (double*) malloc(samples*sizeof(double));  //each sample
    double elapsed; 
  
    int i,j,k;
    struct timespec ts_start, ts_end;

    double base_measure; //expecting somewhat linear growth as array increases
    
    if(DEBUG)printf("---Latency Test Over Random Accesses to Memory---\n");
    for(i = 1; i < max_mb * 2 ; i *= 2 )
    {

        for(j =0; j < samples; j++)
        {

            int arr_size = (i*1024*1024)/sizeof(int); //up to max MB in power of 2
            int* arr = (int*) malloc(arr_size*sizeof(int));
            clock_gettime(CLOCK_MONOTONIC, &ts_start);
            for(k = 0; k<sample_size;k++)
                {
                arr[rand_range(0,arr_size)] += 1;
                }
            clock_gettime(CLOCK_MONOTONIC, &ts_end);
            free(arr);
            elapsed = diff_timespec(&ts_end, &ts_start) / (float)(sample_size) * 1000000; //nanoseconds div by 1mil to produce microseconds
            // storing in an array that will later be sorted to compute the median
            sample[j] = elapsed;

        }
        //What we will use to compare with -- assuming everything is linear until out of cache
        if(i == 1)
        {
            base_measure = avg(sample, sample_size);
        }
        
        //
        if(DEBUG)printf("\t%f avg for %d MB\n", avg(sample, sample_size), i);
        if(avg(sample, sample_size) > i * base_measure)
        {
            mem_info -> cache_size = i/2;
            break;
            
        }
    
    }
    if(DEBUG)
    {
        if(mem_info -> cache_size == 0)
        {
            printf("CACHE NOT DETECTED - possibly due to small sample size\n");
        }
            printf("---------------------end------------------\n");
    }

}

void test_memory_access_time(MemBenchmark* mem_info,  int max_mb, int samples, int sample_size)
{
    if(mem_info -> cache_size == 0)
    {
        find_cache_size(mem_info,  max_mb,  samples, sample_size);
        if(mem_info -> cache_size == 0)
        {
            return;
        }
        test_memory_access_time(mem_info, max_mb, samples, sample_size);
        return;
    }
    
    max_mb = (mem_info->cache_size)* 2; //trying to minimize space used 

    int i, j;
    struct timespec ts_start, ts_end;
    double* sample = (double*) malloc(samples*sizeof(double));
    double elapsed;
    int arr_size = (1024 * 1024 * max_mb) / sizeof(int);




    //main event
    for(i = 0; i < samples; i++)
    {

        int* arr = (int*) malloc(arr_size* sizeof(int));
        clock_gettime(CLOCK_MONOTONIC, &ts_start);
            for(j = 0; j<sample_size;j++)
            {
                arr[rand_range(0,arr_size)] += 1;
            }
        clock_gettime(CLOCK_MONOTONIC, &ts_end);
        free(arr);
        elapsed = diff_timespec(&ts_end, &ts_start) / (float)(sample_size) * 1000000; 
        sample[i] = elapsed;

    }


    qsort(sample, samples, sizeof(double), cmpdoublep);
    subtract_from(sample, find_avg_rand_time((max_mb * 1024 * 1024),samples, sample_size), samples); //need to include this in final

    TimeStats_d* memory = &(mem_info -> mem_access_stats);
    memory -> mean = avg(sample, samples);
    memory -> median = sample[(samples)/2];
    memory -> smallest = sample[0];
    memory -> largest = sample[samples-1];;

    free(sample);
}

void test_cache_access_time(MemBenchmark* mem_info,  int max_mb, int samples, int sample_size)
{
    if(mem_info -> cache_size == 0)
    {
        find_cache_size(mem_info,  max_mb,  samples, sample_size);
        if(mem_info -> cache_size == 0)
        {
            return; //could not find cache size, need more samples or memory
        }
        test_cache_access_time(mem_info, max_mb, samples, sample_size);
        return;
    }

    
    max_mb = mem_info -> cache_size;
    int i, j;
    struct timespec ts_start, ts_end;
    double* sample = (double*) malloc(samples*sizeof(double));
    double elapsed;
    int arr_size = (1024 * 1024 * max_mb) / sizeof(int);
    int* arr = (int*)malloc( (arr_size) * sizeof(int));

    //priming the cache
    for(i = 0; i < arr_size; i++)
    {
        arr[i] = 1;
    }

    //main event
    for(i = 0; i < samples; i++)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts_start);
            for(j = 0; j<sample_size;j++)
            {
                arr[j%arr_size] = 1;
            }
        clock_gettime(CLOCK_MONOTONIC, &ts_end);
        elapsed = diff_timespec(&ts_end, &ts_start) / (float)(sample_size) * 1000000; 
        sample[i] = elapsed;

    }

    qsort(sample, samples, sizeof(double), cmpdoublep);
    TimeStats_d* cache = &(mem_info->cache_access_stats);
    cache -> largest = sample[samples-1];
    cache -> smallest = sample[0];
    cache -> median = sample[samples/2];
    cache -> mean = avg(sample, samples); 
    free(arr);
    free(sample);

}

void find_cache_block(MemBenchmark* mem_info,  int max_mb, int samples, int sample_size)
{

    if(mem_info -> cache_size == 0)
    {
        find_cache_size(mem_info,  max_mb,  samples, sample_size);
        if(mem_info -> cache_size == 0)
        {
            return; //could not find cache size, need more samples or memory
        }
        find_cache_block(mem_info, max_mb, samples, sample_size);
        return;
    }
    
  
    int i, j, k , stride;
    struct timespec ts_start, ts_end;

    int arr_size =  (1024 * 1024)/sizeof(char); //within the cache size, smallest 
    int max_strides = arr_size/(1024);  //by index



  
    int stride_levels = (int)(log2((double)(max_strides)) + 1); //one array for each stride level, a stride means how many bytes will be iterated through at once
    
    double** e_times = (double**) malloc(stride_levels*sizeof(double*)); //2d array for each stride level

    //allocate for each subarray
    for(i = 0; i < stride_levels; i++)
    {
        e_times[i] = (double*) malloc(sample_size*(sizeof(double))); //for each index, there will be 
    }
    double elapsed;

    //arr to be iterated thru 
        //attempting to relocate so same memory not in
    char* arr = (char*) malloc(arr_size*sizeof(char)); 

    for(j = arr_size - 1; j >= 0; j-- )
    {
        arr[j] += '!';
    }

    //--------------------------------------------------MAIN EVENT--------------------------------------------------
    //for each level of striding
    for(i = 0; i < stride_levels; i++)
    {
        //for each sample
        stride = 1 << i;

        for(j = 0; j< sample_size; j++)
        {
            clock_gettime(CLOCK_MONOTONIC, &ts_start);

            //for each stride jump - accessing array
            for(k = 0; k < arr_size; k += stride)
            {
                arr[k] += 1;
            }
            clock_gettime(CLOCK_MONOTONIC, &ts_end);
            elapsed = (diff_timespec(&ts_end, &ts_start) / (float)(arr_size/stride)) * 1000000; //microseconds

            e_times[i][j] = elapsed;

        }
    }
    
    free(arr);
    double* rate_of_change = (double*) malloc((stride_levels-1) * sizeof(double));

    for(i = 0; i < stride_levels-1; i++)
    {
        rate_of_change[i] = (e_times[i+1][sample_size/2] - e_times[i][sample_size/2])/e_times[i+1][sample_size/2];
    }

    //find max
    double max = rate_of_change[0];
    int max_ind;
    for(i = 0; i < stride_levels-1; i++)
    {
        if(rate_of_change[i] > max)
        {
            max = rate_of_change[i];
            max_ind = i;
        }
    }

    mem_info -> cache_block = 1 << max_ind;
    
    for (i = 0; i < stride_levels; i++) {
        free(e_times[i]);
    }
    free(e_times);
    free(rate_of_change);
}