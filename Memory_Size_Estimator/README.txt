Q&A:

1. How big is a cache block?
    ANSWER:
        The cache block for me is 64B. My method was to choose the biggest jump from one stride size to the next.
2. How big is the cache?
    ANSWER:
        The size of the cache is at least 16MB for me as the jump to 32MB increases accesses more than twice(100x).
3. How long does a reference to memory take to complete?
    ANSWER:
        It takes memory, on average over 1000 samples with 1000 accesses, 2 microseconds to access data on my computer.
4. How long does a reference that can be satisfied from cache complete?
    ANSWER:
        It takes cache significantly less time to access data at 4ns over the same number of tests as memory.


Final Strat:
    - Figure out the cache size first, this way, it is easier for us to figure out memory size without using the 
        max MB that the user allots. 
        - Prime the cache
        - access sequentially
        - increase array size 
        - repeat until huge jump occurs- the last one will be our cache size
    - From the cache size(without cache size, it is likely that there isn't enough memory or measurements per sample), we double it and randomly access each index to get time access.
        - For each sample, malloc 2x cache block size
        - access randomly with rand_int per measurement , sample_size times
        - free the array for the next reallocation to be less likely in the cache
    - For the block size, allocate at least 1 MB to access over a number of times with different strides each
        - go through each stride until the end of the array
        - record it
        - repeat
        - at the end, find the array with the highest increase from the last, set that one as the cache block
    - For cache access time, do the same thing we did for memory but access everything in sequence to get fastest time
        - allocate cache block size
        - prime the array
        - access sequentially and adding one
        - record at the end


Initial Strat and Processes:
    - First, getting memory access times
    - create big enough array 
    - access each each array effectively getting mem access time AND caching last elems of array
    - Second, getting block size by going in reverse
    - going in reverse and measuring at various pts attempts to access most recently cached items
    - will stop when a big increase of latency appears giving us block size
    - Third, using block size, we find cache size(still going in reverse)
    - we will measure at each block size until we find a big enough latency jump
    - with that, it will go in as L1... 
    - for L2 and L3, we run it again wherever the previous level capped off until we match main memory access time
    - Finally, with all knowledge about cache sizes, we will time all block accesses at all levels of cache
    
    Strat Changes:

        Did not work:
        - Timing seemed to be the same 


        Strat changes:
        - going after MAIN MEMORY access first
        - using rand()_range as index to avoid CPU optimization/prefetching
        - as long as SIZE of array is big enough, there's only a 1/SIZE chance of a cache hit- that chance only gets smaller with each access 
        - test how long rand()_range takes
        - taking away time to perform rand() from time elapsed in memory accesses

        Did not work:
        - timings vary already for mem access - jumps between 200 nanoseconds to 400 nanoseconds on avg
        - 2 OUTPUTS, 1000000 accesses each:
            MEASURING access and rand() TIME
                avg =  0.454795 microseconds
                mode range = 0.130013 microseconds - 0.130013 microseconds 
                sample[0] = 0.118622 microseconds
                sample[1000] = 12.016487 microseconds
                sample[500] = 0.131265 microseconds
            MEASURING access and rand() TIME
                avg =  0.234538 microseconds
                mode range = 0.127979 microseconds - 0.127979 microseconds 
                sample[0] = 0.122759 microseconds
                sample[1000] = 2.562496 microseconds
                sample[500] = 0.132688 microseconds

        Strat Changes:
        - going to lock a CPU to the process(CPU Pinning)

        Did work:
        - locking CPU changed the measurements, making them more consistent(between 220ns to 280ns for 1m accesses, 130ns to 140ns for 10m to 100m accesses )
        - timings for rand() did not change beyond 1m to 100m calls with sample of 1000 to 10000 and ends up being 12 ns we 

        Did not work:
        - timings for mem access times are high when samples and measurements per sample are low, but become gradually lower as samples increase

        Strat Changes:
        - I believe what is happening is the cache is filling up with more accesses(with millions of access, some are bound for cache hits)
            so I will malloc an array before every sample

        Did work:
        - As malloc is called for every sample, the access times are more consistent
        - In my tests, 16MB to 24MB, access times are very low, 20ns or so 
        - When I changed allocated space to 32MB-256MB, it jumps up to 2 to 2.5 microseconds on avg so it leads me to believe that my cache size is between 16MB
            and 31 MB. However, instead guessing I will implement a way to get a more accurate measurement of cache size.
        - For the test, I was considering malloc per memory access to see if access time would change if we 
            accessed a new array every time-- I tested this out by making my measurements per sample 1 and number of samples
            ranging from 10 to 100000. The timings were consistent with the previously noted 2 microseconds compared to 
            1000 accesses per measurement for 1000 samples. Though, the lower number of samples lead to higher averages but the median
            stayed the same.
        - OUTPUT with 1 access per sample and 10 samples:
            We are performing 1 accesses per sample with a sample size of 10
            Memory Allocated: 33554432 bytes

            Pulling pre-sorted access times
                    sample[0] = 5.040000 microseconds
                    sample[4] = 2.435000 microseconds
                    sample[9] = 2.424000 microseconds

            MEASURING access without rand() TIME
                    avg =  2.746400 microseconds
                    mode range = 2.412000 microseconds - 2.523000 microseconds making up 70.00000% of the sample 
                    sample[0] = 2.412000 microseconds
                    sample[9] = 5.028000 microseconds
                    sample[4] = 2.523000 microseconds

        - OUTPUT with 1000 accesses per sample and 1000 samples:
            We are performing 1000 accesses per sample with a sample size of 1000
            Memory Allocated: 33554432 bytes

            Pulling pre-sorted access times
                    sample[0] = 2.499533 microseconds
                    sample[499] = 2.176119 microseconds
                    sample[999] = 1.949357 microseconds

            MEASURING access without rand() TIME
                    avg =  2.002486 microseconds
                    mode range = 1.911239 microseconds - 2.006646 microseconds making up 47.20000% of the sample 
                    sample[0] = 1.820058 microseconds
                    sample[999] = 3.157473 microseconds
                    sample[499] = 1.965208 microseconds

        - Strat Changes:
            - Will be trying to find cache blocks in doubling strides, when there appears to be a big slowdown, we assume the previous 
                result is the block size
            - Will set a number of tests in an array well within cache block(1MB) - mostly due to performance issues
        
        Did Work:
            - Observing the changes, block size is the one that has the highest rate of change which 

        - Strat Changes:
            - Using the memory time access strategy, we will randomly access items in the array for each sample.
            - However, the array size will double until we have found profound leaps.

        - Did Work:
            - By reversing the memory access strat, a huge jump was observed before and after the cache size- or what is at least the cache size



