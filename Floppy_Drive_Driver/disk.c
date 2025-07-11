#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"
/**
 * LAST BUILD:
9:51 PM 4/21/2025
 */
int s_lseek(int fd, int offset, int whence)
{
    if(lseek(fd, offset , whence) == -1)
    {
        printf("SEEK OPERATION  FAILED\n");
        return 0;
    }
    return 1;
}

int s_read(int fd, void* buffer, size_t nbyte)
{
    if(read(fd, buffer, nbyte) <= 0)
    {
        printf("READ OPERATION FAILED\n");       
        return 0;
    }
    return 1;
} 
 
Disk physical_disk(char* name)
{

    int fd = open(name, O_RDONLY);
    if(fd == -1)
    {
        printf("No file by the name %s found\n", name);
        return NULL;
    }

    Disk theDisk = (Disk)malloc(sizeof(disk_t));

    if(!theDisk) 
    {
        close(fd);
        printf("Memory allocation failed\n"); 
        return NULL;
    }

    theDisk->floppyDisk = fd;
    
    geometry_t* disk_geo = &(theDisk ->geometry);

    if(!s_lseek(theDisk ->floppyDisk,11,SEEK_SET) || !s_read(theDisk ->floppyDisk, &(disk_geo->bytesPerSector), 2))
    {
        free(theDisk);
        close(theDisk->floppyDisk);
        return NULL;
    }

    if (!s_lseek(theDisk ->floppyDisk,19,SEEK_SET) || !s_read(theDisk ->floppyDisk, &(disk_geo->maxLogicalSectors), 2))
    {

        free(theDisk);
        close(theDisk->floppyDisk);
        return NULL;
    }


    if (!s_lseek(theDisk ->floppyDisk,24,SEEK_SET) || !s_read(theDisk ->floppyDisk, &(disk_geo->sectorsPerTrack), 2))
    {

        free(theDisk);
        close(theDisk->floppyDisk);
        return NULL;
    }

    if (!s_lseek(theDisk ->floppyDisk,26,SEEK_SET) || !s_read(theDisk ->floppyDisk, &(disk_geo->heads), 2))
    {

        free(theDisk);
        close(theDisk->floppyDisk);
        return NULL;
    }


    disk_geo -> cylinders = disk_geo->maxLogicalSectors/((disk_geo->sectorsPerTrack) * disk_geo->heads); 

    return theDisk;
}

int sector_read(Disk theDisk, unsigned int logicalSectorNumber, unsigned char* buffer)
{
    if(!theDisk) 
    {
        printf("Attributes of disk unknown...\n"); 
        return 0;
    }
    if(logicalSectorNumber > theDisk->geometry.maxLogicalSectors - 1)
    {
        printf("EXCEEDING MAXIMUM LOGICAL SECTORS\n");
        return 0;
    }
    
    int buffersize = theDisk->geometry.bytesPerSector;
    if(!s_lseek(theDisk->floppyDisk, logicalSectorNumber * theDisk->geometry.bytesPerSector , SEEK_SET))
    {
        return 0;
    }

    if(!s_read(theDisk ->floppyDisk, buffer, buffersize))
    {   
        return 0;
    }

    
    return 1;
}

void sector_dump(Disk theDisk, int logicalSectorNumber, char type)
{
    if(!theDisk) 
    {
        printf("Attributes of disk unknown...\nPlease initialize disk before\n"); 
        return;
    }

    //make buffer size of 1 entire sector
    unsigned char* buffer = malloc(theDisk->geometry.bytesPerSector * sizeof(unsigned char));

    if(!buffer)
    {
        printf("MALLOC FAILED\n");
        return;
    }

    if(!sector_read(theDisk, logicalSectorNumber ,buffer))
    {
        return;
    }

    //formatting
    unsigned char c;
    for(int i = 0; i < (theDisk->geometry.bytesPerSector)/16; i++)
    {
        //byte offset
        printf("0x%04X ",((logicalSectorNumber*theDisk->geometry.bytesPerSector) + i * 16));
        
        for(int j = 0; j < 16; j++){
            c = buffer[(i * 16) + j];
            switch (type)
            {
            case 'c':
                if(c < 32 || c > 128)
                {
                    c = '?';
                }
                printf("%c ", c);
                break;
            case 'x':
                //printing directly from buffer or else formatting issues 
                printf("%02X ", c);
                break;
            case 'o':
                printf("%03o ", c);
                break;
            default:
                break;
            }
        }
        printf("\n");
    }

    free(buffer);
}