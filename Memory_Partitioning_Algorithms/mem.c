#include <stdio.h>    /* for printf statements when debugging */
#include <stdlib.h>   /* for malloc() and free() */
#include "mem.h"

/*
  Physical memory array. This is a static global array for all
  functions in this file.  An element in the array with a value of
  zero represents a free unit of memory.
*/
static dur_t* memory;

/*
 The size (i.e. number of units) of the physical memory array. This is
 a static global variable used by functions in this file.
 */
static int mem_size;


/*
 The last_placement_position variable contains the END position of the
 last allocated unit used by the next fit placement algorithm.
 */
static int last_placement_position;

/* returns size of contiguous block starting at position, pos */

//returns size of block which, when added, goes one over
int get_contiguous_blocksize(int pos)
{
  int start = pos;
  int val = memory[pos];
  
  
  do {
    pos++;
  } while (pos < mem_size && memory[pos] == val);

  return pos - start;
}

//fills duration from pos(INCLUSIVE) to pos+n(EXCLUSIVE)
//INCREMENTS POS
void fill_n_spots(int* pos, int n, dur_t duration)
{
  if(memory == NULL)
  {
    printf("No memory initialized");
    exit(-1);
  }
  if((*pos) + n > mem_size)
  {
    printf("Exceeds memory space at pos %d\n", (*pos) + n);
    exit(-1);
  }

  for(int i = 0; i<n ; i++)
  {
    memory[(*pos)++] = duration;
  }

  return;
  
}
/*
  Using the memory placement algorithm, strategy, allocate size
  units of memory that will reside in memory for duration time units.

  If successful, this function returns the number of contiguous blocks
  (a block is a contiguous "chuck" of units) of free memory probed while 
  searching for a suitable block of memory according to the placement 
  strategy specified.  If unsuccessful, return -1.

  If a suitable contiguous block of memory is found, the first size
  units of this block must be set to the value, duration.
 */
int mem_allocate(mem_strats_t strategy, int size, dur_t duration)
{
  if(memory == NULL)
  {
    printf("No memory initialized");
    exit(-1);
  }


  int pos = 0; //position
  int block_size = 0; //used to measure block size
  int probe_count = 0; //to return if no fail


  //BEST FIT ALGORITHM - question, do i stop when I find exact size?
  if(strategy == BESTFIT)
  {
    pos = 0;
    int best_pos = -1;
    int best_size = mem_size+1;

    while(pos < mem_size)
    {
      block_size = get_contiguous_blocksize(pos);

      if(memory[pos] == 0)
      {
        probe_count+=1; //probin an empty spot
        //exact size
        if(block_size == size)
        {
          fill_n_spots(&pos, size, duration);
          return probe_count; //return count after filling a spot
        }

        //checks for best size
        else if(block_size > size)
        {
          if(block_size < best_size)
          {
            best_pos = pos;
            best_size = block_size;
          }
        }
      }

      pos += block_size;

    }

    if(best_pos != -1)
    {
      fill_n_spots(&best_pos, size, duration);
      return probe_count;
    }

    return -1;
  }

  //FIRST FIT ALGORITHM
  else if(strategy == FIRSTFIT)
  {

    while(pos < mem_size) 
    {

      
      block_size = get_contiguous_blocksize(pos);
      if(memory[pos] == 0)
      {
        probe_count+=1; 

        if(block_size >= size)
        {
          fill_n_spots(&pos, size, duration);
          return probe_count; //return count after filling a spot
        }

      }
      pos+=block_size;
    }
  }

  //NEXTFIT SOLUTION
  else if(strategy == NEXTFIT)
  {
    //"beginning"
    pos = last_placement_position;

    int end = mem_size; //count downwards

    while(end > 0)
    {
      pos %= mem_size;  //prevent overflow
      block_size = get_contiguous_blocksize(pos);
      if(memory[pos] == 0)
      {
        probe_count+=1;

        if(block_size >= size)
        {
          fill_n_spots(&pos, size, duration);
          last_placement_position = pos; 
          return probe_count;
        }
      }

      pos+=block_size; //no match
      end-=block_size;
    }
  }

  else
  {
    printf("strategy does not exist\n");
    return -2;
  }

  return -1; //failure :(

}

/*
  Go through all of memory and decrement all positive-valued entries.
  This simulates one unit of time having transpired.  NOTE: when a
  memory cell is decremented to zero, it becomes "unallocated".
 */
int mem_single_time_unit_transpired()
{

  if(memory == NULL)
  {
    printf("No memory initialized");
    exit(-1);
  }
  
  for(int pos = 0;pos < mem_size;pos++)
  {
    if( memory[pos] != 0)
    {
      memory[pos]--;
    }
  }

  return 0;
  
}

/*
  Return the number of fragments in memory.  A fragment is a
  contiguous free block of memory of size less than or equal to
  frag_size.
 */
int mem_fragment_count(int frag_size)
{
  if(memory == NULL)
  {
    printf("No memory initialized");
    exit(-1);
  }

  int frag_count = 0;
  int size_o_block = 0;
  int pos = 0;
  while(pos < mem_size)
  {
    size_o_block = get_contiguous_blocksize(pos);

    if(memory[pos] == 0 && size_o_block < frag_size)
    {
      frag_count++;
    }

    pos += size_o_block;

  }

  

  return frag_count;

}

/*
  Set the value of zero to all entries of memory.
 */
void mem_clear()
{
  if(memory == NULL)
  {
    printf("No memory initialized");
    exit(-1);
  }

  for(int pos = 0; pos < mem_size; pos++)
  {
    memory[pos] = 0;
  }

}

/*
 Allocate physical memory to size. This function should 
 only be called once near the beginning of your main function.
 */
void mem_init(int size)
{
  memory = malloc(sizeof(dur_t)*size);
  mem_size = size;
}

/*
 Deallocate physical memory. This function should 
 only be called once near the end of your main function.
 */
void mem_free()
{
  free(memory);
}

/*
  Print memory for testing/debugging purposes.  You will need to test
  and debug your allocation algorithms.  Calling this routine in your
  main() after every allocation (successful or not) will help in this
  endeavor.  NOTE: you should print the memory contents in contiguous
  blocks, rather than single units; otherwise, the output will be very
  long.
 */
void mem_print()
{
  int i = 0;
  int blocksize;

  printf("  start size dur\n");
  while (i < mem_size) {
    blocksize = get_contiguous_blocksize(i);
    printf("%5d %5d %3d\n", i, blocksize, memory[i]);
    i += blocksize;
  }
  printf("\n");
}
