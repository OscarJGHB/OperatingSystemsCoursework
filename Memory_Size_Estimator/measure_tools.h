int rand_range(int min, int max);

//assumes sorted

void subtract_from(double* arr, double subtract_with, int size);

//returns start of mode
int get_mode(double* arr, const double range, int size, int* counted);

// used by qsort() for comparing doubles (will sort in ascending order)
int cmpdoublep(const void* p1, const void* p2);

// compute the difference between two timespec structs
// returns the number of secs transpired as a double precision number
// returns seconds
double diff_timespec(const struct timespec *time1, const struct timespec *time0);


double avg(double* list, int size);
