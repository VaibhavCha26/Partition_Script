#pragma once


// this for calling all the headers so that the main call function won't get messy.
#include "uefi_check.h"
//
//partition function header files calls.
#include "./partitioning/partition_script_uefi_64.h"
#include "./partitioning/run_partition_call.h"
#include "./partitioning/format_partitioned_space.h"
