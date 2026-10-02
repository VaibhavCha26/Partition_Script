#include <stdio.h>
#include <stdlib.h>
#include "headers.h/header_call.h"
#include "headers.h/partitioning/partition_script_uefi_64.h"
// so during compilation i still have to type that long this out so make a makefile i guess? T-T 
int main(int argc, char* argv[]){

  if(uefi_check){
    printf("uefi_check True \n");
    //
    // mounting_status --> format..---> partition ---> run_parted_call ---> ???
    int partition_uefi64_status = partition_uefi64(NULL,
                                                   NULL); // for now.
    if(partition_uefi64_status == 0){
      int mount_status = mounting_status(NULL,NULL); 
      if(mount_status == 0){printf("mounting_done ! T-T \n");}
      else{printf("mounting failed \n T_T");}

    }
  }
  return 0;
}
