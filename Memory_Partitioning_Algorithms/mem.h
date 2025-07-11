/* minimum and maximum duration of use for an allocated block of memory */
#ifndef MIN_DURATION
  #define MIN_DURATION     13
#endif // MIN_DURATION

#ifndef MAX_DURATION
  #define MAX_DURATION     27      /* must "fit" in a dur_t type (see below) */
#endif // MAX_DURATION

/* minimum and maximum allocation request size */
#ifndef MIN_REQUEST_SIZE
  #define MIN_REQUEST_SIZE  7
#endif // MIN_REQUEST_SIZE

#ifndef MAX_REQUEST_SIZE
  #define MAX_REQUEST_SIZE 57
#endif // MAX_REQUEST_SIZE

typedef unsigned char dur_t;     /* duration type (eg. unsigned char, int) */
typedef enum mem_strats { BESTFIT, FIRSTFIT, NEXTFIT } mem_strats_t;

int mem_allocate(mem_strats_t strategy, int size, dur_t duration);

int mem_single_time_unit_transpired();

int mem_fragment_count(int frag_size);

void mem_clear();

void mem_init(int size);

void mem_free();

void mem_print();
