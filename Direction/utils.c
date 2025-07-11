// -----------------------------------
// CSCI 340 - Operating Systems I 
// Spring 2025 (Anthony Leclerc)
// utils.c implementation file
// Assignment (Homework) 1
//
// -----------------------------------

// these are the ONLY library header files that can be used. Under no
// circumstances can additional library header files be included.  In
// particular, this restriction means you CANNOT USE any function
// declared in <string.h>, which includes functions such as: strtok(),
// strchr(), strstr(), strlen(), index(), and others.

// NOTE: when you compile, DO NOT IGNORE WARNINGS!  These warnings are
// likely indicating you are doing something wrong, such as using a
// library function that you are not allowed to use or passing the
// wrong type of argument into a function.

#include <stdio.h>	// standard I/O functions
#include <stdlib.h> 	// atof()
#include <math.h>	// math functions

#include "utils.h"	// user defined functions

// see utils.h for the required function prototypes and specifications

// NOTE: you may add any "helper" functions you need to get the job done.
//       for example, if you need to determine the length of a string.

// put your IMPLEMENTATION of the function prototypes below:

//filename is pointer to first character of string

void next_char(char** line){ //increments to next char after a comma/space
    while(**line != ',' && **line != '\0'){
            (*line)++;
        }
    (*line)++;
}

int read_lines(char* filename, line_t* line_arr, int n){

    FILE *fp; //file
    char line[255]; //makes space for 255 characters
    fp = fopen(filename, "r"); //opens txt files
    char *p; //ptr starts at first chr of line
    int counter = 0; //num lines read

    if(fp == NULL)
    {
        printf("FILE NOT FOUND");
        return 0;
    }
    while(fgets(line, 255, fp)!= NULL && counter < n ){
        p = line;
        if(*p != '\n'){
            line_t set;

            set.p0.x = atof(p);
        
            next_char(&p);
            set.p0.y = atof(p);
        
            next_char(&p);
            set.p1.x = atof(p);
        
            next_char(&p);
            set.p1.y = atof(p);

            line_arr[counter] = set;
            counter++;
        }
    }
    fclose(fp);
    return counter; 
}
;

double calc_direction(line_t* line_ptr)
{
    point_t p0 = line_ptr->p0;
    point_t p1 = line_ptr->p1;
    return atan2(p1.y-p0.y,p1.x-p0.x);
};

double calc_magnitude(line_t* line_ptr)
{
    point_t p0 = line_ptr->p0;
    point_t p1 = line_ptr->p1;
    double part1 = pow((p1.x-p0.x),2);
    double part2 = pow((p1.y-p0.y),2);
    return sqrt(part1+part2);
};

