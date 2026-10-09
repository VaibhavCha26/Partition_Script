#include <stdio.h>
#include "../../headers.h/safety_steps/user_confirmation.h"

int user_confirm_status(char* string_to_be_confirmed){
  // instead of calling printf and scanf something can be called too but why use sys calls here ?
  // like suppose i hope that stdio is still there but what if it isn't there -- and does that ever happen?
  printf("Do you confirm that this process shall be called (1,0) : %s",string_to_be_confirmed);
  int user_status;
  scanf("%d",&user_status);
  if( (user_status <= 0) ){
    perror("Confirmation call failed EXIT:");
    return 0;
  }
  else {
    return 1;
  }
}
