#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <regex.h>

#define BUFFER_SIZE 8
#define BUFFER_SIZE_BIN 64
#define SWAP_PTE_FLAG 0x0800000000000000
#define PAGE_SIZE 4096
#define MAX_PARAMETERS 3

#define PRESENT_PTE_FLAG 0x0000000000000001
#define FILE_ANON_PTE_FLAG 0x0000000000000800
#define EXCLUSIVE_PTE_FLAG 0x0000000000000400
#define SOFTDIRTY_PTE_FLAG 0x0000000040000000
typedef unsigned long long address_t;


int mapall(address_t pid) {
    char maps_path[100];
    char pagemap_path[100];
    FILE* maps_file;
    FILE* pagemap_file;
    char region_name[256];
    char line[256];


    sprintf(maps_path, "/proc/%llx/maps", pid);
    maps_file = fopen(maps_path, "r");
    if (maps_file == NULL) {
        perror("Error open maps file");
        return -1;
    }


    while (fgets(line, sizeof(line), maps_file)) {
        unsigned long start_address, end_address;


        sscanf(line, "%lx-%lx %*s %*s %*s %*s %s", &start_address, &end_address, region_name);

        //printf("Region Name: %s\n", region_name);


        unsigned long long start_page_number = start_address / PAGE_SIZE;
        unsigned long long end_page_number = end_address / PAGE_SIZE;


        sprintf(pagemap_path, "/proc/%llx/pagemap", pid);
        pagemap_file = fopen(pagemap_path, "rb");
        if (pagemap_file == NULL) {
            perror("Error open pagemap file");
            return -1;
        }


        for (unsigned long long page_number = start_page_number; page_number <= end_page_number; ++page_number) {
            off_t offset = page_number * 8;


            if (fseeko(pagemap_file, offset, SEEK_SET) == -1) {
                perror("Error");
                return -1;
            }

            uint64_t value;

            if (fread(&value, sizeof(value), 1, pagemap_file) != 1) {
                perror("Error");
                return -1;
            }


            uint64_t frame_number = value & ((1ULL << 54) - 1);


            if (frame_number != 0) {
                printf("Page number: 0x%llx, Frame number: 0x%lx\n", page_number, frame_number);
            } else {
                printf("Page number: 0x%llx, not-in-memory\n", page_number);
            }
        }


        fclose(pagemap_file);
    }


    fclose(maps_file);
    
    return 0;
}

void buffer_to_binary(char* b, int size, char *binary){
    int i ;
    int j;
    char c;
    int bit;
    for (i = 0; i < size; i++){
        c = b[i];
        for (j = 0; j < 8; j++){
            bit = (c >> (7 - j)) & 1;
            binary[i * 8 + j] = bit + '0';
        }
    }
}


void char_to_binary(char c, char *binaryString) {
    int i;
    // Iterate through each bit of the character
    for (i = 7; i >= 0; i--) {
        // Use bitwise shift and bitwise AND operations to extract the bits
        int bit = (c >> i) & 1;
        // Convert the bit to a character and store it in the string
        binaryString[7 - i] = bit + '0';
    }
}

long hex_string_to_long(char *s){
    int size = 0;
    int i;
    int curr_int;
    long res = 0;

    i = 0;
    while (s[i] != '\0'){
        size++;
        i++;
    }

    long coef = 1;
    i = size - 1;
    while (i >= 0){

        if (s[i] == 'a'){
            curr_int = 10;
        } else if(s[i] == 'b'){
            curr_int = 11;
        } else if(s[i] == 'c'){
            curr_int = 12;
        } else if(s[i] == 'd'){
            curr_int = 13;
        } else if(s[i] == 'e'){
            curr_int = 14;
        } else if(s[i] == 'f'){
            curr_int = 15;
        } else {
            curr_int = s[i];
            curr_int -= 48;
        }

        res += curr_int * coef;
        coef = coef * 16;
        i--;
    }

    return res;
}

