#include <time.h>
#include <stdlib.h>

//inc - exc
int rand_range(int min, int max)
{
  return min + rand() % (max - min - 1);
}

//assumes sorted

void subtract_from(double* arr, double subtract_with, int size)
{
  for(int i = 0; i<size; i++)
  {
    arr[i] -= subtract_with;
  }
}

//returns start of mode
int get_mode(double* arr, const double range, int size, int* counted)
{
  int target = 0; //INDEX
  int most_counted = target;
  int counter = 1;
  int most_counted_counter = 1;
  
  for(int i = 1; i<size; i++)
  {
    
    if(arr[i] <= arr[target] * range)
    {
      counter++;
      if(counter > most_counted_counter)
      {
        most_counted = target;
        most_counted_counter = counter; //sets new most counted num
      }

    }
    else
    {
      counter = 1;
      target = i;
    }
  }

  *counted = most_counted_counter;
  return most_counted;
}

// used by qsort() for comparing doubles (will sort in ascending order)
int cmpdoublep(const void* p1, const void* p2)
{
  return * (double*) p1 > * (double*) p2;
}

// compute the difference between two timespec structs
// returns the number of secs transpired as a double precision number
// returns seconds
double diff_timespec(const struct timespec *time1, const struct timespec *time0) {
  //difference in seconds + difference in nanoseconds/100 mil
  return (time1->tv_sec - time0->tv_sec) + (time1->tv_nsec - time0->tv_nsec) / 1000000000.0; 
}


double avg(double* list, int size)
{
  double total = 0.0;
  for( int i = 0; i < size; i++)
  {
    total += list[i];
  }
  return total /= size;
}
