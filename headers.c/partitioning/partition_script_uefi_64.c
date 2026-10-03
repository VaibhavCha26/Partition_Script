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
// <parted.h> is the libparted and will do the partition itself but its unstable nad poorly documented;
// if the fork fails - the child dies but if libparted fails - everything crashes or [corrupt a disk due to pointer error ] --> wtf? 
// there's libext4fs too -- both library are very low level and requires manual config of everything 
// wtf ?
//
//run_parted_command is good abstraction but there's np sureity that a correct path would be fed to it 
// !!!!!!! SO MAKE SURE TO ADD CHECKING TO THE GIVEN ARGV TOO BEFORE CALLING run_parted_command.
//
char efi_part[64];
char root_part[64];
//
//
//
#include <linux/fs.h> // ===> it contains the blueprint specifications for how the linux os handles filesystems, block drives and raw storage devices.
// it contains raw highly specific numbers inside this - enginners have mapped those numbers to text words ( macros ) can i manually handle instead of using this 0_0 :P 
int partition_uefi64(char* partitions_to_be_made[],char* argv[]){
  DIR* dir_partition = opendir("/dev");
  if(!dir_partition){perror("opening /dev failed:"); exit(1);}
  
goto_cause_i_am_lazy:
  struct dirent* dir_partition_elements = readdir(dir_partition);

  //
  //
  // another problem how to make sure that its the right file and not some random shit ? 
  //
  // -->> this condition was making sure i can't iterate on the /dev so removed it.
  if((strcmp(dir_partition_elements->d_name, ".") == 0)
    || (strcmp(dir_partition_elements->d_name,"..") == 0) || 
    (dir_partition_elements == NULL) ) {
    //
    if(dir_partition_elements == NULL){printf("Nothing in /dev wtf ?");exit(1);}
    goto goto_cause_i_am_lazy; // dangerous stuff because i am very lazy :P 
    // technically should have used a loop somehow but that would mean restructuring a lot of stuff 
  }
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
  //
  //
  //
  //
partition_failed_goto:
  int size_of_disk = ioctl(partition_file_fd,
                             BLKGETSIZE64,
                             &size); // long may only contain 32 bit so..... idk :P
  // Integer Truncation Overflow - wtf -- if i pass that 64 bit into 32 i will triger memory corruption ( writing things not assigned to it ) 
  // and the kernel driver doesn't look at my C variable declaration - it will forcefully fuck the assignement over ? why T-T 
  if(size_of_disk < 0){perror("ioctl failed getting size");printf("\n");exit(1);}
  // !! WAIT WHAT ABOUT RERTURN NUMBER FROM UEFI_CHECK;
  //
  //
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
  //
  //here it would be a problem if its something else except /dev/vda - recalling this would make this entire thing to go again and take on a already formatted disk.
  if(argv == NULL && uefi_check){
    printf("NULL args : using default:");

    char abs_disk_path[64];
    snprintf(abs_disk_path,sizeof(abs_disk_path),
             "/dev/%s1",dir_partition_elements->d_name);
    // wtf i don't know what i did with the snprintf but apparently the dir_partition_elements would pass that thign as a string and that won't work 
    // as parted would skip that completely.
    //
    //
    //
    //  this one is mklabel and not mkpart - there;s a difference between them mklabel : destroys all the structures and deploy the gpt -- 
    //  that it completely removes the old structure and writes a clean disk sector at the very beginning of the drive - NOT SURE WHAT EXACTLY IT IS - READ THE BOOK; -- I FORGOT T-T
    char* argv[] = {"parted", "-s",
      dir_partition_elements->d_name,   // mklabel should not take abs_disk_path
      "mklabel",
      "gpt",
      NULL
    };
    //
    //
    //not that the disk is in the gpt format - we can change the actual partition format of it;
    char* efi_partition[] = {"parted","-s",abs_disk_path,"mkpart",
    "primary","fat32","1MiB","513MiB",NULL};
    // mkpart hasn't made or changed the file systems - it simply divided the chunk of /dev/whatever and told the kernel sector a to b is locked :)
    // that partititon doesn't have any table, sturcture or concept of files and stuff 
    //
    // primary : simply for the older mbr types - crucial for them - parted is forcing to use it becuase it want to be backwardly compatible - useless on gpt disks.
    // fat32 : ??? wtf is a hex marker?
    //
    //
    //
    //
    //
    int efi_status = run_parted_command(efi_partition);
    if(efi_status != 0){perror("efi partition not created T-T:");exit(1);}
    //
    // HIGH BUG PROBABILITY !!!!
    snprintf(efi_part,sizeof(efi_part),
             "/dev/%s2",dir_partition_elements->d_name);
    //
    // NEED TO FORCE THE PROGRAM TO STOP TILL THE PARTITIONING IS COMPLETE.
    // 1. partprobe -- reread the partition table (kernel)
    // 2. udevadm settle - when i partition the disk - a new device node will be create like /dev/sdb1 - this should watch the 
    //                  something like th udev event queue and pause the script until all triggered kernel device event has been completely processsed.
    //
    //if(){
   //  }   --- something to make the kernel stop from runnning the program to;; the previous command is completed.
    //
    //now root partiton ;
    char* root_partition[] = {"parted","-s",abs_disk_path,"mkpart",
      "ext4","513MiB","100%",NULL}; // use 100% of the remaining starting from 513 MiB and make it ext4
    //
    int root_status = run_parted_command(root_partition);
    if(root_status != 0){perror("root partitioning failed wtf:");exit(1);}
    // creating path to the individual partitions - cause why not ?
    //
    //
    //
    //
    // HIGH BUG PROBABILITY !!!!!!
    snprintf(root_part,sizeof(root_part),
             "/dev/%s2",dir_partition_elements->d_name); // are you sure this is the right element? what about /dev/sda1 and something like that ?
  }
  //
  //
  // and here if the string is passed the checking of the string - is it correct or not should take place.
  if(argv != NULL && strcmp(argv[0],"parted") != 0){
    perror("wrong syscall called : Expected: \"parted\" ");
    exit(1);
  }

  return 0;
}

// ioctl approach 
void partition_status_check(){
}
//
// udevaadm settle approach - wtf is that T-T;

