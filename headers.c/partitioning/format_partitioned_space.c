#include "../../headers.h/partitioning/format_partitioned_space.h"
#include "../../headers.h/partitioning/run_partition_call.h"
#include "../../headers.h/partitioning/partition_script_uefi_64.h"
#include <stdlib.h>
#include <stdio.h>

int format_partitioned_space(){ // can add a part function string weird.
  char* efi_format[] = {"mkfs.vfat","-F","32",efi_part,NULL};
  char* root_format[] = {"mkfs.ext4","-F",root_part,NULL};
  //
  //
  //
  int format_efi_status = run_parted_command(efi_format);
  if(format_efi_status != 0){perror("EFI formatting to fat32 failed");exit(1);}
  // same issue how do i wait for kernel to complete the formatting now ?
  int format_root_status = run_parted_command(root_format)
; if(format_root_status != 0){perror("ROOT formatting to ext4 failed:");exit(1);}
  return 0;
}
