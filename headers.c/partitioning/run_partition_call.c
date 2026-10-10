#include "../../headers.h/partitioning/run_partition_call.h"
#include "../../headers.h/safety_steps/user_confirmation.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ioctl.h>
// i don't know what errno is but ioctl handles it itself somehow 0_0 
#include <unistd.h>
#include <sys/wait.h>

int exit_code = 0;
//. The issue is not this function itself — it’s that the surrounding repo never properly defines the command arguments or validates the target device.
//
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
  else if (id_parted_fork_call < 0){
    perror("fork failed in run_parted_command: ");
    exit(1);
  }
  // do i need the childs process status?
  // Yes T-T 
  int goto_counter_wait = 0;
  int child_process_status;
  pid_t pid_waitpid = waitpid(id_parted_fork_call,&child_process_status,WNOHANG);
try_waiting_again:
  pid_waitpid = waitpid(id_parted_fork_call,&child_process_status,WNOHANG);
  if(pid_waitpid == 0){
    usleep(10000);
    if(goto_counter_wait < 100){
      goto_counter_wait++;
      goto try_waiting_again;
    }
    else {
      perror("Waitpid Timeout error: fuck T_T");
      exit(1);
    }
  }
  else if (pid_waitpid < 0){
    perror("child process failed:");
    exit(1);
  }
  // wifexited - for checking if it finished normally -- 0 / 1 
  // wexitstatus - for checking what integer it returns exactly.
  //
  // what for wait ?? waitpid or wait aor what ?
  if(WIFEXITED(child_process_status)){
    // NULL ? >_> are you sure? :P
    // why not wait() ?
    exit_code = WEXITSTATUS(child_process_status);
    return 0;
  }
  else {
    perror("What ? wifexited failed -- like what ??");
    exit(1);
  }
  return -1; // child didn't exit normally;
}
