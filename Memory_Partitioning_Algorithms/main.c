#include <stdio.h>    /* for printf */
#include <stdlib.h>   /* for string to integer conversion, random numbers */
#include "mem.h"

#define DEBUG_TIME 0
#define DEBUG_RUN 0

/*
  The main program will accept four paramemters on the command line.
  SIZE OF PHYSICAL MEMORY
  NUMBER OF RUNS
  TIME STEPS PER RUN
  RANDOM SEED

  ./hw7 1000 100 3000 1235

  This means that your program should initialize physical memory to
  1,000 units, perform 100 runs with each run taking 3000 units of
  time, and the random number generator should be seeded (one time)
  with the value 1235.s
*/
int rand_range(long min, long max)
{
  return min + rand() % (max-min+1);
}

char* strat_used(mem_strats_t strat)
{
  switch (strat)
  {
  case 0:
    return "BESTFIT\0";
    break;

  case 1:
    return "FIRSTFIT\0";
    break;

  case 2:
    return "NEXTFIT\0";
    break;
  
  default:
    return "DOES NOT EXIST\0";
    break;
  }
}

int main(int argc, char** argv)
{  
  
  if(argc != 5)
  {
    printf("Must include 4 arguments: {SIZE OF PHYSICAL MEMORY} {NUMBER OF RUNS} {TIME STEPS PER RUN} {RANDOM SEED}\n");
    return -1;
  }

  mem_init(atoi(argv[1]));
  int num_runs = atoi(argv[2]); // for each algorithm
  int time = atoi(argv[3]); //for each run of each algorithm
  srand(atoi(argv[4]));

  //given by rand_range
  int process_size;
  int process_duration;

  //TODO 3 for-lSoops 
    //first: determine which algortihm {0,1,2}
        //each strat has avg/times ran ttl
      //second: do num runs iterations
          //when a run is over, do take counted variables and div by time units
        //third: do time_unit iterations for each num_runss 
            //count here.

  //needed:
    //avg probes, failures, fragments
    //no need to store, we can just print out at end of each strat

  

  //loop varibles
  mem_strats_t strat = BESTFIT;
  mem_strats_t last_strat = NEXTFIT;

  double avg_probes;
  double avg_frags;
  double avg_fails;

  int probes;
  int frags;
  int fails;

  unsigned int size;
  unsigned int duration;
  int success;

  printf("%-10s |%-10s |%-10s |%-10s\n", "STRAT USED", "AVG PROBES", "AVG FRAGS", "AVG FAILS");

  for(; strat<=last_strat; strat++)
  {


    avg_fails = 0;
    avg_frags = 0;
    avg_probes = 0;

    for(int i = 0; i< num_runs; i++)
    {
      if(DEBUG_RUN || DEBUG_TIME){printf("\tRUN NUM: %d\n", i+1);}
      probes = 0;
      frags = 0;
      fails = 0;

      for(int j = 0; j<time;j++)
      {
        if(DEBUG_TIME){printf("\t\tTIME ELAPSED %dMS\n", j);}

        size = rand_range(MIN_REQUEST_SIZE, MAX_REQUEST_SIZE);
        duration = rand_range(MIN_DURATION, MAX_DURATION);
        success = mem_allocate(strat, size, duration); //if allovation succeeded
        if(DEBUG_TIME){printf("\t\tSize: %d, Dur: %d\n", size,duration);}

        if(success == -1) //fail
        {
          if(DEBUG_TIME){printf("\t\talloc fail\n");}
          fails += 1;
        }
        else //succeeded
        {
          if(DEBUG_TIME){printf("\t\talloc succeeded\n");}
          probes += success;
        }

        if(DEBUG_TIME){mem_print();}

        frags += mem_fragment_count(MIN_REQUEST_SIZE);
        mem_single_time_unit_transpired();
      }

      if(DEBUG_RUN){printf("\t[run TOTALS] probes: %d frags: %d fails: %d \n",probes, frags, fails);}
      
      if(DEBUG_RUN){printf("\t[run AVG] probes: %.3f/tm frags: %.3f/tm fails: %.3f/tm\n",(double)probes/time,(double)frags/time, (double)avg_fails/time);}
      
      //summing each average before dividing outside
      avg_fails += (double)fails/time;
      avg_probes += (double)probes/time;
      avg_frags += (double)frags/time;

      if(DEBUG_RUN){printf("\t[SUM AVG] probes: %.3f frags: %.3f fails: %.3f\n",avg_probes,avg_frags, avg_fails);}
    }


    avg_fails /= num_runs;
    avg_probes /= num_runs;
    avg_frags /= num_runs;
    //divided by runs

    printf("%-10s |%10.3f |%10.3f |%10.3f|\n",strat_used(strat), avg_probes, avg_frags, avg_fails);
    //mem_clear();
  

  }
  

  
  mem_free();
  return 0;
}

