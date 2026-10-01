#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../../../headers.h/dir_making_options/pacstrap_call.h"

int pacstrap_call(char* argv_pacstrap[]){
  pid_t id_pacstrap_fork = fork();
  //
  if(id_pacstrap_fork == 0){
    int execvp_status_pacstrap = execvp(argv_pacstrap[0],
                                        argv_pacstrap);
    if(execvp_status_pacstrap < 0){
      perror("pacstrap_call failed: T-T");
      exit(1);
    }

    else {
      waitpid(id_pacstrap_fork,
              NULL,0);
    }
  }
  return 0;
}
