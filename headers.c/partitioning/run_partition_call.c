#include "../../headers.h/partitioning/run_partition_call.h"
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


// abstraction : nice :)
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
