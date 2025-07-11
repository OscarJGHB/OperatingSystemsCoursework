#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "disk.h"

//if not a number then returns 1
int detect_non_num(char* char_ptr)
{
    
    while(*char_ptr != '\0')
    {
        if(*char_ptr < '0' || *char_ptr > '9')
        {
            return 1;
        } 
        char_ptr++;
    }
    return 0; //no char detected
}

int main(int argc , char** argv)
{
    if(argc < 2 || argc == 3 || argc > 4)
    {
        printf("USAGE: %s filename [sector# output_type]\n", *argv);
        return -1;
    }

    char* filename = argv[1];

    Disk theDisk = physical_disk(filename);

    if(!theDisk)
    {
        printf("No disk was found\n");
        return 1;
    }
    
    switch (argc)
    {
    case 2:
        printf("Physical Attributes of Floppy Disk:\n");
        printf("\t%-19s: %d\n","Sectors Per Track",theDisk->geometry.sectorsPerTrack);
        printf("\t%-19s: %d\n","Bytes Per Sector",theDisk->geometry.bytesPerSector);
        printf("\t%-19s: %d\n","Cylinders/Tracks", theDisk->geometry.cylinders);
        printf("\t%-19s: %d\n","Heads", theDisk->geometry.heads);
        printf("\t%-19s: %d\n","Valid Sector Range", theDisk->geometry.maxLogicalSectors);
        break;
    
    case 4://sector num + charformat
        char type = *(argv[3]);
        if(detect_non_num(argv[2]) || (type!= 'c' && type != 'x' && type != 'o' ) )
        {
            printf("USAGE: %s filename [sector# output_type]\n", *argv);
            printf("Optional arguments need to be in order: (int) sector# (c,x,o) output_type\n");
            return 1;
        }
        int sector_number = atoi(argv[2]);
        sector_dump(theDisk, sector_number, type);

        break;

    default:
        break;
    }

    close(theDisk->floppyDisk);
    free(theDisk);







    //needs 
    return 0;
}