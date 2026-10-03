#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "../headers.h/uefi_check.h"
//An installer cannot operate as a simple, forward-moving stream. 
//It is a state machine that relies heavily on a Request-Response-Verify loop.
//installer is like a security guard - each instruction is given to the kernel only when the kernel gives the confirmation for the previous code.
// in case of streaming i/o and pipelines - the user-space buffering becomes less of an issue -- wtf? :(
int number;
bool uefi_check = 0;
struct dirent* entry_of_dir;
// bad way ! if uefi_checking is not called it will contain garbage...
//
#define FW_SIZE "/sys/firmware/efi"
int uefi_checking(void){
  // the installer deals with raw files like /dev/vda -- everything like wiping the partition table needs to be asked to the kernel to do it - without caching them into ram  
  // when fopen and fwrite is called they create a memory and buffer of size 4-8kb into the ram. -- so calling fread won't read the part i asked for but would read the entire thing till buffer is full
  //
  //BUFFER : so the buffer creates the problem with both the fread and fwrite -- and that too both ways if i want to read something -- it will give me old data storing in the buffer -- that wasn't able to be used 
  // because the buffer wasn't filled and for the fwrite if i give some command for something - but that is over so now i don't need that but the buffer will still give it to me when it get filled.
  // -- syscalls are real time while this above abstractions are not so high risk stuff can't be given to them - they won't even give it as command if buffer is not full.
  // 
  // fwrite() step: Write out the vital Partition Table layout map to sector 0. (The C library says: "Got it, saving this in the RAM buffer for later").
  // Next line of code: You calculate a complex division problem to size the next partition, 
  // but you accidentally divide by zero. The program instantly crashes with a Segmentation Fault or Floating Point Exception.Because the program crashed, the hidden RAM buffer is instantly vaporized by the operating system. 
  // The C library never gets the chance to run its final automatic flush.
  //
  // there's something like FLUSH TRAP : so if i did some work with fwrite - something important like disk partitions and stuff - then they won't evne hit the kernel till the buffer of fwrite is full.
  // so if a runtime failure occurs or something similer like powercut -- my instruction dies in the buffer.
  //
  // ALIGNEMENT WTF ? : so storage devices like /dev/sda accepts something like multiples of 512 byte or 4096 bytes -- fread and fopen will cut the buffer whenver they want -- assuming data as a continous stream of bytes
  // so its the concept of sectors -- the storage device is divided into multiple sectors and kernel only accepts them 
  // so main problem is that the data can't go over the sector edge.
  // if i misalligned it the kernel will throw an error like " i don't accept your buffer size i want my sector size"
  //
  // fopen and fread are C lib functions while read, open are kernel sys calls 
  // there's full buffering in fopen and fread and no such in kernel calls
  //in installer there's nothing file or such file systems the only thing there is live data structures of kernel - 
  //
  //
  // time to completely fuck my brain up again and then stop midway T-T;
  //
  // so first send it to the vm using ssh - as a script and the remaining it will handle 
  // or hhtp 8000 smth -- also can change it so that either code goes and it complies there or send the compiled code and run it there,
  // sending the compiled code is bad ? why >_> ?
  //
  // making it a pointer so when i derefrence it inside another file it should work?
  //
  //
  // READDIR MIGHT READ . and .. too !!
  //
  //
  DIR *dir = opendir(FW_SIZE); // DIR and dirent are specific data types that system use for dir and things inside that dir.
  if(dir==NULL){perror("THE SYSTEM IS BIOS:");exit(1);}
  //
next_element_dir:
  entry_of_dir = readdir(dir); // conveyor belt )-);
  if(entry_of_dir==NULL){perror("nothing inside /sys/firmware/efi:");exit(1);}
  // opendir simply gives the dir stream pointer -- so basically the location of conveyor belt ? we need readdir to read inside the dir 
  // and get a struct to individual files inside it for data defined by pointers :| 
  //
  // opendir and struct of opendir is just to check the existence of these files and nothing else -- we already know where the fw_platform_size is ?
  //
  // its going to crash i am having a deep feeling T-T
  if(!strcmp(entry_of_dir->d_name,"fw_platform_size")){
    int file_fd = open("/sys/firmware/efi/fw_platform_size",
                       O_RDONLY); // flags makes the file read only;
    //
    // open gives the file fd and i don't what the fuck is fd 
    if(file_fd<0){
      printf("The system is BIOS");
      return 1;
    }

    char size_buffer[4]; // why char and not int ? wouldn't this 
    ssize_t uefi_type = read(file_fd,size_buffer, //weird why use ssize_t here?
         sizeof(size_buffer)-1); 
    // the last parameter is simply the max amout of [] we can write ig?
    // be safe the null pointer isn't there in the size_buffer i don't know why.
    //
    // CRITICAL HERE: aoti and printf will fail i don't add the null pointer;
    size_buffer[uefi_type] = '\0'; // wtf why didn't "work" ?
    //
    number = atoi(size_buffer);
    if(number == 64){uefi_check = true;}
    else if (number == 32){uefi_check = true;}
    //
    
    close(file_fd);
  }
  // okay time for cs crime
  else{
    goto next_element_dir;
  }
  closedir(dir);
  return number;
}


