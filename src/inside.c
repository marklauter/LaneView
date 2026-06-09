/*   Copyright 1996 TeCom Inc. All rights reserved                  */

/*
$Log:   J:/intrlane/src/laneview/vcs/INSIDE.C_V  $
 *
 *    Rev 1.38   Jul 30 1998 10:52:14   tmges
 * Give a nice background to "Invalid date"
 * msgbox.
 *
 *    Rev 1.37   Jul 30 1998 10:09:24   tmges
 * Don't log in if system date is prior to 1995
 *
 *    Rev 1.36   Mar 16 1998 01:48:26   tmmsl
 * added some icons
 *
 *    Rev 1.35   Mar 12 1998 21:05:58   tmmsl
 * took out some unused stuff to make memory
 *
 *    Rev 1.34   Mar 11 1998 12:48:52   tmmsl
 * 1. added GetResource()
 * 2. added /tcsrules switch for
 *     log in "this many" days
 *
 *    Rev 1.33   09 Jul 1997 12:36:00   TMGES
 * add _tzset() call after _putenv("TZ=UCT")
 *
 *    Rev 1.32   25 Jun 1997 10:02:58   TMMSL
 * revision 4.1.1 final check in
 * visual changes
 * new configurable estimated bill
 * support for billing dates
 *
 *    Rev 1.31   27 May 1997 11:13:02   TMMSL
 * 1.3.1 updates
 *
 *    Rev 1.30   12 May 1997 16:49:54   TMMSL
 * Minor bug fixes for 4.1.1 LaneView Release
 *
 *    Rev 1.29   18 Mar 1997 08:11:12   TMMSL
 * Changes for InterLane Release 1.3
 *
 *    Rev 1.28   23 Feb 1997 20:05:20   TMMSL
 * 1.3 user interface changes
 *
 *    Rev 1.27   30 Dec 1996 16:30:42   TMMSL
 * fix message when fonts not found
 *
 *    Rev 1.26   27 Dec 1996 14:12:50   TMMSL
 * removed unused variable
 *
 *    Rev 1.25   18 Dec 1996 16:19:56   STGES
 * 1. removed DAWN_DUSK_WEEK
 *                                   dawn_dusk_week;
 *
 *
 *    Rev 1.24   13 Dec 1996 07:15:54   STGES
 * 1. clean up
 *
 *    Rev 1.23   12 Dec 1996 04:59:36   TMMSL
 * Added shut down message to lv_stop()
 *
 *    Rev 1.22   11 Dec 1996 07:07:50   TMMSL
 * 1. removed unreferenced function
 *     declarations
 * 2. removed meter_detail() to uumenu.c
 *
 *    Rev 1.21   22 Nov 1996 19:35:20   TMMSL
 * fixed lv_stop();
 *
 *    Rev 1.20   15 Nov 1996 02:22:28   TMMSL
 * 1. removed is_on_peak()
 * 2. removed show_logo()
 * 3. removed intro_screen()
 * 4. removed copyright()
 * 5. removed references to holidays
 * 6. addded new rates
 * 7. added /d debug switch for main()
 *
 *    Rev 1.19   01 Nov 1996 22:39:24   TMMSL
 * 1. added lv_stop() ... function that exits laneview
 * 2. removed #if ONE_BOX ... #if TWO_BOX
 *     stuff ... all one box code removed
 * 3. made font array 2 diminsional
 * 4. load two fonts into memory
 * 5. enlarged buffer[] to 100
 * 6. removed mouse fields
 * 7. removed ref to enter and waitkb
 * 8. changed vga_pallet
 * 9. changed display mode to vga
 * 10. define screens new way
 *
 *
 *    Rev 1.18   Sep 03 1996 15:09:18   STGES
 * new copyright info inserted
 *
 *    Rev 1.17   Aug 12 1996 15:30:50   DYBBN
 * 1) Changes to allow password to control ability to edit load
 *     control devices.
 * 2) Added put_env command to fix time conversions.
 * 3) Changes to allow prompt before deleting mood files when
 *     no devices are defined.
 * 4) Changed all text from TEMS to LaneView.
 * 5) Changes for new billing structures.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <sys\types.h>

#include "cebus.h"
#include "2box.h"
#include "ctstdr.h" /* req'd by ctree */
#include "ctdecl.h" /* req'd by ctree */
#include "vga.h"
#include "inutil.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "lwindow.h"
#include "ismenu.h"
#include "mloop.h"
#include "calendar.h"
#include "devcat.h"
#include "device.h"
#include "msgbox.h"
#include "inside.h"
#include "insubs.h"
#include "ismenu.h"
#include "mouse.h"

