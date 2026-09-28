#pragma once 

extern char efi_part[64];
extern char root_part[64];
int partition_uefi64(char* partitions_to_be_made[],char* argv[]);
