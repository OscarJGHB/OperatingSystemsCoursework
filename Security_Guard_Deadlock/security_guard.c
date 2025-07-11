/*
Original work Copyright (c) 2017 Anthony Leclerc <leclerca@cofc.edu>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
*/

// to compile enter:
//    cc -Wall security_guard.c binary_semaphore.c -lpthread
// NOTE: you will get a warning about unused variable, sthreads, but
// this will go away, as you complete the code.

#include <stdio.h>
#include <stdlib.h>  // for exit(), rand(), strtol()
#include <pthread.h>
#include <time.h>    // for nanosleep()
#include <errno.h>   // for EINTR error check in millisleep()

#include "binary_semaphore.h"

// you can adjust next two values to speedup/slowdown the simulation
#define MIN_SLEEP      20   // minimum sleep time in milliseconds
#define MAX_SLEEP     100   // maximum sleep time in milliseconds

#define START_SEED     11   // arbitrary value to seed random number generator

// guard_state with a value of k:
//          k < 0 : means guard is waiting in the room 
//          k = 0 : means guard is in the hall of department
//          k > 0 : means guard is IN the room (only if condition met)
int guard_state;         // waiting, in the hall, or in the room
int num_students;        // number of students in the room

// TODO:  list here the "handful" of semaphores you will need to synchronize
//        I've listed one you will need for sure, to "get you going"

//will be used for num students and guard state
binary_semaphore mutex;  // to protect shared variables (including semaphores)
binary_semaphore guard_waiting;
binary_semaphore room_clearing;

//semaphore to allow other students in


// will malloc space for seeds[] in the main
unsigned int *seeds;     // rand seeds for guard and students generating delays

// NOTE:  globals below are initialized by command line args and never changed !
int capacity;       // maximum number of students in a room
int num_checks;     // number of checks the guard makes

void millisleep(long millisecs)   // delay for "millisecs" milliseconds
{ // details of this function are unimportant for the assignment
  struct timespec req;
  req.tv_sec  = millisecs / 1000;
  millisecs -= req.tv_sec * 1000;
  req.tv_nsec = millisecs * 1000000;
  while(nanosleep(&req, &req) == -1 && errno == EINTR)
    ;
}

// generate random int in range [min, max]
int rand_range(unsigned int *seedptr, long min, long max)
{ // details of this function are unimportant for the assignment
  // using reentrante version of rand() function (because multithreaded)
  // NOTE: however, overall behavior of code will still be non-deterministic
  return min + rand_r(seedptr) % (max - min + 1);
}

void study(long id)  // student studies for some random time
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  printf("student %2ld studying in room with %2d students for %3d millisecs\n",
	 id, num_students, ms);
  millisleep(ms);
}


void do_something_else(long id)    // student does something else
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  millisleep(ms);
}

void assess_security()  // guard assess room security
{ // details of this function are unimportant for the assignment
  // NOTE:  we have (own) the mutex when we first enter this routine
  guard_state = 1;     // positive means in the room
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard assessing room security for %3d millisecs...\n", ms);
  millisleep(ms);
  printf("\tguard done assessing room security\n");
}

void guard_walk_hallway()  // guard walks the hallway
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard walking the hallway for %3d millisecs...\n", ms);
  millisleep(ms);
}

// this function contains the main synchronization logic for the guard
void guard_check_room()
{
  semWaitB(&mutex);
  //printf("\tthere are %d students...\n",num_students);
  
  if(num_students == 0)
  {
    assess_security();
    semSignalB(&mutex);

  }

//if students in room, then guard will wait
  else if(num_students < capacity)
  {
    //printf("\tstudents are still in room, guard waiting\n");
    guard_state = -1;
    printf("\tguard waiting to enter room with %d students...\n", num_students);
    semWaitB(&guard_waiting);//guard gonna be waitings
    semSignalB(&mutex);  //let student know guard waiting

    //put guard to sleep until last student signals it
    semWaitB(&guard_waiting); //forces self to waity
    printf("\tguard done waiting to enter room with %d students\n",num_students);
  
    semWaitB(&mutex);
    if(num_students == 0)
    {
      assess_security();
    }

    
  }
  if(num_students >= capacity)
  {
    
    guard_state = 1; //stop future students coming in
    printf("\tguard clearing room with %d students\n", num_students);
    printf("\tguard waiting for students to clear out with %d students\n", num_students);
    semWaitB(&room_clearing);
    semSignalB(&mutex); 
    semWaitB(&room_clearing);
    printf("\tdone clearing out room\n");
  }

  semSignalB(&guard_waiting); //important for semaphore balance
  semSignalB(&room_clearing);
  semSignalB(&mutex); //signals all previous mutex


  semWaitB(&mutex);
  guard_state = 0;
  printf("\tguard left room\n");
  semSignalB(&mutex);
}