extern void initialize_ctree(void);
extern void initialize_cenode(void);
extern int read_devices(int days_back);
extern int log_off(void);
static void text_intro(int, int);
extern void FindFirstHistory(void);

char ANSWER;
int glb_cat_index;
int exit_no = 0;     //reason for exit... ex:  exit(exit_no);
int MainLoopOpt = 0; //MainLoop flag 0 = don't draw top, 1 = do draw top

char norm_attrib = 0x1f;
char bold_attrib = 0x1e;
char rev_attrib = 0x74;

char buffer[100];
UCHAR font_array[2][4096]; // allows for 2 fonts, light & bold

char icon[17][298];
//char   busy_v[298];
//char   notbusy_v[298];
int secured_user;

void LoadIcon(char* icon, char* filename);
void DisplayIcon(int top, int left, char far* icon);

int im_busy;
extern CURSOR_DEF_T arrow_ms;
extern CURSOR_DEF_T hourglass_ms;
extern CURSOR_DEF_T finger_ms;
extern CURSOR_DEF_T bar_ms;

extern int in_phone_view;
extern char ADVANCED;
extern long ltime;
extern struct tm* tod;
extern int shedding_status;
extern unsigned short DOSAPI_Vector;
extern unsigned char INTERVAL_HERE[];
extern int cebus_msg_waiting;
extern int cebus_count;
extern int cebus_max_tries;
extern int cebus_attempts;
extern int cebus_error_count;
extern int day_tab[2][13];
int enter(int, int); /* input from keyboard
            parameters: type (INTEGER, ALPHA, FLOAT)
                   OK_F9 / NO_F9
            returns:        -1 if ESC entered,
                  -1x for F1 thru F9
                  -20 for F10
                  or entry length */

int current_day_type;
USHORT monitored_devices[MAX_HISTORY];
int mon_devices_used;
int buckets_used;
int show_utility_message;
void(_far* Sender)();

#define MAIN_MAX 8

int shown_devices[MAX_DEVICES];

CUSTOMER cust_info;
METER meter;
//BILLING_CONSTANTS    billing_constants;
//BILLING_INFO       billing_info;

extern int meter_hour;
int cebus_found;

int show_time;
int mouse_found;
int* mouse_fields;

extern struct CEBUSDATA {
  int IRQ;
  int HouseCode;
  int BasePort;
  int OutsideHC;
} cebusSetup;

int in_bar_graph;

int in_monitor;

extern INTERVAL_DATA interval;

DAYS_RATES todays_rates;

LWINDOW_T* glbWindow = NULL;

int LVLITE;
int DEMO;
int LOGGED_IN;
int DEBUG;
ULONG reset_count;

