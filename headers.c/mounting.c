#include <stdio.h>
#include <stdlib.h>
#include "../headers.h/uefi_check.h"
#include "../headers.h/mounting.h"
#include "../headers.h/partitioning/format_partitioned_space.h"
#include "../headers.h/partitioning/partition_script_uefi_64.h"
#include <sys/stat.h>  // for mkdir 
#include <sys/mount.h>
#include <stdio.h>

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
int mounting_status(char efi_user_part[64], char root_user_part[64])
{
  int partitioning_status = format_partitioned_space();
  if(efi_user_part == NULL){efi_user_part = efi_part;}
  if(root_user_part == NULL){root_user_part = root_part;}

  if(partitioning_status == 0)
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

    // 1.INODE Bitmap (AVAILIBILITY CHART) - driver reads this (consult ig.) - same is free or in use system.  -- SO THAT IT WON"T HAVE TO READ THE LARGE METADATA
    // 2. Jumps to Inode Table (THE BRAIN) and writes the metadata to fields like Type,perm,size and stuff 
    //
    // 3. Allocation of data block - scans the block bitmap and finds the corresponding free space then links the block to node data map pointers to point to it.
    //     two main things that is . and .. mapping is important and the driver will hardcode it - and map it to those nodes 
    //     and then opens the data block belonging to parent dir and appends the name to node.
    //
    // SUMMARY : Its not like memory - where it has a header-data structure - that is continous here it is scattered and fragmented - theh drive is intentionsally
    // segregated into fragments called the block groups and the inode table contains its data block and that . and .. links inside it.
    // 
    // we can't design it like a ram because to go from say one part to another part in disk i would have to make it go through the entire list 
    // taking useless time (hdd casue they are slow) and in ssd - there's a certain no i can write the ssd -- SOMETHING LIKE SILICON ENDURANCE LIMITS;
    //
    // to solve the time issue the inodes are tightly packed into a centralized inode table and the kernel can just read it and jump straight to the address.
    //  OH SO BASICALLY ALL OF THEM ARE IN ONE BLOCK SO ITS LIKE READING ONLY THAT ONE BLOCK AGAIN AND AGAIN.
    //
    //  this method was used in fat - it had CORRUPTION PROPOGATION - linked list works in ram because even if a pointer gets corrrupted no problem reload and done 
    //  but if it happens in the drive the block is permanently lost because the path to it completely lost.
    //  IN EXT4 no problem because the inode acts as a centralized master hub - if one goes bad - the master still holds path to the next block 
    //
    // 
    // WE HAVE THIS STRUCTURE IN RAM AS WELL BUT ONLY WHEN WE TALK TO THE DISK - we load it into ram CALLED In-Memory Inode Table 
    // however for normal malloc and stuff we don't use use this inodes becasue ram in volatile and physically uniform.
    // unnecessary cpu computational overhead -- accessing is easy in ram and very fast - not slow like in disks.
    //
    // HAMMERING THE SAME block in ssd will kill it - same like earlier - so it uses flash translation layer (FTL) -- remaps the flash cell dynamically.
    //
    //
    // Instead of reading inode table again from disk - take it into cachce -- called teh virtual file system (VFS)
    //
    //   [ Your C Installer Code ]
    //        │
//      (Looks up file)
//            │
//            ▼
//  [ Linux VFS Inode Cache (RAM) ]  <─── Hits this 99% of the time! (Instant)
//          │
//       (Cache Miss)
//            │
//            ▼
//  [ Physical Hard Drive Tracks ]   <─── Only hits this if it's the very first look.
//
    //
    // so its the page cache in the ram only but more specifically its the inode cache and the dentry cache.
    //
    //
    // SO does is it the same combination - of linking like the memory = why is it so inefficient well 
    //
    //   [ THE HARDWARE TRACKS (EXT4 BLOCK GROUP) ]
//  ┌──────────────────┬──────────────────┬──────────────────┐
//  │   1. BITMAPS     │  2. INODE TABLE  │  3. DATA BLOCKS  │
//  │  [011111000...]  │  [Slot 4: Meta]  │  [Block 8402]    │
//  └────────┬─────────┴─────────┬────────┴────────┬─────────┘
//           │                   │                 │
//           │ (Flipped to 1)    │                 │
//         └──────────────────>│ (Points to)     │
//                               └────────────────>│ (Stores Content)
// 
    //
    //  SO THATS WHY SYNC() and FSYNC() is used its basically telling the kernel to remove the vfs inode cache
    //  you have so flush those old inode nodes out and wait for new one.
  
    // scary stuff i have btfrs rights? 
    // this is the installer value - for os installer pass 0 -- but why ? T-T;
    // -o flag for standard ext4 and vfat operations - pass NULL but for 
    //      btrfs pass a string but wait what is mine - i have btrfs right? 
    
    // the /dev/vda or stuff is inherently the root directory and should contain the boot partition as well
    // there's nothing like divinding drive to boot parititona dn then root partitiiona dn stuff. !!!!
    //
    // Plan -- Mount the /mnt to the  "Root" and then make a folder boot inside it and mount the /mnt/boot there -
    // for allowing the bootloader to go there - damn 
    //
    // How is it possible for ROOT TO HAVE BOOT INSIDE IT - EXT4 VS FAT#@ WTF ?
    //  -- The kernel has an advanced abs layer called the VFS - so basically its a manager 
    //  between application and the individual filesystem drivers (fat32 and ext4 drivers T-T)
    //  so my programs folders doesn't matter its just a path and when the kernel access that - "It mounts points as redirection flags"
    //
    // PAIN : i mount the root (ext4) and then creates a file inside it - what happens is that the vfs sees the path 
    // and then gives it to the ext4 driver - which then puts it into the ext4 partition
    // when i mount the boot folder inside this ext4 root - the vfs makes a "REDIRECTOR HOOK"
    //
//
    //                   [ THE VFS ROUTING MATRIX ]
//                          /mnt/ (EXT4 Base)
//                            │
//            ┌───────────────┴───────────────┐
//            ▼ (Normal Path)                 ▼ (The Redirector Hook)
//     /mnt/etc/hostname               /mnt/boot/EFI/
//            │                               │
//    [ EXT4 Driver ]                  [ FAT32 Driver ]
//          │                               │
//  Burns to Root Partition          Burns to Boot Partition

    //
    // for now /dev/vda is fine - later will add checking for it.
    // again same making the kernel wait problem
    if(mount(root_user_part,
             "/mnt",
             "ext4",0,NULL) < 0){
      // T-T - 
      perror("Mounting of root_part failed:");
      exit(1);
    }
    if(mkdir("/mnt/boot",0755)){
      perror("Making directory failed wtf T-T:");
      exit(1);
    }

    if(mount(efi_user_part,
             "/mnt/boot",
             "vfat",
             0,
             NULL) < 0){
      perror("Mounting for boot fomat partition failed:");
      exit(1);
    }
  }
  return 0;
}
