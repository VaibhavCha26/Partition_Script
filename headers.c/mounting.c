#include <stdio.h>
#include <stdlib.h>
#include "../headers.h/mounting.h"
#include "../headers.h/partitioning/format_partitioned_space.h"
#include <sys/stat.h>


// in operating system - like windows = every partition gets its own isolated letter name..
// 
// in linux when i boot into - mounting in the process of "grafting" the physical storage device onto a specific folder
// branch of dir tree.
//
// /mnt is a folder sitting in my ram wut? -- mount() sys call is there;
// so if i drop something inside mount it will act as a tunnel and force my bytes to reach the physical sectors
//
//
//BEFORE MOUNTING:
//[ Live ISO RAM Tree ] ──> /mnt (Empty Folder in RAM)
//[ Raw Hard Drive ]    ──> /dev/sda2 (Isolated blocks floating in space)

//AFTER MOUNTING ROOT:
//[ Live ISO RAM Tree ] ──> /mnt ═══( Transparent Bridge )═══> [ /dev/sda2 Hard Drive ]
//
// after mounting /mnt is my new hard drive :)
// but there is ext4 and fat32 so. i need to create folders inside /mnt like /mnt/boot and /mnt/root and then mount them
// so that when i call /boot --> fat32 and /sys --> ext4;
// so basically tunnel inside of a tunnel;
//
int mounting_status()
{
  int partitioning_status = format_partitioned_space();
  if(partitioning_status)
  {
  // can use mknod() and mkdir() --> to create those dirs inside /mnt but pain;
  // modern linux kernels hate mknod becuase of metadata corruption :( T-T
  
  // MKDIR : 1. context swtich, 2. path lookup (VFS LAYER) - wut ? - so.. it technically passes /mnt and checks its internal mount table
    // and sees it poining to /dev/sda and goes there; 3. Driver Handshake : gives the /boot to the ext4 file system driver !!! 
    //
  //   Inode Allocation: filename and actual data of file area in different locations. 
    //   Inode is a strict fixed size 128 byte binary structure that lives at the beginning of the partition - so like header for a page;
    //   points to the physcial sectors exactly where the actual data of the drive that hold the content of the file 
    //   -- DOES NOT CONTAIN THE NAME OF THE FILE;
    //          EXT4 PARTITION HARDWARE LAYOUT:
//┌──────────────────┬──────────────────┬──────────────────┐
//│   Inode Bitmap   │   Inode Table    │   Data Blocks    │
//│  [011111000...]  │  [ ... ][Slot 4] │  [Block 8402]    │
//└──────────────────┴──────────────────┴──────────────────┘
//         │                  │                  │
// 1. Find free bit    2. Fill Metadata   3. Allocate Block

    // 1.INODE Bitmap - driver reads this (consult ig.)
  }
  return 0;
}