void main(int argc, char* argv[]) {
  int i;
  long ltime;
  struct tm* tod;
  FILE* stream;

  //   LoadIcon(busy_v, "logbusy.ico");
  //   LoadIcon(notbusy_v, "notbusy.ico");

  glb_cat_index = 0;
  DEMO = FALSE;
  LOGGED_IN = FALSE;
  ANSWER = FALSE;
  LVLITE = FALSE;
  DEBUG = FALSE;

  if (_putenv("TZ=UCT") == -1) {
    //printf( "Unable to set TZ\n" );
  }
  _tzset();

  for (i = 1; i < argc; i++) // Test for command line arguments.
  {
    if (strcmpi(argv[i], "/l") == 0) {
      LVLITE = TRUE;
    } else if (strcmpi(argv[i], "/d") == 0) {
      DEMO = TRUE;
    } else if (strcmpi(argv[i], "/a") == 0) {
      ANSWER = TRUE;
    } else if (strcmpi(argv[i], "/m") == 0) {
      DEBUG = TRUE;
      reset_count = 0;
    } else if (strcmpi(argv[i], "/tcsrules") == 0) {
      unlink("devices.dat");
      unlink("events.dat");
      unlink("moods.dat");
      unlink("triggers.dat");
      unlink("security.dat");
      unlink("history.dat");
      unlink("history.ndx");
      LVLITE = TRUE;
      DEBUG = TRUE;
      reset_count = 0;
      secured_user = TRUE;
    }
  }
  if (DEMO == TRUE) {
    ANSWER = FALSE;
    LVLITE = FALSE;
    DEBUG = FALSE;
  }
  if ((DEMO == FALSE) && (LVLITE == FALSE) && (ANSWER == FALSE) && (DEBUG == FALSE) && (argc > 1)) { // print help for the user person
    printf("\nLaneView and LaneView Remote Usage:\n");
    printf("  /L = LaneView Lite\n");
    printf("  /D = Demo Mode\n");
    printf("  /M = Debug Mode\n");
    printf("  /A = Answer Mode [LaneView Remote Only]\n");
    printf("  /H = Help (this screen)\n");
    printf("  /? = Help (this screen)\n");
    exit(255);
  }

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  today = tod->tm_mday;
  this_year = tod->tm_year;
  if ((stream = fopen("user.log", "at")) != NULL) {
    fprintf(stream, "%0.2d/%0.2d/%0.2d\t%0.2d:%0.2d:%0.2d\tSTART UP\n",
            this_month, today, this_year,
            tod->tm_hour, tod->tm_min, tod->tm_sec);
    fclose(stream);
  } else {
    printf("Error opening user.log");
  }
  if ((stream = fopen("iface.dat", "rb")) != NULL) {
    fread(&ADVANCED, sizeof(ADVANCED), 1, stream);
    fclose(stream);
  } else {
    ADVANCED = FALSE;
  }
  if ((stream = fopen("setup.ini", "rb")) != NULL) {
    fread(&confirmation, sizeof(confirmation), 1, stream);
    fclose(stream);
  } else {
    confirmation = TRUE;
  }

  hide_cursor();

  if (_putenv("TZ=UCT") == -1) {
    printf("Unable to set TZ\n");
    lv_stop();
  }

  show_time = FALSE;

  GetResource("fontlght", font_array[0]);
  GetResource("fontbold", font_array[1]);
  get_font_loc(font_array[LGHT]);

#if 0
   if ((stream = fopen("FONTLGHT", "rb")) == NULL)
   {
      printf("fontlght not found");
      exit(255);
   }
   fread(, 1, 4096, stream);
   fclose(stream);
   if ((stream = fopen("FONTBOLD", "rb")) == NULL)
   {
      printf("fontbold not found");
      exit(255);
   }
   fread(font_array[1], 1, 4096, stream);
   fclose(stream);
#endif

  setdisplaymode(0x12); // VGA mode
  vga_pallet(1);

  if ((mouse_found = find_mouse()) != FALSE) {
    mouse_on();
    position_mouse(300, 320);
    mouse_cursor(&arrow_ms);
  }
  time(&ltime);
  tod = localtime(&ltime);

  initialize_ctree();

  if (tod->tm_year < 95) {
    clear();
    msgbox("Your computer's date is wrong. |"
           "Please set the date and try|"
           "again!                     ",
           "Bad Date Detected", MB_OK);
    DEMO = TRUE; // Don't try to log out!!
    lv_stop();
  }

  initialize_cenode();

  if (read_devices(-1) == FAILURE) {
    msgbox("Data transfer from ICU incomplete.", "Process Halted", MB_OK);
    exit_no = 1;
    lv_stop();
  }

  time(&ltime);
  tod = localtime(&ltime);
  //current_day_type = (int)pow(2, tod->tm_wday);

  ticks_set(8, 810L);
  memset(icon, FG_WHT, 15 * 298);
  LoadIcon(icon[0], "bargr.ico");
  LoadIcon(icon[1], "trigger.ico");
  LoadIcon(icon[2], "linegr.ico");
  LoadIcon(icon[3], "mood.ico");
  LoadIcon(icon[4], "dollar.ico");
  LoadIcon(icon[5], "lock.ico");
  LoadIcon(icon[6], "device.ico");
  LoadIcon(icon[7], "cal.ico");
  LoadIcon(icon[8], "schedule.ico");
  LoadIcon(icon[9], "monitor.ico");
  LoadIcon(icon[10], "advance.ico");
  LoadIcon(icon[11], "costeff.ico");
  LoadIcon(icon[12], "exit.ico");
  LoadIcon(icon[13], "about.ico");
  LoadIcon(icon[14], "viewrate.ico");
  LoadIcon(icon[15], "mail.ico");
  LoadIcon(icon[16], "calander.ico");
  //   LoadIcon(busy_v, "busy.ico");

  FindFirstHistory();

  IntroScreen();
  MainLoop(); // call main program loop
} //end of main()

