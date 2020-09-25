# sramtest-util

Test your Linux Embedded device SRAM by using sramtest-util. It just write the fixed bytes on SRAM and read it back to check whether SRAM works flawlessly.

By default the SRAM is configured as 

 ```
#define SRAM_FILE "/dev/mem"
#define SRAM_OFFSET 0x88000000
#define SRAM_SIZE (256*1024)
 ```

and this configuration needs to be changed according to your device SRAM offset and size.