long bin_string_to_long(char *s){
    long coef = 1;
    int i;
    int size;
    long res = 0;
    int curr_bit;

    size = 0;
    i = 0;
    while (s[i] != '\0'){
        size++;
        i++;
    }

    i = size - 1;
    while (i >= 0){
        curr_bit = s[i];
        curr_bit -= 48;

        res += curr_bit * coef;
        coef = coef * 2;
        i--;
    }

    return res;
}

int frameinfo(unsigned long pfn){
    int fd;
    int fd_count;
    ssize_t bytes_read;
    ssize_t bytes_read_count;

    unsigned long entry;
    unsigned long entry_count;

    int i;

    fd = open("/proc/kpageflags", O_RDONLY);
    if (fd == -1) {
        printf("Failed to open /proc/kpageflags\n");
        return -1;
    }

    // Open the /proc/kpagecount file for reading
    fd_count = open("/proc/kpagecount", O_RDONLY);
    if (fd_count == -1) {
        printf("Failed to open /proc/kpagecount\n");
        return -1;
    }

    unsigned long offset =  pfn * sizeof(unsigned long); // start from the desired frame

    if (lseek(fd, offset, SEEK_SET) == -1){
        printf("lseek error\n");
        return -3;
    }
    if (lseek(fd_count, offset, SEEK_SET) == -1){
        printf("lseek error\n");
        return -3;
    }

    bytes_read = read(fd, &entry, sizeof(unsigned long));
    if (bytes_read == -1) {
        printf("Failed to read the entry\n");
        close(fd);
        return -1;
    }
    if (bytes_read == 0){
        printf("error1\n");
        return -2;
    }

    // Read and process the entry
    bytes_read_count = read(fd_count, &entry_count, sizeof(unsigned long));
    if (bytes_read_count == -1) {
        printf("Failed to read the entry\n");
        close(fd);
        return -1;
    }
    if (bytes_read_count == 0){
        printf("error2\n");
        return -2;
    }

    printf("count: %lu\n", entry_count);

    printf("\n######\nFlags:\n######\n");
    char flags[27][20] = {"LOCKED", "ERROR", "REFERENCED", "UPTODATE", "DIRTY", "LRU", "ACTIVE", "SLAB", "WRITEBACK",
                          "RECLAIM", "BUDDY", "MMAP", "ANON", "SWAPCACHE", "SWAPBACKED", "COMPOUND_HEAD", "COMPOUND_TAIL", "HUGE",
                          "UNEVICTABLE", "HWPOISON", "NOPAGE", "KSM", "THP", "OFFLINE", "ZERO_PAGE", "IDLE", "PGTABLE"};

    unsigned long mask;
    for (i = 0; i < 27; i++){
        mask = 1UL << i;
        printf("%2d.%13s: %lu\n", i, flags[i], (entry & mask) >> i);
    }
    return 0;
}

int mapva(address_t pid, address_t virtual_address){
    // open the maps file
    char file_path[100];
    snprintf(file_path, sizeof(file_path), "/proc/%llx/maps", pid);
    FILE* maps_file = fopen(file_path,"r");
    if (maps_file == NULL) {
        perror("Failed to open maps file");
        return 1;
    }

    // read the maps file, find the desired virtual address
    unsigned long long start, end;
    char permissions[5];
    unsigned long long offset, inode;
    int device_major, device_minor;

    char line[256];
    while (fgets(line, sizeof(line), maps_file)) {
        sscanf(line, "%llx-%llx %s %llx %x:%x %llu", &start, &end, permissions, &offset, &device_major, &device_minor, &inode);

        if (virtual_address >= start && virtual_address < end) {
            // Found the memory mapping containing the virtual address
            // break the loop
            break;
        }
    }

    // open the pagemap
    address_t offset_within_mapping = virtual_address - start;

    char pagemap_file_path[100];
    snprintf(pagemap_file_path, sizeof(pagemap_file_path), "/proc/%llx/pagemap", pid);

    FILE* pagemap_file = fopen(pagemap_file_path, "rb");
    if (pagemap_file == NULL) {
        perror("Failed to open pagemap file");
        return 1;
    }

    // calculate PFN using pagemap
    address_t pagemap_entry_size = sizeof(address_t);
    address_t pagemap_entry_offset = (virtual_address / PAGE_SIZE) * pagemap_entry_size;

    if (fseeko(pagemap_file, pagemap_entry_offset, SEEK_SET) == -1) {
        perror("Failed to seek pagemap file");
        return 1;
    }

    address_t pagemap_entry;
    if (fread(&pagemap_entry, pagemap_entry_size, 1, pagemap_file) != 1) {
        perror("Failed to read pagemap file");
        return 1;
    }

    // Extract the page frame number (PFN) from the pagemap entry least significant 48 bits
    address_t pfn = pagemap_entry & 0x0000FFFFFFFFFFFF;
    // calculate the physical address
    address_t physical_address = pfn + offset_within_mapping;
    printf("va= 0x%llx, fnum: 0x0000%llx\n", virtual_address, physical_address);
    return 0;
}

