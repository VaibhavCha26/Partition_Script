#pragma once 
// Tells the compiler to only read this file once

#include <stdbool.h>
extern bool uefi_check;

// but why only once; any other smart way?
int uefi_checking(void);
