// This one is the easy orcha. way - supposedly T-T;
// it will only and the other one will only work if i have internet so....
// i setup internet myself ? 
// or should i just do the "local package cache mount" - basically download all the base package binaries and dump those or through
// compression image like squashfs archive.
#include <stdio.h>
#include <stdlib.h>

#include "../../../headers.h/mounting.h"
#include "../../../headers.h/dir_making_options/pacstrap_option.h"
#include "../../../headers.h/dir_making_options/pacstrap_call.h"
#include "../../../headers.h/partitioning/partition_script_uefi_64.h"

int pacstrap_option(char* dir_choices[]){ // same thing - checking for dir_choices - valid or not;
  if(mounting_status(efi_part,root_part) == 0)
  {
    if(dir_choices == NULL)
    {
      char *default_dir_choices[] = {"pacstrap","-K","/mnt",
        "base","linux",
        "linux-firware","nvim","networkmanager",NULL};
      pacstrap_call(default_dir_choices);
    }

    else
    { 
      // check everything about the given dir_choices;
      // what is the best way to do it ?
      pacstrap_call(dir_choices);
    }  
  }

  else {
    perror("idk T-T: Mounting failed wtf ?");
    exit(1);
  }
  return 0;
}