bool is_swap(address_t pagemap_entry) {
    //return pagemap_entry & SWAP_PTE_FLAG;
    return (pagemap_entry >> 62) & 1;
}


bool is_present(address_t pagemap_entry) {
    //return pagemap_entry & PRESENT_PTE_FLAG;
    return (pagemap_entry >> 63) & 1;
}

bool is_file_anon(address_t pagemap_entry) {
    //bool soft_dirty = (pagemap_entry >> 55) & 1;
    //bool file_page = (pagemap_entry >> 61) & 1;
    //return !soft_dirty && file_page;
    return (pagemap_entry >> 61) & 1;
}


bool is_softdirty(address_t pagemap_entry) {
    return (pagemap_entry >> 55) & 1;
}

bool is_exclusive(address_t pagemap_entry) {
    //return pagemap_entry & EXCLUSIVE_PTE_FLAG;
    return (pagemap_entry >> 56) & 1;
}


int pte(address_t pid, address_t virtual_address){

    // open the maps file
    char file_path[100];
    snprintf(file_path, sizeof(file_path), "/proc/%llx/maps", pid);

    FILE* maps_file = fopen(file_path,"r");
    if (maps_file == NULL) {
        perror("Failed to open maps file\n");
        return 1;
    }

    // read the maps file, find the desired virtual address
    unsigned long long start, end;
    char permissions[5];
    unsigned long long offset, inode;
    int device_major, device_minor;

    char line[256];
    while (fgets(line, sizeof(line), maps_file)) {
        sscanf(line, "%llx-%llx %s %llx %x:%x %llu", &start, &end, permissions, &offset, &device_major, &device_minor, &inode);

        if (virtual_address >= start && virtual_address < end) {
            break;
        }
    }
    char pagemap_file_path[100];
    snprintf(pagemap_file_path, sizeof(pagemap_file_path), "/proc/%llx/pagemap", pid);

    FILE* pagemap_file = fopen(pagemap_file_path, "rb");
    if (pagemap_file == NULL) {
        perror("Failed to open pagemap file");
        return 1;
    }

    address_t pagemap_entry_size = sizeof(address_t);
    address_t pagemap_entry_offset = (virtual_address / PAGE_SIZE) * pagemap_entry_size;

    if (fseeko(pagemap_file, pagemap_entry_offset, SEEK_SET) == -1) {
        perror("Failed to seek pagemap file");
        return 1;
    }

    address_t pagemap_entry;
    if (fread(&pagemap_entry, pagemap_entry_size, 1, pagemap_file) != 1) {
        perror("Failed to read pagemap file");
        return 1;
    }

    bool present = is_present(pagemap_entry);
    bool swapped = is_swap(pagemap_entry);
    bool file_anon = is_file_anon(pagemap_entry);
    bool exclusive = is_exclusive(pagemap_entry);
    bool softdirty = is_softdirty(pagemap_entry);
    address_t pfn = pagemap_entry & 0x0000FFFFFFFFFFFF;

    printf("[vaddr=0x%llx, vpn=0x%llx]: present = %d, swapped = %d, file_anon = %d, exclusive = %d, softdirty = %d, pfn = 0x0000%llx\n",
           virtual_address, virtual_address/PAGE_SIZE, present, swapped, file_anon, exclusive, softdirty, pfn);

    return 0;
}

