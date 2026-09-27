#include "../headers.h/partition_script_uefi_64.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ioctl.h>
// i don't know what errno is but ioctl handles it itself somehow 0_0 
#include <unistd.h>
#include <sys/wait.h>

#include <linux/fs.h> // ===> it contains the blueprint specifications for how the linux os handles filesystems, block drives and raw storage devices.
// it contains raw highly specific numbers inside this - enginners have mapped those numbers to text words ( macros ) can i manually handle instead of using this 0_0 :P 
int partition_uefi64(){
  DIR* dir_partition = opendir("/dev");
  struct dirent* dir_partition_elements = 
    readdir(dir_partition);
  //
  int partition_file_fd = open(dir_partition_elements->d_name,
                               O_RDWR,O_EXCL,O_NONBLOCK); // why the O_NONBLOCK ?? wtf ? T-T 
  //
  //
  //
  if(partition_file_fd < 0 ){printf("/dev contains no file wtf?");exit(1);}
  // ioctl is input/output contorl - read and write are for basic files -- for hardware devices (hard drive, graphics card or webcam) - can't write text string
  // to a hard drive or any hardware and tell it to reset its hardware cache.
  //
  // KERNEL DEVICE DRIVER -- wtf is this shit. T-T 
  //
  // why ioctl? same thing : writing to storage drive is very slow so to speed things up the kernel saves the data into page cache ( basically space in ram ) - writing happen later slowly
  // 1. DESYNCHRONIZATION
  // 2. DATA LOSS - if machine loses power
  // flushing the cache mean dropping everything to cpu NOW 
  // ioctl is the "escape hatch system call" allows to send highly specific direct commands straight to hardware device driver.
  //
  int flush_cache = ioctl(partition_file_fd,BLKRRPART,0); // actual flushing i guess and
  if(flush_cache < 0){perror("flush failed:");exit(1);}
  //unsigned long size; // ---> feeling that this will be a issue.
  uint64_t size;
  // free size as well ? 
  int size_of_disk = ioctl(partition_file_fd,
                             BLKGETSIZE64,
                             &size); // long may only contain 32 bit so..... idk :P
  // Integer Truncation Overflow - wtf -- if i pass that 64 bit into 32 i will triger memory corruption ( writing things not assigned to it ) 
  // and the kernel driver doesn't look at my C variable declaration - it will forcefully fuck the assignement over ? why T-T 
  if(size_of_disk < 0){perror("ioctl failed getting size");printf("\n");exit(1);}

  // Actual Partition_time MBR / GPT wtf T-T 
  // GPT one for uefi but what the fuck do i do for mbr?
  // a UEFI motherboard technically can read MBR because of backward compatibility using CSM -- wait so it doesn't matter which one of these i have ? ;p 
  // MBR VS EFI - bookmark :P
  //
  //for efi : sector 0 : "protective mbr", sector 1 : "the gpt header", (wtf*)(sector 2 to 33: partition entries)
  // wtf - efi is way tougher than mbr :( T-T 
  // well archinstall (official) --> orchestrator - it doesn't manually write binary bits for gpt and stuff it uses 
  // pre compiled c tool by GNU team to absorb this pain: parted or (fdisk)
  // instead of writing everything and a custom crc32 algo - master just call fork and execvp() and then runs parted.
  // 
  // <?> i will call parted here. T-T :( - i can't do the manual binary packing right now;
  // well actually no -- so to use the partition call multiple times -- divide it into a sep funciton !!
  // and the input to it would what i have to do -- so like array or args; 
  return 0;
}
//
//
//
//
// great now i can pass the args for the partition i need and i can make partitions with this.
int run_parted_command(char* argv[]){
  // <?> i will call parted here. T-T :( - i can't do the manual binary packing right now;
  //
  //
  pid_t id_parted_fork_call = fork(); 
  // why pid_t and not int -- difference? -- "typedef of int" - platform dependent - short,long etc.
  //
  

  if(id_parted_fork_call == 0){
    //
    //
    // execv'p -- here p means path - no need for absoulute path.
    int execvp_status = execvp(argv[0], argv);
    if(execvp_status < 0){perror("execvp failed:");exit(1);}
    //
    // parted is the file i want to run ! 
    // This is an array of pointers to null-terminated strings that represent the argument list available to the new program
    // -s : prevents from asking the user for confirmation and prompts like do you want to do this :P
    // "/dev/vda" -- target block 
    // "mklabel" -- internal commmand telling parted to creat a new disk label on the device 
    // "gpt" -- type of partition i want to create -- wait.. so... mbr? ;)
    // null pointer is to stop execvp stop reading.
    //
    // Actual execution: 
    //
    //return 0; -- hmm O_O.
  }
  // do i need the childs process status?
  //
  // NULL ? >_> are you sure? :P
  waitpid(id_parted_fork_call,NULL,0);
  return 0;
}