extern LWINDOW_T progWindow[1];
UCHAR* lv_stop_title = "Shutting Down";
void lv_stop(void) {
  PKT_INFO incoming_pkt;
  UCHAR rotate = 0x92;
  UCHAR work[2];

  FILE* stream;
  long ltime;
  struct tm* tod;

  work[1] = 0;

  glbWindow = progWindow;
  progWindow->title = lv_stop_title;
  progWindow->win_pos.top -= 115;
  PutWindow(glbWindow);
  ptext("Leaving LaneView", 218, 256, BG_WHT + FG_BLK);
  ptext("Please Wait...", 240, 265, BG_WHT + FG_BLK);

  if (DEMO == FALSE) {
    if (cebus_found == TRUE) {
      if (exit_no != 1) {
        log_off();
      }
      ticks_set(1, 18L);
      ticks_set(0, 2L);
      while (ticks_check(1) == FALSE) {
        if (CEBUS__GetPacket(&incoming_pkt)) {
          ticks_set(1, 18L);
        }
        if (ticks_check(0) == TRUE) {
          work[0] = rotate;
          ptext(work, 240, 395, BG_WHT + FG_BLK);
          if ((rotate++) > 0x93) {
            rotate = 0x92;
          }
          ticks_set(0, 3L);
        }
      }
      CEBUS__ClosePort();
    }
  }

  CLSFIL(1, COMPLETE);
  CLSFIL(2, COMPLETE);
  CLSFIL(3, COMPLETE);
  CLSFIL(4, COMPLETE);
  CLSFIL(5, COMPLETE);
  CLSFIL(6, COMPLETE);
  CLSFIL(7, COMPLETE);
  CLSFIL(8, COMPLETE);

  show_cursor();
  setdisplaymode(3);
  _fcloseall();

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  today = tod->tm_mday;
  this_year = tod->tm_year;

  if ((stream = fopen("user.log", "at")) != NULL) {
    fprintf(stream, "%0.2d/%0.2d/%0.2d\t%0.2d:%0.2d:%0.2d\tNORMAL PROGRAM TERMINATION\n",
            this_month, today, this_year,
            tod->tm_hour, tod->tm_min, tod->tm_sec);
    fclose(stream);
  } else {
    printf("Error opening user.log");
  }
  exit(exit_no);
}

void Wait(long int wait_ticks) {
  //    position(15,10);
  //    norm("inside");
  //    position(16,10);
  //    norm("        ");
  ticks_set(6, (ULONG)wait_ticks);
  while (ticks_check(6) == 0) {
    ;
  }
  //   position(15,10);
  //   norm("        ");
  //   position(16,10);
  //   norm("outside");
}

//=====================================
// read 24x24 16 color icon
//=====================================
void LoadIcon(char* icon, char* filename) {
  GetResource(filename, icon);
}

// void show_logo(
//   int top_pixel,
//   int left_column,
//   int width_in_bytes,
//   int height_in_pixels,
//   char far* pointer_to_icon
//   );
void DisplayIcon(int top, int left, char far* icon) {
  hide_mouse();
  show_logo(top, left / 8, 3, 24, icon + 10);
  unhide_mouse();
}

#define MAX_RSRCS 512

typedef struct
{
  char name[13];
  long pos;
  ULONG size;
} RLIST_ENTRY_T;

int resource_count;

RLIST_ENTRY_T rlist[MAX_RSRCS];

ULONG GetResource(char* name, char* buffer) {
  int i;
  USHORT blk_size;
  ULONG size;
  SLONG bytes_left;
  FILE* stream;

  size = 0;
  if (resource_count == 0) {
    stream = fopen("View.ILR", "rb");
    if (stream != NULL) {
      fread(&resource_count, 1, 2, stream);
      fread(rlist, resource_count, sizeof(RLIST_ENTRY_T), stream);
      fclose(stream);
    }
  }

  for (i = 0; i < resource_count; ++i) {
    if (stricmp(name, rlist[i].name) == 0) {
      stream = fopen("View.ILR", "rb");
      if (stream != NULL) {
        fseek(stream, rlist[i].pos, SEEK_SET);
        size = rlist[i].size;
        bytes_left = size;
        blk_size = (unsigned short)(32 * 1024L);
        while (bytes_left > 0) {
          if (bytes_left < (long int)blk_size) {
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
  return (size);
}
