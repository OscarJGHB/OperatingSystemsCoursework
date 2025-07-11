
// Disk geometry type
typedef struct {
    unsigned int bytesPerSector;
    unsigned int sectorsPerTrack;
    unsigned int heads;
    unsigned int cylinders;
    unsigned int maxLogicalSectors;
} geometry_t;

    // Handle is a file descriptor returned by open()
typedef int handle_t;

    // Our floppy disk abstraction
typedef struct {
    handle_t floppyDisk;
    geometry_t geometry;
} disk_t;


    // Disk is a pointer to a disk_t
typedef disk_t *Disk;


/**
 * Initializes the MS-DOS device indicated by name and returns information about the device as a Disk. More information about the Disk can be found in the
section titled “Disk and Geometry Structures”.
 */
Disk physical_disk(char* name);

/** 
Reads the given logical sector from the Disk and enters the data, byte-by-byte, from that
sector into the given buffer. Note, buffer must be large enough to hold a sector’s worth of data. Allocate this buffer dynamically to match the size of a sector on the given disk. Returns 1 if succeeds,
0 otherwise.
**/
int sector_read(Disk theDisk, unsigned int logicalSectorNumber, unsigned char* buffer);

/**
 * first calls
sector read and then prints the contents of buffer to stdout. The type may be one of ‘c’, ‘x’, or
‘o’ to produce output as character, hexadecimal, or octal. Output is to be formatted 16 bytes to a
line, with the physical byte number of the first byte given in hex. For example, suppose the logical
sector starts with byte number 1234 (4d2 in hex), and the buffer contains 32 bytes consisting of the
numbers 1 through 32. 
 */
void sector_dump(Disk theDisk, int logicalSectorNumber, char type);