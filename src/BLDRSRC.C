// ==========================================================================
// $Workfile:   BLDRSRC.C  $
// $Revision:   1.2  $
//
// HISTORY:
// Author            Date      Description
// --------------------------------------------------------------------------
//
//
//
// DESCRIPTION
// Builds a resource file for our *View apps, CST, etc.
//
// Command Line argument: (One only)
// ...Path where the resource files reside.
// ==========================================================================
//   Copyright (c) 1998 TeCom Inc. All rights reserved
// ==========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <bios.h>
#include <conio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>

#define MAX_RSRCS 512

typedef unsigned long ULONG;

typedef struct
{
  char name[13];
  long pos;
  ULONG size;
} RLIST_ENTRY_T;

RLIST_ENTRY_T rlist[MAX_RSRCS];

int total_entries;

char buffer[64L * 1024L];

void BuildFile(char* src_path);

char revision[] = "Revision: 1.01";

void main(int argc, char** argv) {
  char first_file[100];
  struct _find_t c_file;

  printf("\n\n%s\r\n", revision);

  if (argc != 2) {
    printf("\nCommand Line argument: (One only)\n");
    printf("....Path where the resource files reside.\n");
    exit(1);
  }

  strcpy(first_file, argv[1]);
  strcat(first_file, "\\*.*");

  if (_dos_findfirst(first_file, _A_NORMAL, &c_file) == 0) {
    while (1) {
      //printf("Adding: %s ", c_file.name);
      strcpy(rlist[total_entries].name, c_file.name);
      rlist[total_entries].size = c_file.size;
      //printf(" %ld\r\n", c_file.size);
      ++total_entries;
      if (_dos_findnext(&c_file) != 0) {
        printf("%d entries found.", total_entries);
        BuildFile(argv[1]);
        break;
      }
      if (total_entries >= MAX_RSRCS) {
        printf("WARNING:  Maximum entries exceeded.\r\n");
        break;
      }
    }
  } else {
    printf("Couldn't file the source directory!\n");
  }
}

long int AddRsrc(int which, char* path, FILE* res_stream) {
  char rpath[100];
  long int rd_size;
  unsigned int blk_size;
  long int bytes_left;
  FILE* stream;

  memset(rpath, 0, 100);
  strcpy(rpath, path);
  strcat(rpath, "\\");
  strcat(rpath, rlist[which].name);
  rd_size = 0;
  blk_size = (unsigned int)(32 * 1024L);
  bytes_left = rlist[which].size;
  stream = fopen(rpath, "rb");
  if (stream != NULL) {
    while (bytes_left > 0) {
      if (bytes_left < (long int)blk_size) {
        blk_size = (unsigned short)bytes_left;
      }
      fread(buffer, 1, blk_size, stream);
      fwrite(buffer, 1, blk_size, res_stream);
      rd_size += blk_size;
      bytes_left -= blk_size;
    }
    fclose(stream);
  }
  return (rd_size);
}

void BuildFile(char* path) {
  int i;
  unsigned long rsize;
  long offset;
  FILE* res_stream;

  res_stream = fopen("View.ILR", "w+b");
  if (res_stream == NULL) {
    perror("Error opening View.ILR: ");
    return;
  }

  fwrite(&total_entries, 1, 2, res_stream);

  offset = 2 + total_entries * sizeof(RLIST_ENTRY_T);

  // save space in file for the resource list
  fwrite(&rlist, sizeof(RLIST_ENTRY_T), total_entries, res_stream);

  for (i = 0; i < total_entries; ++i) {
    rlist[i].pos = offset; // update offset in list
    rsize = AddRsrc(i, path, res_stream);
    offset += rsize;
  }

  //   for (i = 0; i < total_entries; ++i)
  //   {
  //      printf("%s size: %ld offset: %ld\r\n", rlist[i].name,
  //                                    rlist[i].size,
  //                                    rlist[i].pos);
  //   }

  // Now update resource list (after populating all fields) in the resource file
  fseek(res_stream, 2L, SEEK_SET);
  fwrite(&rlist, sizeof(RLIST_ENTRY_T), total_entries, res_stream);
  fclose(res_stream);
}

#if 0 // demonstrates usage

#define MAX_RSRCS 512

typedef struct
{
   char   name[13];
   long   pos;
   long   size;
} RLIST_ENTRY_T;

int resource_count;

RLIST_ENTRY_T rlist[MAX_RSRCS];

ULONG GetResource(char* name, char* buffer)
{
   int         i;
   USHORT      blk_size;
   ULONG      size;
   SLONG      bytes_left;
   FILE* stream;

   size = 0;
   if (resource_count == 0)
   {
      stream = fopen("View.ILR", "rb");
      if (stream != NULL)
      {
         fread(&resource_count, 1, 2, stream);
         fread(rlist, resource_count, sizeof(RLIST_ENTRY_T), stream);
         fclose(stream);
      }
   }

   for (i = 0; i < resource_count; ++i)
   {
      if (stricmp(name, rlist[i].name) == 0)
      {
         stream = fopen("View.ILR", "rb");
         if (stream != NULL)
         {
            fseek(stream, pos, SEEK_SET);
            size = rlist[i].size;
            bytes_left = size;
            blk_size = (unsigned int)(32 * 1024L);
            while (bytes_left > 0)
            {
               if (bytes_left < (long int)blk_size)
               {
                  blk_size = (unsigned short)bytes_left;
               }
               fread(buffer, 1, blk_size, stream);
               buffer += blk_size;
               bytes_left -= blk_size;
            }
            fclose(stream);
         }
         break;
      }
   }
   return(size);
}
#endif
