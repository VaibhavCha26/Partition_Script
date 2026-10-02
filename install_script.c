#include <stdio.h>
#include <stdlib.h>
#include "headers.h/header_call.h"
// so during compilation i still have to type that long this out so make a makefile i guess? T-T 
int main(int argc, char* argv[]){

  if(uefi_check){
    //
    // mounting_status --> format..---> partition ---> run_parted_call ---> ???
    //
    mounting_status(NULL,NULL); // for now 
  }
  return 0;
}