int memused(address_t pid){
    char *address_start;
    char *address_end;
    //char *permissions;
    char *token;

    unsigned long address_start_long;
    unsigned long address_end_long;

    unsigned long total_addresses = 0;
    unsigned long pages_excl_mapped = 0;
    unsigned long pages_mapped = 0;

    unsigned long curr_vpn;
    unsigned long end_vpn;
    unsigned long curr_pfn;

    int fd;
    int fd_count;
    unsigned long offset;
    unsigned long offset_count;

    ssize_t bytes_read;
    ssize_t bytes_read_count;

    unsigned long entry;
    unsigned long entry_count;


    //int i;

    unsigned long mask;


    // virtual memory
    char filename[100];
    sprintf(filename, "/proc/%llx/maps", pid);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open %s\n", filename);
        return -1;
    }

    sprintf(filename, "/proc/%llx/pagemap", pid);
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Failed to open %s\n", filename);
        return -1;
    }

    fd_count = open("/proc/kpagecount", O_RDONLY);
    if (fd_count == -1) {
        printf("Failed to open /proc/kpagecount\n");
        return -1;
    }


    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Tokenize the line by space or tab
        token = strtok(line, "p");

        // Extract the start and end addresses
        address_start = strtok(token, "-");
        address_end = strtok(NULL, " \t");

        // Find the permissions field
        //permissions = strtok(NULL, " \t");

        address_start_long = hex_string_to_long(address_start);
        address_end_long = hex_string_to_long(address_end);

        total_addresses += (address_end_long-address_start_long);

        curr_vpn = address_start_long / 4096;
        end_vpn = address_end_long / 4096;

        while (curr_vpn < end_vpn){
            offset = curr_vpn * sizeof(unsigned long);
            if (lseek(fd, offset, SEEK_SET) == -1){
                printf("lseek error\n");
                return -3;
            }

            bytes_read = read(fd, &entry, BUFFER_SIZE);
            if (bytes_read == -1) {
                printf("Failed to read the entry\n");
                close(fd);
                return -1;
            }
            if (bytes_read == 0) {
                //return -2;
                curr_vpn++;
                break;
            }


            mask = 1UL << 63;
            if (((entry & mask) >> 63) == 0){
                curr_vpn++;
                continue;
            }

            mask = (1UL << 55) - 1;
            curr_pfn = entry & mask;

            //printf("curr_pfn: %lu\n", curr_pfn);

            offset_count = curr_pfn * sizeof(unsigned long);

            if (lseek(fd_count, offset_count, SEEK_SET) == -1){
                printf("lseek error\n");
                return -3;
            }
            bytes_read_count = read(fd_count, &entry_count, BUFFER_SIZE);
            if (bytes_read_count == -1) {
                printf("Failed to read the entry1\n");
                close(fd);
                return -1;
            }
            if (bytes_read_count == 0){
                //return -2;
                curr_vpn++;
                break;
            }

            if (entry_count == 1){
                pages_excl_mapped++;
            }
            if (entry_count > 0){
                pages_mapped++;
            }
            //printf("count: %lu\n", entry_count);

            curr_vpn++;
        }
    }


    fclose (file);
    close(fd);
    close(fd_count);

    //printf("total_addresses: %ld\n", total_addresses);
    printf("Total VM size: %ld KB\n", total_addresses / 1024);
    printf("Excl mapped: %ld KB\n", (pages_excl_mapped * PAGE_SIZE) / 1024);
    printf("Mapped all: %ld KB\n", (pages_mapped * PAGE_SIZE) / 1024);

    return 0;
}




/////////////////////////////////////////////////////////////////////////////////
//maprange pack
static void printInfo(uint64_t address, uint64_t data, int used) {

    printf("mapping:\tvpn=0x%-16lx\t", address / PAGE_SIZE);

    if(!used){
        printf("unused\n");
        return;
    }
    if(((data >> 63) & 1) == 1){
        printf("pfn=%-16lx\n", data & 0x7fffffffffffff);
    } else {
        printf("not-in-memory\n");
    }
}



