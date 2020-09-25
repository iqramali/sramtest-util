#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <syslog.h>

#define SRAM_FILE "/dev/mem"
#define SRAM_OFFSET 0x88000000
#define SRAM_SIZE (256*1024)

unsigned char sram_copy[SRAM_SIZE];

static int test_sram(void *sram) {
    volatile unsigned short *p = sram;
    unsigned i;
    unsigned short mask;
    int ret = 0;

    memcpy(sram_copy, sram, SRAM_SIZE);

    printf("Testing SRAM 0x33cc\n");

    for (i = 0; i < SRAM_SIZE / sizeof(*p); i++)
	   p[i] = 0x33cc;

    for (i = 0; i < SRAM_SIZE / sizeof(*p); i++) {
    	if (p[i] != 0x33cc) {
    	    printf("Error: Failure at address %p: expected 0x%04x, got 0x%04x\n", p+i, 0x33cc, p[i]);
    	    goto bye;
    	}
    }

    printf("Testing SRAM 0xcc33\n");

    for (i = 0; i < SRAM_SIZE / sizeof(*p); i++)
	p[i] = 0xcc33;

    for (i = 0; i < SRAM_SIZE / sizeof(*p); i++) {
    	if (p[i] != 0xcc33) {
    	    printf("Error: Failure at address %p: expected 0x%04x, got 0x%04x\n", p+i, 0xcc33, p[i]);
    	    goto bye;
    	}
    }

    printf("Testing SRAM bits\n");
    for (mask = 1; mask; mask <<= 1) {
    	for (i = 0; i < SRAM_SIZE / sizeof(*p); i++)
    	    p[i] = mask;

    	for (i = 0; i < SRAM_SIZE / sizeof(*p); i++) {
    	    if (p[i] != mask) {
        		printf("Error: Failure at address %p: expected 0x%04x, got 0x%04x\n", p+i, mask,
                        p[i]);
        		goto bye;
    	    }
    	}
    }

    printf("Info: Testing inverted bits\n");
    for (mask = 1; mask; mask <<= 1) {
	for (i = 0; i < SRAM_SIZE / sizeof(*p); i++)
	    p[i] = ~mask;

	for (i = 0; i < SRAM_SIZE / sizeof(*p); i++) {
	    if (p[i] != (unsigned short)~mask) {
		printf("Error: Failure at address %p: expected 0x%04x, got 0x%04x\n", p+i, (unsigned short)~mask,
                p[i]);
		goto bye;
	    }
	}
    }

    printf("Testing numbers\n");

    for (i = 0; i < SRAM_SIZE / sizeof(i); i++)
	((int *)p)[i] = i;
    for (i = 0; i < SRAM_SIZE / sizeof(i); i++) {
	if (((unsigned *)p)[i] != i) {
	    printf("Error: Failure at address %p: expected 0x%04x, got 0x%04x\n", ((unsigned *)p)+i, i,
                ((unsigned *)p)[i]);
	    goto bye;
	}
    }

    printf("Testing inverted numbers\n");

    for (i = 0; i < SRAM_SIZE / sizeof(i); i++)
	   ((int *)p)[i] = ~i;

    for (i = 0; i < SRAM_SIZE / sizeof(i); i++) {
    	if (((unsigned *)p)[i] != ~i) {
    	    printf("Error: failure at address %p: expected 0x%x, got 0x%x\n", ((unsigned *)p)+i, ~i,
                    ((unsigned *)p)[i]);
    	    goto bye;
    	}
    }

    printf("SRAM OK\n");

    ret = 1;

 bye:
    memcpy(sram, sram_copy, SRAM_SIZE);

    return 0;
}

int main(void) {
    int fd;
    void *sram;

    if ((fd = open(SRAM_FILE, O_RDWR)) == -1) {
           perror("open " SRAM_FILE " failed");
           exit(1);
    }

    if ((sram = mmap(NULL, SRAM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
        fd, SRAM_OFFSET)) == MAP_FAILED) {
    	perror("mmap " SRAM_FILE " failed");
    	exit(1);
    }

    if (!test_sram(sram))
	   exit(1);

    exit(0);
}
