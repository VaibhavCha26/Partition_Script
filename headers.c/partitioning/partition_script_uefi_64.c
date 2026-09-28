#include "../../headers.h/partitioning/run_partition_call.h"
#include "../../headers.h/uefi_check.h"
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
#include <string.h>
//
//
//run_parted_command is good abstraction but there's np sureity that a correct path would be fed to it 
// !!!!!!! SO MAKE SURE TO ADD CHECKING TO THE GIVEN ARGV TOO BEFORE CALLING run_parted_command.
//
#include <linux/fs.h> // ===> it contains the blueprint specifications for how the linux os handles filesystems, block drives and raw storage devices.
// it contains raw highly specific numbers inside this - enginners have mapped those numbers to text words ( macros ) can i manually handle instead of using this 0_0 :P 
int partition_uefi64(char* argv[]){
  DIR* dir_partition = opendir("/dev");
  struct dirent* dir_partition_elements = 
    readdir(dir_partition);
  //
  //
  // another problem how to make sure that its the right file and not some random shit ? 
  if(strcmp(dir_partition_elements->d_name, "vda") != 0){perror("VDA not found Fuck");exit(1);}
  int partition_file_fd = open(dir_partition_elements->d_name,
                               O_RDWR,O_EXCL,O_NONBLOCK); // why the O_NONBLOCK ?? wtf ? T-T 
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
  //
  // so what partitions do i want now ?
  //
  //
  //here it would be a problem if its something else except /dev/vda - recalling this would make this entire thing to go again and take on a already formatted disk.
  if(argv == NULL && uefi_check{printf("NULL args : using defualt");
    char* argv[] = {"parted", "-s", dir_partition_elements->d_name,"mklabel","gpt",NULL};
  }

  // and here if the string is passed the checking of the string - is it correct or not should take place.
  //
  if(strcmp(argv[0],"parted") != 0){
    perror("wrong syscall called : Expected: \"parted\" ");
    exit(1);
  }
  return 0;
}
