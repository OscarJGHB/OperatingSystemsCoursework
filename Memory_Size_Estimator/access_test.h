typedef struct {
    double mean;
    double median;
    double smallest; //range
    double largest;
} TimeStats_d;

typedef struct {
    int cache_size; //done
    int cache_block;
    TimeStats_d cache_access_stats;
    TimeStats_d mem_access_stats;
} MemBenchmark;

//assuming cache already filled
//accesses blocks in reverse
void find_cache_size(MemBenchmark* mem_info, int max_mb, int samples, int sample_size);

//assuming cache already filled
//accesses in reverse
//returns size of block
void find_cache_block(MemBenchmark* mem_info,  int max_mb, int sample_size);

//gets access times to cache
//modifies elapsed time array
//traverses arr reverse until arr_size and fills up e_times with elapsed times
//elapsed times recorded per block
void test_cache_access_time(MemBenchmark* mem_info,  int max_mb, int samples, int sample_size);

//gets access times to main memory
//modifies elapsed time array
//traverses arr until arr_size and fills up e_times with elapsed times
void test_memory_access_time(MemBenchmark* mem_info,  int max_mb, int samples, int sample_size);