#pragma once 
// Tells the compiler to only read this file once
#include <dirent.h>
#include <stdbool.h>

extern bool uefi_check;
extern struct dirent *entry_of_dir;
extern int number;

// but why only once; any other smart way?
int uefi_checking(void);