/////////////////////////////////////////////////////////////////////////////////
//allTableSize
void alltablesize(address_t pid){
    char maps_path[50];
    sprintf(maps_path, "/proc/%llx/maps", pid);
    int map_fd = open(maps_path, O_RDONLY);
    if (map_fd == -1) {
        perror("error opening maps file");
        return;
    }
    FILE *maps_file = fopen(maps_path, "r");


    unsigned long startRange, endRange;
    //unsigned long start;

    unsigned long innerPageCount = 0;
    unsigned long middleInnerCount = 0;
    unsigned long middleOuterCount = 0;
    unsigned long outerCount = 0;

    unsigned long outerSize = 512UL * 512UL * 512UL * 512UL;
    unsigned long middleOuterSize = 512UL * 512UL * 512UL;
    unsigned long middleInnerSize = 512UL * 512UL;
    unsigned long innerSize = 512UL;

    unsigned long pageCount = 0;
    char line[256];
    //int isFirst = 1;
    while (fgets(line, sizeof(line), maps_file)) {
        sscanf(line, "%lx-%lx", &startRange, &endRange);
        // if(isFirst){
        //     start = startRange;
        //     isFirst = 0;
        //     continue;
        // }

        if(startRange != endRange){
            outerCount = 1;
        }
        if(startRange == endRange){
            continue;
        }
        pageCount += (endRange - startRange) / 4096UL;
    }

    innerPageCount = ((pageCount / innerSize) < 1UL) ? 1UL : (pageCount / innerSize);
    middleInnerCount = ((pageCount / middleInnerSize) < 1UL) ? 1UL : (pageCount / middleInnerSize);
    middleOuterCount = ((pageCount / middleOuterSize) < 1UL) ? 1UL : (pageCount / middleOuterSize);
    outerCount = ((pageCount / outerSize) < 1UL) ? 1UL : (pageCount / outerSize);
    unsigned long totalSize = (innerPageCount + middleInnerCount + middleOuterCount + outerCount) * 512LU * 8LU / 1024LU;
    printf("(pid=%llx) total memory occupied by 4-level page table: %lu KB\n",pid, totalSize);

}
/////////////////////////////////////////////////////////////////////////////////


void maprange(address_t pid, address_t va1, address_t va2){
    char maps_path[50];
    sprintf(maps_path, "/proc/%llx/maps", pid);
    int map_fd = open(maps_path, O_RDONLY);
    if (map_fd == -1) {
        perror("error opening maps file");
        return;
    }
    FILE *maps_file = fopen(maps_path, "r");

    char pagemap_path[50];
    sprintf(pagemap_path, "/proc/%llx/pagemap", pid);
    int pagemap_fd = open(pagemap_path, O_RDONLY);
    if (pagemap_fd == -1) {
        perror("error opening pagemap file");
        return;
    }

    unsigned long startRange, endRange;
    unsigned int spanning_range_count = 0;
    unsigned long *startingAd = malloc(sizeof(sizeof(unsigned long)));
    unsigned long *endingAd = malloc(sizeof(sizeof(unsigned long)));

    char line[256];
    while (fgets(line, sizeof(line), maps_file)) {
        sscanf(line, "%lx-%lx", &startRange, &endRange);
        if (endRange <= va1 || startRange >= va2) {
            continue;
        }
        spanning_range_count++;
        endRange = (va2 < endRange) ? va2 : endRange;
        startRange = (va1 > startRange) ? va1 : startRange;

        startingAd =
                realloc(startingAd, spanning_range_count * sizeof(unsigned long));

        endingAd =
                realloc(endingAd, spanning_range_count * sizeof(unsigned long));

        startingAd[spanning_range_count - 1] = startRange;
        endingAd[spanning_range_count - 1] = endRange;

        if (endRange >= va2) {
            break;
        }
    }

    int i;
    //int k;
    endRange = endingAd[0];
    for (i = 0; i < spanning_range_count; i++) {
        startRange = startingAd[i];
        if(startRange - endRange <= 0){
            for (uint64_t k = startRange; k < endRange; k += PAGE_SIZE) {
                printInfo(k, 0, 0);
            }
        }
        endRange = endingAd[i];
        for (uint64_t k = startRange; k < endRange; k += PAGE_SIZE) {
            uint64_t data;
            off_t position = (k / PAGE_SIZE) * sizeof(uint64_t);
            if (lseek(pagemap_fd, position, SEEK_SET) == -1) {
                perror("lseek");
                close(pagemap_fd);
                return;
            }

            if (read(pagemap_fd, &data, sizeof(uint64_t)) != sizeof(data)) {
                perror("read");
                close(pagemap_fd);
                return;
            }

            printInfo(k, data, 1);
        }
    }

    close(pagemap_fd);
}




