#include <stdio.h>
#include <stdlib.h>
#include "headers.h/header_call.h"
#include "headers.h/partitioning/partition_script_uefi_64.h"
#include "headers.h/uefi_check.h"
#include "headers.h/safety_steps/user_confirmation.h"
// so during compilation i still have to type that long this out so make a makefile i guess? T-T 
int main(){
  uefi_checking();
  if(uefi_check){
    printf("uefi_check True \n");
    //
    // scanf -- X  i will use write and read but should i and how ? 
    // taking teh default inputs for partiton script call and mount call is not yet implemented.
    //
    // use struct for that 
    
    // mounting_status --> format..---> partition ---> run_parted_call ---> ???
    int partition_uefi64_status = partition_uefi64(NULL,
                                                   NULL); // for now.
    int user_confirmation_partition = user_confirm_status("partition_uefi64");
    if(partition_uefi64_status == 0 && user_confirmation_partition){
      int mount_status = mounting_status(NULL,NULL); 
      int user_confirmation_mounting = user_confirm_status("mounting (NULL passed) ");
      if(mount_status == 0 || user_confirmation_mounting){printf("mounting_done ! T-T \n");}
      //
      // suppose something is opened is some other file or proccess and i call exit here -- then that will stay open right 
      // and it would be dangerous -- how to fix it : not implemented;
      else{printf("mounting failed \n T_T"); exit(1);}

    }
  }
  return 0;
}