// this function contains the main synchronization logic for a student
void student_study_in_room(long id)
{
  int in_room = 0;

  semWaitB(&mutex);

  if(guard_state == 0)
  {
    num_students++;
    in_room = 1;
    
  }

  else if(guard_state < 0)
  {
    num_students++;
    in_room = 1;
    if(num_students == capacity)
    {
      printf("LAST STUDENT %d entering room with guard waiting\n",(int)id);
      
      semSignalB(&guard_waiting);
      //here is where we signal ok for guard to come in 
    }
    
    //when all students leave, we set flag of wait outside to 0
  }
  
  //leaving room if was in room
  if(in_room)
  {
    semSignalB(&mutex);
    study(id);
    
    semWaitB(&mutex);
    num_students--;
    if(guard_state == 1 && num_students == 0)
    {
      printf("LAST STUDENT %d left room with guard in it\n", (int)id);
      semSignalB(&room_clearing); //room is cleared, guard can leave now
    }
    else if(guard_state < 0 && num_students == 0)
    {
      printf("LAST STUDENT %d left room with guard waitin\n", (int)id);
      semSignalB(&guard_waiting);
    }
    else if(num_students != 0)
    {
      printf("student %d left room\n",(int)id);
    }
    semSignalB(&mutex);
    
  }

  else
  {
    semSignalB(&mutex);
  }
}

// guard thread function  --- NO need to change this function !
void* guard(void* arg)
{
  int i;            // loop control variable
  srand(seeds[0]);  // seed the guard thread random number generator

  // the guard repeatedly checks the room (limited to num_checks) and
  // walks the hallway
  for (i = 0; i < num_checks; i++) {
    guard_check_room();
    guard_walk_hallway();
    //printf("\t\t\tthat was CHECK %d\n",i);
  }


  return NULL;   // thread needs to return a void*
}

// student thread function --- NO need to change this function !
void* student(void* arg)
{
  long id = (long) arg;  // determine thread id from arg
  srand(seeds[id]);      // seed this threads random number generator

  // repeatedly study and do something else
  while (1) {
    student_study_in_room(id);
    do_something_else(id);
  }

  return NULL;   // thread needs to return a void*
}

int main(int argc, char** argv)  // the main function
{
  int n;                   // number of student threads
  pthread_t  cthread;      // guard thread
  pthread_t* sthreads;     // student threads
  long i;                  // loop control variable

  if (argc < 4) {
    fprintf(stderr, "USAGE: %s num_threads capacity num_checks\n", argv[0]);
    return 0;
  }

  // TODO: get three input parameters, convert, and properly store

  // num threads 
  n = atoi(argv[1]);
  //capacity
  capacity = atoi(argv[2]);
  //checks
  num_checks = atoi(argv[3]);

  if(n < capacity)
  {
    printf("num_threads should be >= capacity\n");
    exit(-1);
  }
  
  // TODO: allocate space for the seeds[] array
  // NOTE: seeds[0] is guard seed, seeds[k] is the seed for student k

  seeds = malloc(sizeof(unsigned int) * (n+1));

  // TODO: allocate space for the student threads array, sthreads
  sthreads = malloc(sizeof(pthread_t) * n);


  // Initialize global variables and semaphores
  guard_state = 0;   // not in room (walking the hall)
  num_students = 0;  // number of students in the room

  semInitB(&mutex, 1);  // initialize mutex
  semInitB(&room_clearing,1);
  semInitB(&guard_waiting,1);
  // TODO: complete the semaphore initializations, for all your semaphores


  // initialize guard seed and create the guard thread
  seeds[0] = START_SEED;
  pthread_create(&cthread, NULL, guard, (void*) NULL);
  
  for (i = 1; i <= n; i++) {
    // TODO: create the student threads and initialize seeds[k], for
    // each student k
    seeds[i] = (int)i;
    pthread_create(&sthreads[i-1], NULL, student, (void*) i);

  }

  pthread_join(cthread, NULL);   // wait for guard thread to complete
  

  for (i = 0; i < n; i++) {

    pthread_cancel(sthreads[i]);
    // TODO: cancel each of the student threads (do man on pthread_cancel())
    //pthread_cancel();

  }

  free(seeds);
  free(sthreads);

  // TODO: free up any dynamic memory you allocated
  
  return 0;
}