int mapallin(address_t pid) {
    char maps_path[100];
    char pagemap_path[100];
    FILE* maps_file;
    FILE* pagemap_file;
    char region_name[256];
    char line[256];


    sprintf(maps_path, "/proc/%llx/maps", pid);
    maps_file = fopen(maps_path, "r");
    if (maps_file == NULL) {
        perror("Error open maps file");
        return -1;
    }


    while (fgets(line, sizeof(line), maps_file)) {
        unsigned long start_address, end_address;


        sscanf(line, "%lx-%lx %*s %*s %*s %*s %s", &start_address, &end_address, region_name);

        //printf("Region Name: %s\n", region_name);


        unsigned long long start_page_number = start_address / PAGE_SIZE;
        unsigned long long end_page_number = end_address / PAGE_SIZE;


        sprintf(pagemap_path, "/proc/%llx/pagemap", pid);
        pagemap_file = fopen(pagemap_path, "rb");
        if (pagemap_file == NULL) {
            perror("Error open pagemap file");
            return -1;
        }


        for (unsigned long long page_number = start_page_number; page_number <= end_page_number; ++page_number) {
            off_t offset = page_number * 8;


            if (fseeko(pagemap_file, offset, SEEK_SET) == -1) {
                return -1;
            }

            uint64_t value;

            if (fread(&value, sizeof(value), 1, pagemap_file) != 1) {
                return -1;
            }


            uint64_t frame_number = value & ((1ULL << 54) - 1);


            if (frame_number != 0) {
                printf("Page number: 0x%llx, Frame number: 0x%lx\n", page_number, frame_number);
            }
        }


        fclose(pagemap_file);
    }


    fclose(maps_file);

    return 0;
}
  
int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Insufficient arguments provided.\n");
    return 1;
  }
  char* option = argv[1];
  address_t parameters[MAX_PARAMETERS];
  int numParams = argc - 2;
  char* endPtr;
  for (int i = 0; i < numParams; i++) {
    parameters[i] = strtoul(argv[i + 2], &endPtr, 16);
  }
  
  // Option 2: -frameinfo
  if (strcmp(option, "-frameinfo") == 0) {
      for (int i = 0; i < numParams; i++) {
          parameters[i] = strtoul(argv[i + 2], &endPtr, 10);
      }
      frameinfo( parameters[0]);
    return 0;
  }

  // Option 3: -memused
  else if (strcmp(option, "-memused") == 0) {
    memused(parameters[0]);
    return 0;
  }

  // Option 4: -mapva
  else if (strcmp(option, "-mapva") == 0) {
    mapva(parameters[0], parameters[1]);
    return 0;
  }
  
  // Option 5: -pte
  else if (strcmp(option, "-pte") == 0) {
    pte(parameters[0], parameters[1]);
    return 0;
  }
  // Option 6: -maprange
  else if (strcmp(option, "-maprange") == 0) {
    maprange(parameters[0], parameters[1], parameters[2]);
    return 0;
  }
  // Option 7: -mapall
  else if (strcmp(option, "-mapall") == 0) {
    mapall(parameters[0]);
    return 0;
  }
  // Option 8: -mapallin
  else if (strcmp(option, "-mapallin") == 0) {
    mapallin(parameters[0]);
    return 0;
  }
  // Option 9: -alltablesize
  else if (strcmp(option, "-alltablesize") == 0) {
    alltablesize(parameters[0]);
    return 0;

  }

  else {
    printf("Invalid option: %s\n", option);
    return 1;
  }
}

