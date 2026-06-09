/*   Copyright 1996 TeCom Inc. All rights reserved                  */

/*
$Log:   J:/intrlane/src/laneview/vcs/INIO.C_V  $
 *
 *    Rev 1.110   Jul 31 1998 14:48:00   tmges
 * Add UtUpdate.exe to list of files to get
 * from ICU during login.
 *
 *    Rev 1.109   Jul 30 1998 10:07:10   tmges
 * Better handle the occurance of a 15-min.
 * interval during the log-in process.
 *
 *    Rev 1.108   Jun 30 1998 13:00:28   tmges
 * Changed wording of messages.
 *
 *    Rev 1.107   May 01 1998 13:38:54   tmges
 * 1. Add Wait() calls to ce_bus_reset().
 * 2. Fix bug in sf_get_reply().  [It wouldn't
 *     work if there were more than 0x0FF
 *     packets left.]
 *
 *    Rev 1.106   Apr 15 1998 09:34:26   tmges
 * 1. Refine Fetch Table & Fetch File routines
 *     to update status more nicely.
 * 2. Routine to unzip received updates.
 *
 *
 *    Rev 1.105   Apr 01 1998 14:16:46   tmges
 * 1.  Make FetchTable() show progress more
 *      often.
 * 2.  Retrieve Device/Whole House
 *      summary through FetchTable().
 *
 *    Rev 1.104   Mar 20 1998 14:52:56   tmges
 * "Fine tune" FetchTable() and FetchFile().
 *
 *    Rev 1.103   Mar 16 1998 11:01:54   tmges
 * in FetchTable()... increased time it'll reset
 * before giving up.
 *
 *    Rev 1.102   Mar 16 1998 02:17:32   tmmsl
 *
 *
 *    Rev 1.101   Mar 16 1998 01:09:48   tmmsl
 * added a NEW_MAIL flag
 *
 *    Rev 1.100   Mar 15 1998 22:59:20   tmges
 * Added FetchTable()
 *
 * CheckEmail calls FetchTable
 *
 *    Rev 1.99   Mar 14 1998 05:01:28   tmges
 * make rate file continue to be:
 *              RTmmYY
 * until 2000 AD, when it becomes
 *              RTmmYYYY
 *
 *
 *    Rev 1.98   Mar 14 1998 04:37:34   tmmsl
 *
 *
 *    Rev 1.97   Mar 13 1998 03:13:42   tmges
 * Removed unused items
 *
 *    Rev 1.96   Mar 13 1998 01:36:04   tmges
 * 1.  Convert2Release 160() call added to
 *      read_devices().
 * 2.  CheckEmail() modified
 *
 *    Rev 1.95   Mar 12 1998 13:30:00   tmmsl
 * added email receive capability
 *
 *    Rev 1.94   Mar 11 1998 12:53:26   tmmsl
 * 1. added FetchFile() for improved data
 *     xfer
 * 2. added ability to download LaneView
 *     updates with view.ilr
 * 3. added ability to download billing.prg
 * 4. added DeleteFile()
 *
 *    Rev 1.93   Jan 26 1998 12:50:38   tmmsl
 * GetTable() calls for variable length fetches
 * given a length instead of 0.
 *
 *    Rev 1.92   08 Jan 1998 11:15:18   TMMSL
 * 1. fixed ticks_set() & ticks_check()
 *     to work with Windows95
 *
 *    Rev 1.91   07 Jan 1998 15:45:46   TMMSL
 * 1. moved critical ticks check from
 *     recieve_table()
 * 2. reordered the function & data declarations
 *     at begining of file
 *
 *    Rev 1.90   05 Jan 1998 08:02:36   TMMSL
 * changes for 1.5.  I thought this file was
 * checked in before the holidays but
 * was wrong.   I don't know what changes
 * have been made, but it looks like they were
 * just clean up.
 *
 *    Rev 1.89   29 Jul 1997 20:13:20   TMGES
 * Add decompression for Whole House &
 * Submeter detail tables
 *
 *    Rev 1.88   29 Jul 1997 17:06:24   TMMSL
 * changed tabs
 *
 *    Rev 1.87   14 Jul 1997 16:08:46   TMMSL
 * 1. Changed ReadDevicesDemo() to
 *     ReadDevicesFromDisk()
 * 2. Added functionality to
 *     ReadDevicesFromDisk()
 * 3. Changed f_buffer[] to [64L*1024L] from
 *     [63*1024+1023]
 * 4. Changed version to 1.4.0
 * 5. Now reading security device at startup
 *     from disk or ICU if needed.
 * 6. Added UpdateHistoryFast()
 * 7. Only get data from ICU when the data
 *     on PC is out of date
 *
 *    Rev 1.85   25 Jun 1997 10:02:48   TMMSL
 * revision 4.1.1 final check in
 * visual changes
 * new configurable estimated bill
 * support for billing dates
 *
 *    Rev 1.84   27 May 1997 11:12:56   TMMSL
 * 1.3.1 updates
 *
 *    Rev 1.83   12 May 1997 16:49:50   TMMSL
 * Minor bug fixes for 4.1.1 LaneView Release
 *
 *    Rev 1.82   18 Mar 1997 08:11:08   TMMSL
 * Changes for InterLane Release 1.3
 *
 *    Rev 1.81   23 Feb 1997 20:05:16   TMMSL
 * 1.3 user interface changes
 *
 *    Rev 1.80   12 Feb 1997 16:02:08   TMMSL
 * move text displayed during log in down by
 * 115 pixels
 *
 *    Rev 1.79   11 Feb 1997 11:15:56   TMMSL
 * 1. added user button log function
 * 2. changed login screen look & feel
 *
 *    Rev 1.78   31 Dec 1996 13:25:16   STGES
 * 1. Made variable:  in_ce_bus_reset
 *     a public.
 *
 *
 *    Rev 1.77   30 Dec 1996 11:39:18   TMMSL
 * changed msgbox for read rates on error
 * other than file not found
 *
 *    Rev 1.76   30 Dec 1996 11:33:06   TMMSL
 * changed error message for rate
 * file not found during read_devices()
 *
 *    Rev 1.75   30 Dec 1996 08:08:20   STGES
 * 1. Lengthened packet sent by
 *     RemoveEvent by 1 byte.
 * 2. Removed extra msgbox when checking
 *     dates of rate tables
 *
 *    Rev 1.74   27 Dec 1996 19:08:58   STGES
 * 1. Make file changes on icu visible to
 *     LaneView
 *
 *
 *    Rev 1.73   27 Dec 1996 17:49:26   TMMSL
 * changed index into dmonitor.. variables
 *
 *    Rev 1.72   27 Dec 1996 14:17:08   TMMSL
 * 1. fixed version conflict over run
 * 2. changed AskToReload()
 * 3. changed dmonitor... variables' subscript
 * from MAX_DEVICES to MAX_HISTORY
 *
 *    Rev 1.71   23 Dec 1996 14:02:48   TMMSL
 * read number of devices defined on ICU
 * directly to dev_list.devices_used instead
 * of into local variable icu_devices_used
 * first.
 *
 *    Rev 1.70   23 Dec 1996 13:11:36   STGES
 * 1. Update reload of devices & events
 *     for new tables, etc.
 * 2. Handle situation where new rates have
 *     been sent to ICU, & LaneView already
 *     has downloaded.
 *
 *
 *    Rev 1.69   21 Dec 1996 13:59:00   TMMSL
 * added sequence number to
 * RemoveEvent()
 *
 *    Rev 1.68   21 Dec 1996 13:13:42   STGES
 * 1. Make send_load_ctrl_override() return
 *     a completion code.
 *
 *
 *    Rev 1.67   21 Dec 1996 12:01:16   STGES
 * gave read_load_ctrl_info() a return type
 *
 *    Rev 1.66   20 Dec 1996 18:40:38   STGES
 * 1. Changed load_ctrl[] to load_ctrl_tbl[];
 * 2. and size of above from
 *     MAX_CONTROLS
 *     to MAX_CONTROLS+1
 *
 *    Rev 1.65   20 Dec 1996 17:29:54   TMMSL
 * moved some #define s around
 *
 *    Rev 1.64   20 Dec 1996 08:03:42   TMMSL
 * removed unreferenced variables to clear
 * compiler warning
 *
 *    Rev 1.63   19 Dec 1996 14:54:12   TMMSL
 * 1. removed malloc()
 * 2. removed #include <malloc.h>
 * 3. removed insert_event
 * 4. changed delete_event to RemoveEvent()
 * 5. removed lbutton()
 * 6. removed rbutton()
 *
 *    Rev 1.62   16 Dec 1996 08:12:10   TMMSL
 * moved log_on() to end of read_devices()
 * and synchronize()
 *
 *    Rev 1.61   16 Dec 1996 04:23:06   STGES
 * 1. Made references match new EMMENU
 *     functions.
 *
 *    Rev 1.60   16 Dec 1996 01:53:56   TMMSL
 * removed references to old adjust_events()
 * and added new AdjustEvents()
 *
 *    Rev 1.59   16 Dec 1996 00:03:22   TMMSL
 * Fixed RemoveTrigger();
 *
 *    Rev 1.58   15 Dec 1996 23:42:40   TMMSL
 * commented out part of user_backup
 *
 *    Rev 1.57   15 Dec 1996 23:38:36   TMMSL
 * minor change
 *
 *    Rev 1.55   15 Dec 1996 10:24:02   TMMSL
 * added GetTable for triggers in
 * read_devices()
 *
 *    Rev 1.54   15 Dec 1996 06:19:10   TMMSL
 * added #include "trigger.h"
 *
 *    Rev 1.53   14 Dec 1996 22:54:20   TMMSL
 * added comments to confusing code
 * in RemoveDevice()
 *
 *    Rev 1.52   14 Dec 1996 21:25:34   TMMSL
 * 1. added process to reset load control
 *     priorities to zero on remove device
 *
 * this means "attached" M&M's or devices
 * will have their priority set to zero when one
 * or the other is deleted from ICU
 *
 * 2. clear status bar at bottom of screen
 *     on SUCCESS of various functions that
 *     use the status bar
 *
 *    Rev 1.51   14 Dec 1996 05:34:08   TMMSL
 * changed log in status message
 *
 *    Rev 1.50   14 Dec 1996 02:10:42   STGES
 * 1.  Updated GetTable(), added SendPacket,
 *      and updated ReceiveTable().  For
 *      better error handling.
 * 2.  Changed calls to GetTable to properly
 *      handle expected return codes.
 *
 *
 *    Rev 1.49   13 Dec 1996 22:56:32   TMMSL
 * 1. added #define RT_...
 *
 *    Rev 1.48   13 Dec 1996 00:31:18   TMMSL
 * tried to fix midnight history request
 * processing problem
 *
 *    Rev 1.47   12 Dec 1996 03:58:44   STGES
 * 1. Use graphs.h for definitions.
 * 2. change from update_meter_detail()
 *                      to update_device_detail()
 *     which is found in graphs.c
 *
 *    Rev 1.46   12 Dec 1996 03:22:44   TMMSL
 * fixed get_file() (window stuff)
 *
 *    Rev 1.45   12 Dec 1996 00:35:36   TMMSL
 * moved log_on() to before read history.
 *
 *    Rev 1.44   11 Dec 1996 06:56:58   TMMSL
 * Minor changes to SendObject()
 *
 *    Rev 1.42   22 Nov 1996 19:33:22   TMMSL
 * 1. removed read_rates();
 * 2. removed process_message();
 *
 *    Rev 1.41   15 Nov 1996 02:10:38   TMMSL
 * 1. removed references to holiday files
 * 2. added progress indicator to login screen
 * 3. changed all getch() to kbchar()
 *
 *    Rev 1.40   11 Nov 1996 21:18:52   TMMSL
 * 1. Added progress indicator to login screen.
 * 2. Changed process_interval functions to
 *     receive PKT_INFO parameter so they
 *     operate on the correct packet.
 *
 *    Rev 1.39   11 Nov 1996 20:26:48   TMMSL
 * 1. added function PacketType(PKT_INFO *)
 *
 *    Rev 1.38   11 Nov 1996 19:45:20   TMMSL
 * 1. removed function frame_3d() to progind.c
 * 2. changed references to user_input() to
 *    MainLoop()
 *
 *    Rev 1.37   01 Nov 1996 22:25:18   TMMSL
 * 1.  added new includes
 * 2.  added beep() with out ctrl+break check
 * 3.  added ticks timer number for cursor
 *      blink
 * 4.  removed #if ONE_BOX ... #if TWO_BOX
 *      stuff (all one box code removed)
 * 5.  added show title
 *
 *    Rev 1.36   23 Sep 1996 15:42:22   STGES
 * 1.  Try sending device_body more before
 *      giving up. (chg device, add device, etc.)
 *
 *    Rev 1.35   Sep 12 1996 17:56:42   STGES
 * 1. Handle lost packets better.
 * 2. fixed some tabs.
 *
 *
 *    Rev 1.34   Sep 03 1996 15:09:12   STGES
 * new copyright info inserted
 *
 *    Rev 1.33   Aug 19 1996 16:38:00   DYBBN
 * Fix for problem report #15 - Corrected subroutines that delete events.
 *
 *    Rev 1.32   Aug 14 1996 10:47:44   DYBBN
 * Fixed problem that with receive_table subroutine that became apparent
 * when we increased the maximum number of devices to 64.
 *
 *    Rev 1.31   Aug 12 1996 15:26:04   DYBBN
 * 1) Added subroutine to request billing constants.
 * 2) Changed enter subroutine to add password data type.
 * 3) Changed send_table to allow multi-packet table sends.
 * 4) Changed device add/edit subroutines for new device data structure.
 * 5) Added subroutine to read load control information.
 * 6) Added subroutine to read device status information.
 * 7) Added subroutine to send device status information.
 * 8) Added subroutine to send load control override message.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <ctype.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <sys\utime.h>
#include <process.h>

#include "ctstdr.h" /* req'd by ctree */
#include "ctdecl.h" /* req'd by ctree */
#include "cebus.h"
#include "2box.h"
#include "vga.h"
#include "scrnobjt.h"
#include "lwindow.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "progind.h"
#include "rate.h"
#include "mloop.h"
#include "device.h"
#include "msgbox.h"
#include "inside.h"
#include "event.h"
#include "moodmenu.h"
#include "inutil.h"
#include "inio.h"
#include "device.h"
#include "inhist.h"
#include "graphs.h"
#include "trigger.h"
#include "emmenu.h"
#include "copyrite.h"
#include "scrtypnl.h"
#include "mailhdr.h"

#define RT_SUCCESS 0
#define RT_TIMEOUT 1
#define RT_CERROR 2
#define RT_FD 3
#define RT_NOHIST 4
#define RT_ICU_BUSY 5
#define RT_BAD_PWD 6
#define RT_NO_MAIL 7 //no mail

extern int NEW_MAIL;
int CheckEmail(void);
static int DeleteFile(UCHAR* fname);
extern int kbchar(void);
extern int close_day(void);
extern int outside_time(struct tm*);
extern int update_history(USHORT, int);
extern int IsSubmeter(DEVICE*);
extern METER meter;
extern DEVICE_HISTORY device_history[];
extern CUSTOMER cust_info;
extern DAYS_RATES todays_rates;
extern INTERVAL_DATA interval;
extern INTERVAL_DATA meter_interval;
extern BILLING_CONSTANTS billing_constants;
extern BILLING_INFO billing_info;
extern HOLIDAY_TAB holidays;
extern long time_delta;
extern struct tm* tod;
extern int mon_devices_used;
extern int in_bar_graph;
extern int util_device_selection;
extern int uuDateSelection;
extern int today;
extern int this_month;
extern int this_year;
extern int field_changed;
extern int enter_key;
extern char buffer[];
extern int show_time;
extern UCHAR version[];
extern UCHAR longversion[];

//extern USHORT   monitored_devices[];

//Convert2Release160():
//      checks for & converts from previous history index files
//      to the Release 1.6.0 (Y2K - compliant) version.
extern void Convert2Release160(void);

PKT_INFO pkt_info;
int cebus_msg_waiting;
int cebus_count;
int cebus_max_tries;
int cebus_attempts;
int in_ce_bus_reset;
int in_phone_view;
int RELOAD;
long req_id;
UCHAR far f_buffer[64L * 1024L];
DAWN_DUSK_TIME dawn_dusk_time;
LOAD_CTRL load_ctrl_tbl[MAX_LOAD_CTRLS + 1];
DEVICE_STATUS device_status;
INTERVAL_DATA$ interval$;
int monitor_cents[15];
int dmonitor_cents[MAX_HISTORY][15];
double monitor_kwh[15];
double dmonitor_kwh[MAX_HISTORY][15];
int monitor_count;
int dmonitor_count[MAX_HISTORY];
int meter_hour;
int mouse_buttons;
time_t sync_check;
unsigned short DOSAPI_Vector = 0x60;
CEBUS_CONFIG ce_configuration;
int use_getch;
USHORT running_sequence;
ULONG start_ticks[10];
ULONG number_ticks[10];
// ticks[0] used
// ticks[1] used in demo(), get_reply()
//               GetTable(), SendPrg()->sf_get_reply()
// ticks[2] used in ce_bus_reset(), log_off(), send_backup_files()
// ticks[3] used in phone.c
// ticks[4] used for text_box blinking cursor
// ticks[5] used for get_file(), RecFileSize(), RecFBlock(), DelFile()
// ticks[6] used by Wait()
// ticks[7] used by ShowMouse()
// ticks[8] used by phone view()
// ticks[9] used by mloop.c

/* next three from cebus.dat file */

struct CEBUSDATA {
  int IRQ;
  int HouseCode;
  int BasePort;
  int OutsideHC;
} cebusSetup;

int FetchFile(char* fname);
static long int RequestFSize(UCHAR* fname);
static int RecFileSize(long int* fsize);
static int CollectFile(char* fname);
static int RecFBlock(int block_number,
                     USHORT* pkt_mask,
                     PROGRESS_INDICATOR_T* pi,
                     int* cur_pkt_count);

int FetchTable(void* table_buf,
               int table_size,
               int table_id,
               void* request_data,
               int req_length,
               int retries,
               int show_status);
static long int RequestTSize(int table_id, UCHAR* request_data, int req_length);
static int RecTableSize(long int* tsize);
static int CollectTable(char* out_buffer, int show_status);
static int RecTBlock(int block_number,
                     USHORT* pkt_mask,
                     int show_status,
                     PROGRESS_INDICATOR_T* pi,
                     int* pkt_count);

void decompress(char* ocp, char far* icp, USHORT size);
static int SendPassword(UCHAR* password);
static int SendPacket(PKT_INFO* packet);
static int ReceiveTable(void* table_ptr, int table_size, int display_line, int table_id);
static int log_on(void);
static int get_file(int show_prog);
static int get_reply(USHORT*);
static int get_reply2(USHORT*, UCHAR*);
static void ce_bus_reset(void);
static int send_packet(PKT_INFO* packet);
static int send_it(char* dst, struct stat* buf, FILE* stream);
static int sf_get_reply(int which, int* outside_packets);
static int CheckRateTableTimes(void);
static int AskToReload(void);
static int send_custbkup(void);
static void ReadDevicesFromDisk(void);
static int verify_file_date(char* fname, struct _stat* buf);

void box(UCHAR* msg) {
  msgbox(msg, "BOX", MB_OK);
}

static unsigned char VALID_PKT[5] = {0x050, 0x0E8, 0x050, 0x003, 0x044};
UCHAR PacketType(PKT_INFO* pkt) {
  int pkt_type = 0;

  if (memcmp(&pkt->pkt.INFO[0], VALID_PKT, 5) != 0) {
    return 0;
  }
  time(&sync_check);
  return pkt->pkt.INFO[5];
}

void setdisplaymode(int mode) {
  union REGS regs;

  regs.x.ax = mode;
  int86(0x10, &regs, &regs);
}

int htoi(char* cp) {
  int i, j;
  unsigned char c;

  while (*cp == ' ') {
    if (*cp == 0) {
      return 0;
    }
    ++cp;
  }
  for (i = 0, j = 0; *(cp + i) != 0; ++i) {
    c = *(cp + i);
    if (c == ' ') {
      continue;
    }
    c = toupper(c);
    if (c <= '9') {
      if (c >= '0') {
        j *= 16;
        j += (c & 0x0f);
      } else {
        return j;
      }
      continue;
    } else if (c <= 'F') {
      if (c >= 'A') {
        j *= 16;
        j += ((c - 'A' + 10));
      } else {
        return j;
      }
      continue;
    }
  }
  return j;
}

void show_cursor(void) {
  union REGS regs;

  regs.h.ah = 1;
  regs.x.cx = 0x0607;
  int86(0x10, &regs, &regs);
}

void hide_cursor(void) {
  union REGS regs;

  regs.h.ah = 1;
  regs.x.cx = 0x2000;
  int86(0x10, &regs, &regs);
}

typedef struct
{
  unsigned char day;
  unsigned char hour;
  unsigned char min;
  unsigned char sec;
  unsigned char hsec;
} TICK_TIMER;

TICK_TIMER timer[20];

void ticks_set(unsigned short which, unsigned long how_many) {
  struct _dosdate_t date;
  struct _dostime_t time;

  /* Get current date and time values */
  _disable();
  _dos_getdate(&date);
  _dos_gettime(&time);
  _enable();

  timer[which].day = date.day;
  timer[which].hour = time.hour;
  timer[which].min = time.minute;
  timer[which].sec = time.second;

  how_many *= 55L;
  how_many /= 10L;
  how_many += time.hsecond;
  timer[which].hsec = (unsigned char)(how_many % 100);
  how_many /= 100;
  if (how_many == 0) {
    return;
  }

  how_many += timer[which].sec;
  timer[which].sec = (unsigned char)(how_many % 60);
  how_many /= 60;
  if (how_many == 0) {
    return;
  }

  how_many += timer[which].min;
  timer[which].min = (unsigned char)(how_many % 60);
  how_many /= 60;
  if (how_many == 0) {
    return;
  }

  how_many += timer[which].hour;
  timer[which].hour = (unsigned int)(how_many % 24);
  how_many /= 24;
  if (how_many == 0) {
    return;
  }
}

unsigned short ticks_check(unsigned short which) {
  struct _dosdate_t date;
  struct _dostime_t time;

  /* Get current date and time values */
  _dos_getdate(&date);
  _dos_gettime(&time);

  if (date.day != timer[which].day) {
    return (TRUE); // timed out
  }

  if (memcmp((void*)&time, (void*)&timer[which].hour, 4) > 0) {
    return (TRUE); // timed out
  } else {
    return (FALSE); // not yet
  }
}

#if 0
void display_pkt(PKT_INFO* packet)
{
   char    work[80];
   int     i, total;

   //    position(23,1);
   fillz(work, 79, ' ');
   printf(work);
   //    position(23,1);
   printf("Len: %d:", packet->length);
   printf(" %2X ", packet->pkt.CONTROL);
   printf("%4X ", packet->pkt.DA);
   printf("%4X ", packet->pkt.DHC);
   printf("%4X ", packet->pkt.SA);
   printf("%4X | ", packet->pkt.SHC);
   for (i = 0, total = 33; i < (packet->length - 9); ++i)
   {
      //if( (total += 3) > 80 )
      //    break;
      printf("%2X ", packet->pkt.INFO[i]);
   }
}
#endif

process_interval$(PKT_INFO* pkt_info) {
  //////////////////////////////////////////////////////////////////////////
  // get mode_byte1 & mode_byte2 & mode_byte3 here in addition to day mask
  //////////////////////////////////////////////////////////////////////////
  memcpy(&interval$, &pkt_info->pkt.INFO[6], sizeof(INTERVAL_DATA$));
  meter.tod_current_day$[interval$.hour] += interval$.tod;
  meter.tod_mtd$[interval$.hour] += interval$.tod;
  meter.tod_ytd$[interval$.hour] += interval$.tod;
  meter.var_current_day$[interval$.hour] += interval$.var;
  meter.var_mtd$[interval$.hour] += interval$.var;
  meter.var_ytd$[interval$.hour] += interval$.var;
  return (0);
}

process_interval(PKT_INFO* pkt_info) {
  char work[100];
  int i;
  int dev;
  int mon_dev_idx;
  int row;
  int col;
  int ecol;
  long int lwork;
  double avg;
  double xwork;
  struct tm outside_tm;

  //display_pkt(&pkt_info);

  memcpy(&interval, &pkt_info->pkt.INFO[6], sizeof(INTERVAL_DATA));

  if (interval.device_slot == 0x0FFFF) {
    meter_hour = interval.hour;
    memcpy(&meter_interval, &interval, sizeof(INTERVAL_DATA));
    if (meter.first_today == 0) {
      meter.first_today = interval.readingRsecs;
    }
    if (meter.current_dial == 0) {
      meter.current_dial = interval.readingRsecs;
    }
    current_day_type = interval.day_mask;
    if (interval.newday != 0) {
      close_day();
      outside_time(&outside_tm);
      read_rates((UCHAR)(outside_tm.tm_mon + 1),
                 (UCHAR)outside_tm.tm_mday,
                 (UCHAR)outside_tm.tm_year,
                 &todays_rates);
      meter.first_today = interval.readingRsecs;
      if (in_device_detail) {
        if (in_device_detail == 0x0FFFF) {
          device_detail(0x0FFFF);
        } else {
          device_detail(in_device_detail - 1);
        }
      }
    }

    // for monitor history
    for (i = 14; i > 0; --i) {
      monitor_cents[i] = monitor_cents[i - 1];
      monitor_kwh[i] = monitor_kwh[i - 1];
    }

    avg = (double)interval.kwh / 10000;

    xwork = avg;
    monitor_kwh[0] = xwork;
    xwork *= ((double)(interval.cost_kwh) / 1000.0);
    i = (int)(xwork + .5);
    xwork = i;
    monitor_cents[0] = (int)xwork;
    if ((++monitor_count) > 13) {
      monitor_count = 14;
    }
    lwork = interval.readingRsecs - meter.current_dial;
    meter.current_day[interval.hour] += lwork;
    meter.mtd[interval.hour] += lwork;
    meter.ytd[interval.hour] += lwork;
    lwork *= interval.cost_kwh;
    lwork /= 10000;
    meter.current_day$[interval.hour] += interval.dollars;
    meter.todays_dollars += interval.dollars;
    meter.mtd$[interval.hour] += interval.dollars;
    meter.ytd$[interval.hour] += interval.dollars;
    meter.current_dial = interval.readingRsecs;
    if ((in_7_min_monitor == TRUE) && (emon_device_idx == 0x0FFFF)) {
      scroll_window_down(8, 2, 21, 79);
      area_clear(7 * 14, 7 * 14 + 13, 8, 79 * 8, FG_WHT);
      sprintf(work, "%7.1f", avg);
      position(8, 2);
      text(work + 2, BG_WHT + FG_BLU);
      row = 7 * 14 + 3;
      col = 6 * 8 + 4;
      i = (int)(avg * 7 + .5); // times 7 to make bar longer
      avg = i;
      ecol = col + (int)(avg * 5);
      if (ecol > 631) {
        ecol = 631; /* sanity check */
      }
      if (ecol < col) {
        ecol = 631; /* sanity check */
      }
      hline(row + 0, col, ecol, FG_YEL);
      hline(row + 1, col, ecol, FG_YEL);
      hline(row + 2, col, ecol, FG_YEL);
      hline(row + 3, col, ecol, FG_YEL);
      hline(row + 4, col, ecol, FG_YEL);
      hline(row + 5, col, ecol, FG_YEL);
    } else if (in_device_detail == 0x0FFFF) {
      update_device_detail(0x0FFFF);
    } else if (in_bar_graph) {
      horizontal_graph(uuDateSelection);
    } else if (in_energy_monitor) {
      emon_horizontal_graph();
    }
  } else // It's a monitored device
  {
    dev = LookupDev(interval.device_slot);
    if (dev == -1) //device not found
    {
      return (0);
    }

    mon_dev_idx = dev_list.devices[dev].history_bucket - 1;

    // for monitor history
    // pushing history down
    for (i = 14; i > 0; --i) {
      dmonitor_cents[mon_dev_idx][i] = dmonitor_cents[mon_dev_idx][i - 1];
      dmonitor_kwh[mon_dev_idx][i] = dmonitor_kwh[mon_dev_idx][i - 1];
    }

    xwork = avg = (double)interval.kwh / 10000;

    // adding new history to top
    dmonitor_kwh[mon_dev_idx][0] = xwork;
    xwork *= ((double)(interval.cost_kwh) / 1000.0);
    i = (int)(xwork + .5);
    xwork = i;
    dmonitor_cents[mon_dev_idx][0] = (int)xwork;
    if ((++dmonitor_count[mon_dev_idx]) > 13) //only save 15 30-second intervals (7.5 minutes)
    {
      dmonitor_count[mon_dev_idx] = 14;
    }

    lwork = interval.kwh;
    lwork /= 120;
    device_history[mon_dev_idx].current_day[interval.hour] += lwork;
    device_history[mon_dev_idx].mtd[interval.hour] += lwork;
    device_history[mon_dev_idx].ytd[interval.hour] += lwork;

    device_history[mon_dev_idx].current_day$[interval.hour] += interval.dollars;
    device_history[mon_dev_idx].mtd$[interval.hour] += interval.dollars;
    device_history[mon_dev_idx].ytd$[interval.hour] += interval.dollars;

    if ((in_7_min_monitor) && (emon_device_idx == dev)) {
      scroll_window_down(8, 2, 21, 79);
      area_clear(7 * 14, 7 * 14 + 13, 8, 79 * 8, FG_WHT);
      sprintf(work, "%7.1f", avg);
      position(8, 2);
      text(work + 2, BG_WHT + FG_BLU);
      row = 7 * 14 + 3;
      col = 6 * 8 + 4;
      i = (int)(avg * 7 + .5); // times 7 to make bar longer
      avg = i;
      ecol = col + (int)(avg * 5);
      if (ecol > 631) {
        ecol = 631; /* sanity check */
      }
      if (ecol < col) {
        ecol = 631; /* sanity check */
      }
      hline(row + 0, col, ecol, FG_YEL);
      hline(row + 1, col, ecol, FG_YEL);
      hline(row + 2, col, ecol, FG_YEL);
      hline(row + 3, col, ecol, FG_YEL);
      hline(row + 4, col, ecol, FG_YEL);
      hline(row + 5, col, ecol, FG_YEL);
    } else if (in_device_detail == dev + 1) {
      update_device_detail(dev);
    }
  }
  return (0);
}

void ViewUpdated(char* update_name) {
  FILE* stream;

  CEBUS__ClosePort();

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
    fprintf(stream, "%0.2d/%0.2d/%0.2d\t%0.2d:%0.2d:%0.2d\tPROGRAM UPDATED\n",
            this_month, today, this_year,
            tod->tm_hour, tod->tm_min, tod->tm_sec);
    fclose(stream);
  } else {
    printf("Error opening user.log");
  }
  spawnl(_P_OVERLAY, update_name, NULL);
  exit(0);
}

extern char AM_MSG[];

LWINDOW_T read_devicesWindow[1] = {0, 0, 640, 480, //window position
                                   NULL,           //screen object
                                   NULL,           //draw
                                   0,              //item count
                                   0,              //cursor position 0-79 left to right
                                   0,              //element that has current focus
                                   fcsBtn,         //focus type
                                   longversion,
                                   NULL, //OnKeyPress();
                                   0};

LWINDOW_T progWindow[1] = {290, 160, 310, 115, //window position
                           NULL,               //screen object
                           NULL,
                           0,                           //item count
                           0,                           //cursor position 0-79 left to right
                           0,                           //element that has current focus
                           fcsBtn,                      //focus type
                           "Accessing ICU Information", // title
                           NULL,                        //OnKeyPress();
                           0};

UCHAR lw_ver[8];

//these are used as the date_time stamps of files from local PC
time_t device_chg_pc;
time_t event_chg_pc;
time_t rate_chg_pc;
time_t mood_chg_pc;
time_t trigger_chg_pc;
time_t security_chg_pc;
time_t billing_chg_pc;

//these are used as the date_time stamps of files from ICU
time_t device_chg_icu;
time_t event_chg_icu;
time_t rate_chg_icu;
time_t mood_chg_icu;
time_t trigger_chg_icu;
time_t security_chg_icu;
time_t billing_chg_icu;

//these are used for reply times from ICU when changing devices, events, etc...
time_t get_reply_time;

int read_devices(int days_back) {
  //    int     i;
  int result;
  int device_version;
  int no_bad;
  int mon;
  UCHAR receive_buffer[26];
  UCHAR transmit_buffer[3];
  FILE* f;
  struct tm outside_tm1;
  struct tm outside_tm2;
  SLONG sec1;
  SLONG sec2;
  int icu_busy;
  int i;
  long outsidetime;
  USHORT devices_used_local;
  struct _stat statbuf;
  DIR_TABLE* dt;
  char lw_versafe[10];
  //   char    tmp[100];

  RELOAD = 0;
  glbWindow = read_devicesWindow;
  PutWindow(glbWindow);

  memset(receive_buffer, 0, 26);
  memset(lw_versafe, 0, 10);

  //display logo and copyright information
  copyright();

  Convert2Release160();

  glbWindow = progWindow;
  PutWindow(glbWindow);
  glbWindow = NULL;

  Busy();

  ReadDevicesFromDisk();
  if (DEMO == TRUE) {
    NEW_MAIL = TRUE;
    NotBusy();
    return SUCCESS;
  }
  // Read Version table
  ptext("Reading Version", 205 + 115, 175, BG_WHT + FG_BLK);

  memset(receive_buffer, 0, 26);
  if ((icu_busy = GetTable(&receive_buffer,
                           16,
                           VERSION_TABLE,
                           version,
                           5,
                           RETRY_COUNT,
                           TRUE)) != SUCCESS) {
    switch (icu_busy) {
    case RT_ICU_BUSY:
      NotBusy();
      if (in_phone_view == TRUE) {
        msgbox("A LaneView session|"
               "already active.    ",
               "ICU BUSY", MB_OK);
      } else {
        msgbox("A LaneView Remote     |"
               "session already active.",
               "ICU BUSY", MB_OK);
      }
      exit_no = 1;
      lv_stop();
      break;
    case RT_BAD_PWD:
      NotBusy();
      msgbox("Invalid Entry. |"
             "Try again later.",
             "Error", MB_OK);
      exit_no = 1;
      lv_stop();
      break;
    }
    NotBusy();
    return FAILURE;
  }
  memcpy(&device_version, &receive_buffer[0], sizeof(USHORT));
  memcpy(&devices_used_local, &receive_buffer[2], sizeof(USHORT));
  memcpy(&events_used, &receive_buffer[4], sizeof(USHORT));
  memcpy(&moods_used, &receive_buffer[6], sizeof(USHORT));
  memcpy(&active_moods, &receive_buffer[8], sizeof(USHORT));
  memcpy(&triggers_used, &receive_buffer[10], sizeof(USHORT));
  memcpy(lw_ver, &receive_buffer[12], 4 * sizeof(UCHAR));
  if (lw_ver[0] == '\0') {
    sprintf(lw_ver, "Unk");
  }

  memcpy(lw_versafe, lw_ver, 4);
  lw_versafe[1] = '.';
  if (atof(lw_versafe) >= 4.12) {
    if (in_phone_view) {
      if (GetTable(&f_buffer,
                   2048, // size is variable length
                   GET_DIR_TABLE,
                   "FVUPDATE.EXE", //"FONEVIEW.EXE",
                   12,
                   RETRY_COUNT,
                   TRUE) == SUCCESS) {
        dt = (void*)f_buffer;
        if ((dt->dir_size - 6) / (sizeof(ENTRY))) {
          result = msgbox("A new Version of LaneView Remote  |"
                          "is available for download.  This  |"
                          "will take 15 to 20 minutes.  If   |"
                          "you want to do this now click YES.",
                          "Great News!", MB_YES_NO);
          if (result == MB_RESULT_YES) {
            area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
            ptext("Downloading LaneView Remote.", 205 + 115, 175, BG_WHT + FG_BLK);
            ptext("Press ESC to abort.", 205 + 115 + 14, 175, BG_WHT + FG_BLK);
            if (!read_file("FVUPDATE.EXE")) {
              DeleteFile("FVUPDATE.EXE");
#if 0
                     area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
                     ptext("Updating InterLane Resource Files.", 205 + 115, 175, BG_WHT + FG_BLK);
                     read_file("VIEW.ILR");
                     if (GetTable(&f_buffer,
                        2048,               // size is variable length
                        GET_DIR_TABLE,
                        "LANEVIEW.EXE",
                        11,
                        RETRY_COUNT,
                        TRUE)
                        == SUCCESS)
                     {
                        dt = (void*)f_buffer;
                        if (!((dt->dir_size - 6) / (sizeof(ENTRY))))
                        {
                           DeleteFile("VIEW.ILR");
                        }
                     }
#endif
              msgbox("Your LaneView Remote software  |"
                     "has just been received.  Answer|"
                     "'Y' (yes) to the following     |"
                     "question, and the final step   |"
                     "will be completed.             ",
                     "Great News!", MB_OK);
              ViewUpdated("FVUPDATE.EXE");
              lv_stop();
            }
          }
        }
      }
    } else {
      if (GetTable(&f_buffer,
                   2048, // size is variable length
                   GET_DIR_TABLE,
                   "LVUPDATE.EXE",
                   12,
                   RETRY_COUNT,
                   TRUE) == SUCCESS) {
        dt = (void*)f_buffer;
        if ((dt->dir_size - 6) / (sizeof(ENTRY))) {
          result = msgbox("A new Version of LaneView is         |"
                          "available for download.  This will   |"
                          "take 40 to 60 minutes.  If you want  |"
                          "to do this now click YES.            ",
                          "Great News!", MB_YES_NO);
          if (result == MB_RESULT_YES) {
            area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
            ptext("Updating LaneView.", 205 + 115, 175, BG_WHT + FG_BLK);
            ptext("Press ESC to abort.", 205 + 115 + 14, 175, BG_WHT + FG_BLK);
            if (!read_file("LVUPDATE.EXE")) {
              DeleteFile("LVUPDATE.EXE");
#if 0
                     area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
                     ptext("Updating InterLane Resource Files.", 205 + 115, 175, BG_WHT + FG_BLK);
                     ptext("Press ESC to abort.", 205 + 115 + 14, 175, BG_WHT + FG_BLK);
                     DeleteFile("LANEVIEW.EXE");
                     read_file("VIEW.ILR");
                     if (GetTable(&f_buffer,
                        2048,               // size is variable length
                        GET_DIR_TABLE,
                        "FONEVIEW.EXE",
                        11,
                        RETRY_COUNT,
                        TRUE)
                        == SUCCESS)
                     {
                        dt = (void*)f_buffer;
                        if (!((dt->dir_size - 6) / (sizeof(ENTRY))))
                        {
                           DeleteFile("VIEW.ILR");
                        }
                     }
                     msgbox("Your LaneView software has  |"
                        "just been upgraded.         |"
                        "Please restart LaneView.    ", "Good News!", MB_OK);
#endif
              //                     msgbox(   "Your LaneView software has  |"
              //                           "just been upgraded.         |"
              //                           "Please restart LaneView.    ", "Good News!", MB_OK);
              msgbox("Your LaneView software has  |"
                     "just been received.  Answer |"
                     "'Y' (yes) to the following  |"
                     "question, and the final step|"
                     "will be completed.          ",
                     "Great News!", MB_OK);
              ViewUpdated("LVUPDATE.EXE");
              lv_stop();
            }
          }
        }
      }
      if (GetTable(&f_buffer,
                   2048, // size is variable length
                   GET_DIR_TABLE,
                   "UTUPDATE.EXE", //"FONEVIEW.EXE",
                   12,
                   RETRY_COUNT,
                   TRUE) == SUCCESS) {
        dt = (void*)f_buffer;
        if ((dt->dir_size - 6) / (sizeof(ENTRY))) {
          result = msgbox("A new Version of Utilities        |"
                          "is available for download.  This  |"
                          "will take 20 to 30 minutes.  If   |"
                          "you want to do this now click YES.",
                          "Great News!", MB_YES_NO);
          if (result == MB_RESULT_YES) {
            area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
            ptext("Downloading new Utility programs.", 205 + 115, 175, BG_WHT + FG_BLK);
            ptext("Press ESC to abort.", 205 + 115 + 14, 175, BG_WHT + FG_BLK);
            if (!read_file("UTUPDATE.EXE")) {
              DeleteFile("UTUPDATE.EXE");
              msgbox("Your utility software programs |"
                     "have just been received. Answer|"
                     "'Y' (yes) to the following     |"
                     "question, and the final step   |"
                     "will be completed.             ",
                     "Great News!", MB_OK);
              ViewUpdated("UTUPDATE.EXE");
              lv_stop();
            }
          }
        }
      }
    }
    //get email
    area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
    ptext("Checking E-Mail.", 205 + 115, 175, BG_WHT + FG_BLK);

    if ((NEW_MAIL = CheckEmail()) == FALSE) {
      Status("No new mail.");
    } else {
      Status("You have new mail.");
    }
  }

  if (device_version != DEVICES_VERSION) {
    NotBusy();
    sprintf(buffer, "Incompatable Versions      |"
                    "|"
                    "LaneWare: %2d  LaneView: %2d",
            (-1) * device_version, (-1) * DEVICES_VERSION);
    msgbox(buffer, "Version Conflict", MB_OK);
    lv_stop();
  }

  if (in_phone_view) {
    unlink("FONEVIEW.BAK");
  } else {
    unlink("LANEVIEW.BAK");
  }

  outside_time(&outside_tm1);
  time(&ltime);
  outside_tm1.tm_isdst = _daylight;
  outsidetime = mktime(&outside_tm1);
  time_delta = outsidetime - ltime;

  if (devices_used_local == 0) {
    if (AskToReload() != 0) {
      NotBusy();
      lv_stop();
    }
  }

  // Read Customer table.
  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Reading Customer Information", 205 + 115, 175, BG_WHT + FG_BLK);
  if (GetTable(&cust_info,
               sizeof(CUSTOMER),
               CUSTOMER_TABLE,
               NULL,
               0,
               RETRY_COUNT,
               TRUE) != SUCCESS) {
    NotBusy();
    return FAILURE;
  }
  for (i = 0; i < 8; i++) {
    cust_info.password[i] &= 0x1F;
  }
  TableToDisk(&cust_info,
              sizeof(CUSTOMER),
              1,
              "custinfo.dat");

  memset(receive_buffer, 0, 26);
  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Checking Dates...", 205 + 115, 175, BG_WHT + FG_BLK);
  if (GetTable(&receive_buffer,
               24, //24,  change to 28 to support check for billing.prg check
               UPDATE_TIMES_TABLE,
               NULL,
               0,
               RETRY_COUNT,
               TRUE) != SUCCESS) {
    NotBusy();
    msgbox("Timed out reading dates from ICU.  |"
           "Unable to get device table from ICU.",
           "Warning", MB_OK);
    return FAILURE;
  }
  if (RELOAD == 0) {
    memcpy(&device_chg_icu, &receive_buffer[0], sizeof(time_t));
    memcpy(&event_chg_icu, &receive_buffer[4], sizeof(time_t));
    memcpy(&rate_chg_icu, &receive_buffer[8], sizeof(time_t));
    memcpy(&mood_chg_icu, &receive_buffer[12], sizeof(time_t));
    memcpy(&trigger_chg_icu, &receive_buffer[16], sizeof(time_t));
    memcpy(&security_chg_icu, &receive_buffer[20], sizeof(time_t));
  } else {
    device_chg_pc = device_chg_icu + 100;
    event_chg_pc = event_chg_icu + 100;
    rate_chg_pc = rate_chg_icu + 100;
    mood_chg_pc = mood_chg_icu + 100;
    trigger_chg_pc = trigger_chg_icu + 100;
    security_chg_pc = security_chg_icu + 100;
  }

  if (GetTable(&f_buffer,
               2048, // size is variable length
               GET_DIR_TABLE,
               "BILLING.PRG",
               11,
               RETRY_COUNT,
               TRUE) == SUCCESS) {
    if (_stat("BILLING.PRG", &statbuf) == 0) {
      if (verify_file_date("BILLING.PRG", &statbuf) == 1) {
        area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
        ptext("Reading Billing.Prg...", 205 + 115, 175, BG_WHT + FG_BLK);
        read_file("BILLING.PRG");
      }
    } else {
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Reading Billing.Prg...", 205 + 115, 175, BG_WHT + FG_BLK);
      read_file("BILLING.PRG");
    }
  }

  if (device_chg_pc < device_chg_icu) {
    // Read Device table
    if (devices_used_local > 0) {
      dev_list.devices_used = devices_used_local;
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Reading Devices...", 205 + 115, 175, BG_WHT + FG_BLK);
      if (GetTable(&dev_list.devices,
                   dev_list.devices_used * sizeof(DEVICE),
                   DEVICE_TABLE,
                   NULL,
                   0,
                   RETRY_COUNT,
                   TRUE) != SUCCESS) {
        NotBusy();
        msgbox("Timed out reading devices from ICU.|"
               "Unable to get device table from ICU.",
               "Warning", MB_OK);
        return FAILURE;
      }
    }
  }

  if (event_chg_pc < event_chg_icu) {
    // Read Event table.
    if (events_used > 0) {
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Reading Events...", 205 + 115, 175, BG_WHT + FG_BLK);
      if (GetTable(&events,
                   events_used * sizeof(EVENT),
                   EVENT_TABLE,
                   NULL,
                   0,
                   RETRY_COUNT,
                   TRUE) != SUCCESS) {
        NotBusy();
        msgbox("Timed out reading events from ICU.|"
               "Unable to get event table from ICU.",
               "Warning", MB_OK);
        return FAILURE;
      }
    }
  }

  if (mood_chg_pc < mood_chg_icu) {
    //read mood table
    if (moods_used > 0) {
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Reading Moods...", 205 + 115, 175, BG_WHT + FG_BLK);
      if (GetTable(&mood_list,
                   moods_used * sizeof(MOOD_T),
                   EXT_MOOD_TABLE,
                   NULL,
                   0,
                   RETRY_COUNT,
                   TRUE) != SUCCESS) {
        NotBusy();
        msgbox("Timed out reading moods from ICU.|"
               "Unable to get mood table from ICU.",
               "Warning", MB_OK);
        return FAILURE;
      }
    }
  }

  if (trigger_chg_pc < trigger_chg_icu) {
    if (triggers_used > 0) {
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Reading Triggers...", 205 + 115, 175, BG_WHT + FG_BLK);
      if (GetTable(&triggers,
                   triggers_used * sizeof(TRIGGER_T),
                   TRIGGER_TABLE,
                   NULL,
                   0,
                   RETRY_COUNT,
                   TRUE) != SUCCESS) {
        NotBusy();
        msgbox("Timed out reading triggers from ICU.|"
               "Unable to get trigger table from ICU.",
               "Warning", MB_OK);
        return FAILURE;
      }
    }
  }

  if (security_chg_pc < security_chg_icu) {
    if (GetTable(&security_device,
                 sizeof(SECURITY_T),
                 SECURITY_TABLE,
                 NULL,
                 0,
                 RETRY_COUNT,
                 TRUE) != SUCCESS) {
      msgbox("Unable to get Security Device from ICU.", "Warning", MB_OK);
    }
  }

  // Backup user device and event information
  if (user_backup() == FAILURE) {
    NotBusy();
    msgbox("Data could not be saved.|"
           "Check disk space.        ",
           "Fatal Error", MB_OK);
    lv_stop();
  }

  // When logged in LITE mode a button on the system setup screen
  // will allow a user to "log in now" or to collect history.
  // "log in now" will set the LVLITE flag to FALSE and call read_devices()
  if (LVLITE == TRUE) {
    time(&ltime);
    tod = localtime(&ltime);
    result = read_rates((UCHAR)(tod->tm_mon), (UCHAR)(tod->tm_mday),
                        (UCHAR)tod->tm_year, &todays_rates);
    NotBusy();
    return SUCCESS;
  }

  // Read Rate Table Dates.
  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Checking for rate updates...", 205 + 115, 175, BG_WHT + FG_BLK);
  if (GetTable(&f_buffer,
               2048, // size is variable length
               GET_DIR_TABLE,
               "RT*",
               3,
               RETRY_COUNT,
               TRUE) != SUCCESS) {
    NotBusy();
    msgbox("Could not get new rate files.", "Error", MB_OK);
    return FAILURE;
  }
  no_bad = CheckRateTableTimes();

  if (no_bad & 2) // last month's rates are old
  {
    // Read Rate table.
    area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
    ptext("Reading Rate Files...", 205 + 115, 175, BG_WHT + FG_BLK);
    transmit_buffer[0] = (UCHAR)tod->tm_year;
    transmit_buffer[1] = (UCHAR)(tod->tm_mon); // last month
    transmit_buffer[2] = 1;
    if (transmit_buffer[1] == 0) {
      transmit_buffer[1] = 12;
      --transmit_buffer[0];
    }

    result = read_rates(transmit_buffer[1], transmit_buffer[2],
                        transmit_buffer[0], &todays_rates);
    if (result == 1) {
      if (transmit_buffer[0] < 100) // Y2K Compliance req'd 2 formats
      {
        sprintf(buffer,
                "Error %1d encountered while|"
                "reading file RT%02d%02d     ",
                result, transmit_buffer[1], transmit_buffer[0]);
      } else { // Y2K Compliance req'd 2 formats
        sprintf(buffer,
                "Error %1d encountered while|"
                "reading file RT%02d%04d     ",
                result, transmit_buffer[1], transmit_buffer[0] + 1900);
      }
      msgbox(buffer, "Error", MB_OK);
      //return FAILURE;
    } else if (result == 5) {
      if (transmit_buffer[0] < 100) {
        sprintf(buffer, "File not found on ICU.|"
                        "RT%02d%02d                 ",
                transmit_buffer[1], transmit_buffer[0]);
      } else {
        sprintf(buffer, "File not found on ICU.|"
                        "RT%02d%04d                 ",
                transmit_buffer[1], transmit_buffer[0] + 1900);
      }
      msgbox(buffer, "Error", MB_OK);
    }
  }

  // Read Rate table.
  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Reading Rate Files...", 205 + 115, 175, BG_WHT + FG_BLK);
  transmit_buffer[0] = (UCHAR)tod->tm_year;
  transmit_buffer[1] = (UCHAR)(tod->tm_mon + 1);
  transmit_buffer[2] = (UCHAR)tod->tm_mday;

  result = read_rates(tod->tm_mon + 1, tod->tm_mday, tod->tm_year, &todays_rates);
  if (result == 1) {
    if (transmit_buffer[0] < 100) {
      sprintf(buffer,
              "Error %1d encountered |"
              "while reading file RT%02d%02d",
              result, transmit_buffer[1], transmit_buffer[0]);
    } else {
      sprintf(buffer,
              "Error %1d encountered |"
              "while reading file RT%02d%04d",
              result, transmit_buffer[1], transmit_buffer[0] + 1900);
    }
    NotBusy();
    msgbox(buffer, "Error", MB_OK);
    return FAILURE;
  } else if (result == 5) {
    if (transmit_buffer[0] < 100) {
      sprintf(buffer, "File not found on ICU.|"
                      "RT%02d%02d                 ",
              transmit_buffer[1], transmit_buffer[0]);
    } else {
      sprintf(buffer, "File not found on ICU.|"
                      "RT%02d%04d                 ",
              transmit_buffer[1], transmit_buffer[0] + 1900);
    }
    msgbox(buffer, "Error", MB_OK);
  }

  // Read History information for Whole House and monitored devices.
  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Reading Interval History...", 205 + 115, 175, BG_WHT + FG_BLK);

  outside_time(&outside_tm1);

  UpdateHistoryFast(days_back);

  mon_devices_used = 0;
  if (read_device_history(TRUE) == FAILURE) {
    NotBusy();
    return FAILURE;
  }
  // Read Meter table.
  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Reading Whole House Summary...", 205 + 115, 175, BG_WHT + FG_BLK);
  if (FetchTable(f_buffer, // &meter,
                 sizeof(meter),
                 METER_TABLE,
                 NULL,
                 0,
                 RETRY_COUNT,
                 TRUE) != 0) {
    NotBusy();
    msgbox("Timed out reading whole house |"
           "summary from ICU.             ",
           "Warning", MB_OK);
    return FAILURE;
  } else {
    decompress((void*)&meter, f_buffer, sizeof(METER));
  }

  time(&ltime);
  tod = localtime(&ltime);
  f = fopen("lastlog.on", "wb");
  mon = tod->tm_mon + 1;
  fwrite(&mon, 1, sizeof(int), f);
  fwrite(&tod->tm_mday, 1, sizeof(int), f);
  fwrite(&tod->tm_year, 1, sizeof(int), f);
  fclose(f);

  // What this next bit is about:
  // We don't want an interval to occur AFTER reading today's history
  // and BEFORE we log in... BECAUSE this would result in loosing a
  // "15 minute history" record.
  //  Therefore, we do this....
  outside_time(&outside_tm2);
  sec1 = (outside_tm1.tm_hour * 60L * 60L) + (outside_tm1.tm_min * 60L) + outside_tm1.tm_sec;
  sec2 = (outside_tm2.tm_hour * 60L * 60L) + (outside_tm2.tm_min * 60L) + outside_tm2.tm_sec;
  if ((sec2 / 900L) != (sec1 / 900L)) // if it is a new interval.. re-read today's history
  {
    area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
    ptext("Reading Interval History...", 205 + 115, 175, BG_WHT + FG_BLK);
    // make SURE that there's a new interval recorded...
    ticks_set(1, 5L * 19L);
    Status("Waiting 5 seconds....");
    while (1) // wait 5 seconds.
    {
      if (ticks_check(1) != 0) {
        break;
      }
    }
    Status("");
    UpdateHistoryFast(-1);
  }
  if (log_on() == FAILURE) {
    NotBusy();
    msgbox("LaneView was unable to connect to ICU.", "Login Error", MB_OK);
    lv_stop();
  }
  NotBusy();
  return SUCCESS;
}

read_device_history(int display_line) {
  char work[80];
  int i;
  int result;

  if (display_line = TRUE) {
    area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
    ptext("Reading Device Summary...", 205 + 115, 175, BG_WHT + FG_BLK);
  }

  for (i = 0; i < dev_list.devices_used; ++i) {
    if (IsSubmeter(&dev_list.devices[i])) {
      if (dev_list.devices[i].history_bucket < 1 || dev_list.devices[i].history_bucket > 10) {
        sprintf(work, "Invalid device history bucket: %d", dev_list.devices[i].history_bucket);
        ExceptionLog(work);
        msgbox(work, "Warning", MB_OK);
        return FAILURE;
      }
      if (display_line == TRUE) {
        area_clear(219 + 115, 233 + 115, 175, 467, FG_WHT);
        sprintf(work, "Updating %.13s", dev_list.devices[i].desc);
        trim(work);
        ptext(work, 219 + 115, 175, BG_WHT + FG_BLK);
      }
      //         result = GetTable(   &device_history[dev_list.devices[i].history_bucket-1],
      //                     sizeof(DEVICE_HISTORY),
      //                     DEVICE_HISTORY_TABLE,
      //                     &dev_list.devices[i].device_slot,
      //                     sizeof(USHORT),
      //                     RETRY_COUNT,
      //                     display_line);
      //         result = GetTable(f_buffer,
      //                     sizeof(DEVICE_HISTORY),
      //                     DEVICE_HISTORY_TABLE,
      //                     &dev_list.devices[i].device_slot,
      //                     sizeof(USHORT),
      //                     RETRY_COUNT,
      //                     display_line);
      result = FetchTable(f_buffer,
                          sizeof(DEVICE_HISTORY),
                          DEVICE_HISTORY_TABLE,
                          &dev_list.devices[i].device_slot,
                          sizeof(USHORT),
                          RETRY_COUNT,
                          display_line);
      if (result != 0) {
        return FAILURE;
      }
      decompress((void*)&device_history[dev_list.devices[i].history_bucket - 1],
                 f_buffer, sizeof(DEVICE_HISTORY));
      monitored_devices[mon_devices_used] = i;
      mon_devices_used++;
    }
  }
  return SUCCESS;
}

read_dawn_dusk_info(UCHAR year, UCHAR month, UCHAR day) {
  UCHAR transmit_buffer[3];

  transmit_buffer[0] = year;
  transmit_buffer[1] = month;
  transmit_buffer[2] = day;

  if (GetTable(&dawn_dusk_time,
               sizeof(DAWN_DUSK_TIME),
               DAWN_DUSK_TABLE,
               &transmit_buffer,
               sizeof(transmit_buffer),
               RETRY_COUNT,
               FALSE) != SUCCESS) {
    return 1;
  }
  return 0;
}

int reinitialize_cenode(void) {
  if (DEMO == TRUE) {
    return SUCCESS;
  }
  if (CEBUS__InitPort(cebusSetup.BasePort, cebusSetup.IRQ)) {
    ce_configuration.GROUP_ADDR_MASK = 0;
    ce_configuration.MODE = STANDARD_MODE;
    ce_configuration.MAX_RESTARTS = 127;
    ce_configuration.MAX_RETRIES = 0;
    CEBUS__InitDevice(cebusSetup.HouseCode, 1, &ce_configuration);
    cebus_found = TRUE;
    return SUCCESS;
  } else {
    cebus_found = FALSE;
    return FAILURE;
  }
}

void initialize_cenode(void) {
  FILE* stream;

  clear();
  if (DEMO == FALSE) {
    if ((stream = fopen("CEBUS.DAT", "rb")) == NULL) { /* file not found */
      msgbox("CEBUS.DAT not found!|"
             "Run ISETUP.EXE       ",
             "Error", MB_OK);
      ExceptionLog("Error reading file: CEBUS.DAT  -  File not found.");
      lv_stop();
    } else /* file was found */
    {
      if (fread((char*)&cebusSetup, 1, sizeof(struct CEBUSDATA), stream) != sizeof(struct CEBUSDATA)) { /* file read error */
        fclose(stream);
        unlink("CEBUS.DAT");
        msgbox("Error reading file: CEBUS.DAT.|"
               "Try again.                     ",
               "Error", MB_OK);
        ExceptionLog("Error reading file: CEBUS.DAT");
        lv_stop();
      }
      fclose(stream);
    }
    if (CEBUS__InitPort(cebusSetup.BasePort, cebusSetup.IRQ)) {
      ce_configuration.GROUP_ADDR_MASK = 0;
      ce_configuration.MODE = STANDARD_MODE;
      ce_configuration.MAX_RESTARTS = 127;
      ce_configuration.MAX_RETRIES = 0;
      CEBUS__InitDevice(cebusSetup.HouseCode, 1, &ce_configuration);
      cebus_found = TRUE;
    } else {
      msgbox("CEBus not initialized.    |"
             "See if it is plugged in or|"
             "check IRQ & Port settings. ",
             "CENODE ERROR", MB_OK);
      ExceptionLog("CEBus not initialized");
      cebus_found = FALSE;
      lv_stop();
    }
  } else {
    ce_configuration.GROUP_ADDR_MASK = 0;
    ce_configuration.MODE = STANDARD_MODE;
    ce_configuration.MAX_RESTARTS = 127;
    ce_configuration.MAX_RETRIES = 0;
    cebus_found = TRUE; //lie to *view
  }
}

extern int DEBUG;
extern ULONG reset_count;
void ce_bus_reset(void) {
  int retry;
  UCHAR tmp[40];

  if (DEMO == TRUE) {
    return;
  }
  in_ce_bus_reset = TRUE;

  if (reset_count++ > 60000) {
    reset_count = 0;
    ExceptionLog("CEBus Resets exceeded 60,000");
  }

  sprintf(tmp, "CEBus Reset: %u", reset_count);
  Status(tmp);
  if (DEBUG == TRUE) {
    sprintf(tmp, "CEBus Reset: %u - DEBUG MODE ON", reset_count);
    ExceptionLog(tmp);
    Status(tmp);
  } else if ((reset_count % 10) == 0) {
    sprintf(tmp, "CEBus Reset: %u", reset_count);
    ExceptionLog(tmp);
  }

  if (cebus_found == TRUE) {
    CEBUS__ClosePort();
    cebus_found = FALSE;
    Wait(4L);
  }

  //Wait(18L);
  for (retry = 0; retry < 5; retry++) {
    if (reinitialize_cenode() == SUCCESS) {
      in_ce_bus_reset = FALSE;
      Wait(18L);
      return;
    }
    sprintf(tmp, "Re-init CENode %2d", retry + 1);
    ExceptionLog(tmp);
    //Status(tmp);
  }
  msgbox("Unplug CENode transformer from wall,|"
         "plug it back in and restart LaneView.",
         "Error", MB_OK);
  ExceptionLog("CEBus RE-INIT FAILED");
  lv_stop();
  in_ce_bus_reset = FALSE;
}

void CheckCEBus(void) {
  int result;
  time_t ltime;
  PKT_INFO pkt_info;

  if ((DEMO == TRUE) || (cebus_found == FALSE)) {
    return;
  }
  while (result = CEBUS__GetPacket(&pkt_info)) {
    if (result == CERROR) {
      ce_bus_reset();
    } else {
      switch (PacketType(&pkt_info)) {
      case INTERVAL_30S:
        process_interval(&pkt_info);
        continue;
      case INTERVAL_30S$:
        process_interval$(&pkt_info);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&pkt_info.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }
    }
  }
  time(&ltime);

  if ((in_phone_view == TRUE) && (LVLITE == TRUE) && ticks_check(8)) {
    ticks_set(8, 810L);
    pkt_info.pkt.CONTROL = 9;
    pkt_info.pkt.SA = 1;
    pkt_info.pkt.SHC = cebusSetup.HouseCode;
    pkt_info.pkt.DA = 1;
    pkt_info.pkt.DHC = cebusSetup.OutsideHC;
    pkt_info.pkt.INFO[0] = 0x050;
    pkt_info.pkt.INFO[1] = 0x0E8; // plus sequence number
    pkt_info.pkt.INFO[2] = 0x051;
    pkt_info.length = 12;
    send_packet(&pkt_info);
  }

  if (difftime(ltime, sync_check) > 120) {
    if (synchronize() == FAILURE) {
      msgbox("Attempt to reconnect with ICU failed.|"
             "LaneView will now shut down.         |"
             "Unsaved data will be lost.            ",
             "Warning", MB_OK);
      lv_stop();
    } else {
      time(&sync_check);
    }
  }
}

int SendTable(int table_no, void* request_data, int data_length) {
  UCHAR size, count, len;
  UCHAR* cp;
  PKT_INFO pkt_info;
  USHORT ch_status;

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8; // plus sequence number
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;

  if (table_no == LOGGED_ON_TABLE) {
    pkt_info.pkt.INFO[4] = 0x045; // set value
  } else {
    pkt_info.pkt.INFO[4] = 0x046; // set array
  }

  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = table_no;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;

  count = 0;
  size = data_length;
  cp = request_data;

  if (data_length > 21) {
    len = 21;
  } else {
    len = data_length;
  }

  while (size) {
    if (data_length > 21) {
      pkt_info.pkt.INFO[2] = (UCHAR)count++;
    }

    memcpy(&pkt_info.pkt.INFO[11], cp, len);
    pkt_info.length = 20 + len;

    if (send_packet(&pkt_info) == FAILURE) {
      return FAILURE;
    }

    cp += len;
    size -= len;

    if (len > size) {
      len = size;
    }
    if (size < 0) {
      size = 0; // so we'll break out of loop
    }
  }
  if (get_reply(&ch_status) == SUCCESS) {
    return SUCCESS;
  } else {
    return FAILURE;
  }
}

#define LDEL_DEV_INFO 12
UCHAR DEL_DEV_INFO[LDEL_DEV_INFO] = {
    0x050, 0x0E8, 0x050,
    0x002, 0x046, 0x0F4,
    0x032, 0x0F6, DEVICE_TABLE,
    0x020, 0x0F5, 0x044};
int RemoveDevice(DEVICE* r_dev) {
  int i;
  int which;
  int times;
  int mm_pri;
  USHORT return_code;
  PKT_INFO pkt_info;

  memset(&pkt_info, 0, sizeof(PKT_INFO));
  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;

  memcpy(&pkt_info.pkt.INFO[0], DEL_DEV_INFO, LDEL_DEV_INFO);
  memcpy(&pkt_info.pkt.INFO[12], &r_dev->device_slot, sizeof(USHORT));

  pkt_info.length = 9 + 2 + LDEL_DEV_INFO;

  if (r_dev->priority > 0) {
    if (IsSubmeter(r_dev) == FALSE) { //search for attached sub meter device
      for (i = 0; i < dev_list.devices_used; i++) {
        if ((IsSubmeter(&dev_list.devices[i]) == TRUE) &&
            (r_dev->monitor_hc == dev_list.devices[i].cebus_hc) &&
            (r_dev->monitor_subunit == dev_list.devices[i].cebus_subunit) &&
            (r_dev->monitor_unit == dev_list.devices[i].cebus_unit)) {
          mm_pri = dev_list.devices[i].priority;
          dev_list.devices[i].priority = 0;
          if (SendObject(&dev_list.devices[i], 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
            msgbox("Unable to change corresponding|"
                   "Sub Meter device's priority.  |"
                   "Try again.                     ",
                   "Warning",
                   MB_OK);
            dev_list.devices[i].priority = mm_pri;
            return FAILURE;
          }
          break;
        }
      }
    } else { //search for attached non-sub meter device
      for (i = 0; i < dev_list.devices_used; i++) {
        if ((IsSubmeter(&dev_list.devices[i]) == FALSE) &&
            (dev_list.devices[i].monitor_hc == r_dev->cebus_hc) &&
            (dev_list.devices[i].monitor_subunit == r_dev->cebus_subunit) &&
            (dev_list.devices[i].monitor_unit == r_dev->cebus_unit)) {
          mm_pri = dev_list.devices[i].priority;
          dev_list.devices[i].priority = 0;
          if (SendObject(&dev_list.devices[i], 'C', sizeof(DEVICE), DEVICE_TABLE) == FAILURE) {
            msgbox("Unable to change corresponding|"
                   "Sub Meter device's priority.  |"
                   "Try again.                     ",
                   "Warning",
                   MB_OK);
            dev_list.devices[i].priority = mm_pri;
            return FAILURE;
          }
          break;
        }
      }
    }
  }
  times = 0;
  while (1) {
    if (send_packet(&pkt_info) == SUCCESS) {
      if (get_reply(&return_code) == SUCCESS) {
        AdjustEvents(r_dev->device_slot);
        AdjustMoods(r_dev->device_slot);
        which = LookupDev(r_dev->device_slot);
        for (i = which; i < dev_list.devices_used; ++i) {
          dev_list.devices[i] = dev_list.devices[i + 1];
        }
        --dev_list.devices_used;
        memset(&dev_list.devices[dev_list.devices_used], 0, sizeof(DEVICE));
        return user_backup();
      }
    }
    if (times++ > RETRY_COUNT) {
      sprintf(buffer, "Unable to delete device!|"
                      "Error Code = %6d            ",
              return_code);
      ExceptionLog(buffer);
      msgbox(buffer, "Information", MB_OK);
      return FAILURE;
    }
  }
}

#define LDEL_MOOD_INFO 12
UCHAR DEL_MOOD_INFO[LDEL_MOOD_INFO] = {
    0x050, 0x0E8, 0x050,
    0x002, 0x046, 0x0F4,
    0x032, 0x0F6, EXT_MOOD_TABLE,
    0x020, 0x0F5, 0x044};
int RemoveMood(MOOD_T* r_mood) {
  int i, which;
  int times;
  USHORT return_code;
  PKT_INFO pkt_info;

  memset(&pkt_info, 0, sizeof(PKT_INFO));
  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;

  memcpy(&pkt_info.pkt.INFO[0], DEL_MOOD_INFO, LDEL_MOOD_INFO);
  memcpy(&pkt_info.pkt.INFO[12], &r_mood->mood_id, sizeof(USHORT));

  pkt_info.length = 9 + 2 + LDEL_MOOD_INFO;

  for (times = 0; times < RETRY_COUNT; times++) {
    if (send_packet(&pkt_info) == SUCCESS) {
      if (get_reply(&return_code) == SUCCESS) {
        which = LookupMoodIndex(r_mood->mood_id);
        for (i = which; i < moods_used; ++i) {
          mood_list[i] = mood_list[i + 1];
        }
        --moods_used;
        memset(&mood_list[moods_used], 0, sizeof(MOOD_T));
        return user_backup();
      }
    }
  }
  return FAILURE;
}

#define LDEL_EVENT_INFO 12
unsigned char DEL_EVENT_INFO[LDEL_EVENT_INFO] =
    {
        0x050, 0x0E8, 0x050,
        0x002, 0x046, 0x0F4,
        0x032, 0x0F6, EVENT_TABLE,
        0x020, 0x0F5, 0x044};

int RemoveEvent(EVENT* event) {
  int i;
  int times;
  int result;
  int event_index;
  PKT_INFO pkt_info;
  USHORT event_id;

  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  memcpy(&pkt_info.pkt.INFO[0], DEL_EVENT_INFO, LDEL_EVENT_INFO);
  memcpy(&pkt_info.pkt.INFO[13], &event->event_id, 2);
  pkt_info.length = 9 + 2 + LDEL_EVENT_INFO + 1;

  times = 0;
  while (1) {
    result = send_packet(&pkt_info);
    if (result == SUCCESS) {
      result = get_reply(&event_id);
      if (result == SUCCESS) {
        user_backup();
        break;
      }
    }
    if (times++ > RETRY_COUNT) {
      return FAILURE;
    }
  }
  event_index = LookupEvent(event->event_id);
  for (i = event_index; i < events_used; ++i) {
    events[i] = events[i + 1];
  }
  --events_used;
  memset(&events[events_used], 0, sizeof(EVENT));
  return user_backup();
}

int get_reply(USHORT* id) // works for add event, add device, maybe others
{
  int result;
  USHORT answer;
  PKT_INFO incoming_pkt;

  if (DEMO == TRUE) {
    return SUCCESS;
  }
  ticks_set(1, 91L); //ticks_set(1, 400L);                 /* set 200 tick alarm (11 seconds) */
  while (1) {
    result = CEBUS__GetPacket(&incoming_pkt);
    if (result == 0) {
      if (ticks_check(1)) {
        *id = 10;
        return FAILURE;
      }
      continue;
    } else if (result == CERROR) {
      ce_bus_reset();
      *id = 11;
      return FAILURE;
    } else {
      if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
        continue;
      }
      switch (PacketType(&incoming_pkt)) {
      case INTERVAL_30S:
        process_interval(&incoming_pkt);
        continue;
      case INTERVAL_30S$:
        process_interval$(&incoming_pkt);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }
      if (incoming_pkt.pkt.INFO[2] == 0x0FD) // error
      {
        answer = incoming_pkt.pkt.INFO[3];
        *id = answer;
        return FAILURE;
      }
      if (incoming_pkt.pkt.INFO[2] == 0x0FE) // successful update
      {
        answer = incoming_pkt.pkt.INFO[3] +
                 incoming_pkt.pkt.INFO[4] * 256;
        memcpy(&get_reply_time, &incoming_pkt.pkt.INFO[5], sizeof(time_t));
        *id = answer;
        return SUCCESS;
      }
    }
  }
}

get_reply2(USHORT* id1, UCHAR* id2) // works for add device, change device
{
  int result;
  USHORT answer1;
  UCHAR answer2;
  PKT_INFO incoming_pkt;

  if (DEMO == TRUE) {
    return SUCCESS;
  }
  ticks_set(1, 54L); /* set 200 tick alarm (11 seconds) */
  while (1) {
    result = CEBUS__GetPacket(&incoming_pkt);
    if (result == 0) {
      if (ticks_check(1)) {
        *id1 = 10;
        return FAILURE;
      }
      continue;
    } else if (result == CERROR) {
      ce_bus_reset();
      *id1 = 11;
      return FAILURE;
    } else {
      if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
        continue;
      }
      switch (PacketType(&incoming_pkt)) {
      case INTERVAL_30S:
        process_interval(&incoming_pkt);
        continue;
      case INTERVAL_30S$:
        process_interval$(&incoming_pkt);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }

      if (incoming_pkt.pkt.INFO[2] == 0x0FD) // error
      {
        answer1 = incoming_pkt.pkt.INFO[3];
        *id1 = answer1;
        *id2 = 0;
        return FAILURE;
      }
      if (incoming_pkt.pkt.INFO[2] == 0x0FE) // successful update
      {
        answer1 = incoming_pkt.pkt.INFO[3] +
                  incoming_pkt.pkt.INFO[4] * 256;
        *id1 = answer1;
        answer2 = incoming_pkt.pkt.INFO[5] +
                  incoming_pkt.pkt.INFO[6] * 256;
        *id2 = answer2;
        memcpy(&get_reply_time, &incoming_pkt.pkt.INFO[7], sizeof(time_t));
        //MARK:  device_chg_icu = time sent back from icu
        return SUCCESS;
      }
    }
  }
}

#define LCHANGE_MODE_INFO 11
unsigned char CHANGE_MODE_INFO[LCHANGE_MODE_INFO] = {
    0x050, 0x0E8, 0x050,
    0x002, 0x046, 0x0F4,
    0x032, 0x0F6, 0x018,
    0x020, 0x0F5};

int SetDayType(UCHAR new_day_type) {
  PKT_INFO pkt_info;
  int retry = 0;
  // send to outside box.
  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  memcpy(&pkt_info.pkt.INFO[0], CHANGE_MODE_INFO, LCHANGE_MODE_INFO);
  memcpy(&pkt_info.pkt.INFO[LCHANGE_MODE_INFO], &new_day_type, sizeof(UCHAR));
  pkt_info.length = 9 + sizeof(UCHAR) + LCHANGE_MODE_INFO;

  while (1) {
    if (send_packet(&pkt_info) == SUCCESS) {
      return SUCCESS;
    }
    if (retry++ > 5) {
      return FAILURE;
    }
  }
}

void SetTime(void) {
  union REGS regs;
  UCHAR date_time[6];

  if (GetTable(&date_time,
               sizeof(date_time),
               TIME_TABLE,
               NULL,
               0,
               RETRY_COUNT,
               FALSE) != SUCCESS) {
    msgbox("Unable to read time from ICU.",
           "Warning", MB_OK);
    ExceptionLog("Unable to read time from ICU.");
    return;
  }

  regs.h.ah = 0x02D;        /* set system time */
  regs.h.ch = date_time[3]; // hours
  regs.h.cl = date_time[4]; // minutes
  regs.h.dh = date_time[5]; // seconds
  regs.h.dl = 50;           // hundredths of second
  int86(0x021, &regs, &regs);

  regs.h.ah = 0x02B;               /* set system date */
  regs.h.dh = date_time[0];        // month
  regs.h.dl = date_time[1];        // day
  regs.x.cx = date_time[2] + 1900; // year
  int86(0x021, &regs, &regs);
  time(&sync_check);
}

int outside_time(struct tm* outside_tm) {
  UCHAR date_time[6];
  memset(date_time, 0, 6);

  if (GetTable(&date_time,
               sizeof(date_time),
               TIME_TABLE,
               NULL,
               0,
               RETRY_COUNT,
               FALSE) != SUCCESS) {
    msgbox("Unable to read time from ICU.",
           "Warning", MB_OK);
    return FAILURE;
  }
  outside_tm->tm_hour = date_time[3]; // hours
  outside_tm->tm_min = date_time[4];  // minutes
  outside_tm->tm_sec = date_time[5];  // seconds

  outside_tm->tm_mon = date_time[0] - 1; // month
  outside_tm->tm_mday = date_time[1];    // day
  outside_tm->tm_year = date_time[2];    // year
  time(&sync_check);
  return SUCCESS;
}

int log_on(void) {
  int times;
  int table, value;

  times = 0;
  table = LOGGED_ON_TABLE;
  value = 1;
  while (++times < 5) {
    if (SendTable(table, &value, 1) == SUCCESS) {
      time(&sync_check);
      LOGGED_IN = TRUE;
      return SUCCESS;
    }
  }
  return FAILURE;
}

int log_off(void) {
  int i;
  int table;
  int value;

  table = LOGGED_ON_TABLE;
  value = 0;

  for (i = 0; i < 5; i++) {
    if (SendTable(table, &value, 1) == SUCCESS) {
      LOGGED_IN = FALSE;
      return SUCCESS;
    }
  }
  return FAILURE;
}

int collect_intervals(USHORT device_id, int month, int day, int year,
                      HISTORY_DAY* history_day) {
  UCHAR transmit_buffer[5];

  transmit_buffer[0] = (UCHAR)year;
  transmit_buffer[1] = (UCHAR)month;
  transmit_buffer[2] = (UCHAR)day;
  memcpy(&transmit_buffer[3], &device_id, 2);

  //returns:
  //SUCCESS
  //FAILURE
  //-1 = history not available
  return GetTable(history_day,
                  sizeof(HISTORY_DAY),
                  HISTORY_TABLE,
                  &transmit_buffer,
                  sizeof(transmit_buffer),
                  RETRY_COUNT,
                  TRUE);
}

send_backup_files(void) {
  char* cp;
  int i, len, bytes, times;
  PKT_INFO out_going;
  USHORT index;
  short bkup_version, bkup_devices, bkup_events;

  //DevicesFromDisk();
  //EventsFromDisk();

  bkup_version = DEVICES_VERSION;
  bkup_devices = dev_list.devices_used;
  bkup_events = events_used;

  //   if (bkup_version != DEVICES_VERSION)
  //   {
  //      return(1);
  //   }

  out_going.pkt.CONTROL = 9;
  out_going.pkt.SA = 1;
  out_going.pkt.SHC = cebusSetup.HouseCode;
  out_going.pkt.DA = 1;
  out_going.pkt.DHC = cebusSetup.OutsideHC;
  out_going.pkt.INFO[0] = 0x050;
  out_going.pkt.INFO[1] = 0x0E8;
  out_going.pkt.INFO[2] = 0x050;
  out_going.pkt.INFO[3] = 0x002;
  out_going.pkt.INFO[4] = 0x046;
  out_going.pkt.INFO[5] = 0x0F4;
  out_going.pkt.INFO[6] = 0x032;
  out_going.pkt.INFO[7] = 0x0F6;
  out_going.pkt.INFO[9] = 0x020;
  out_going.pkt.INFO[10] = 0x0F5;
  out_going.pkt.INFO[8] = 1;
  memcpy(&out_going.pkt.INFO[11], &bkup_devices, 2);
  memcpy(&out_going.pkt.INFO[13], &bkup_events, 2);
  out_going.length = 24;

  // Send Version information.
  if (send_packet(&out_going) == FAILURE) {
    return (1);
  }

  Wait(18L);

  // Send Device information.
  out_going.pkt.INFO[8] = RESTORE_DEVICES_TABLE;
  times = 0;
  while (1) {
    cp = (void*)dev_list.devices;
    bytes = bkup_devices * sizeof(DEVICE);
    if (bytes > 19) {
      len = 19;
      index = bytes / 19;
      if ((bytes % 19) == 0)
        --index;
    } else {
      len = bytes;
      index = 0;
    }

    for (i = 0; i <= (int)index; ++i) {
      out_going.pkt.INFO[11] = i;
      memcpy(&out_going.pkt.INFO[12], cp, len);
      out_going.length = 9 + 12 + len;

      if (send_packet(&out_going) == FAILURE) {
        break;
      }
      cp += len;
      if (bytes < 19) {
        len = bytes;
      }
      bytes -= len;
      if (bytes == 0) {
        break;
      }
    }

    if (bytes == 0) {
      break;
    }
    if (times > RETRY_COUNT) {
      return (1);
    }
  }
  Wait(18L);
  // Send Event information.
  out_going.pkt.INFO[8] = RESTORE_EVENTS_TABLE;
  times = 0;
  while (1) {
    cp = (void*)events;
    bytes = bkup_events * sizeof(EVENT);
    if (bytes > 19) {
      len = 19;
      index = bytes / 19;
      if ((bytes % 19) == 0)
        --index;
    } else {
      len = bytes;
      index = 0;
    }

    for (i = 0; i <= (int)index; ++i) {
      out_going.pkt.INFO[11] = i;
      memcpy(&out_going.pkt.INFO[12], cp, len);
      out_going.length = 9 + 12 + len;

      if (send_packet(&out_going) == FAILURE) {
        break;
      }

      cp += len;
      if (bytes < 19)
        len = bytes;
      bytes -= len;
      if (bytes == 0)
        break;
    }

    if (bytes == 0) {
      break;
    }
    if (times > RETRY_COUNT) {
      return (1);
    }
  }
  return (0);
}

read_file(char* fname) {
  USHORT rc;
  USHORT fsize;
  USHORT tries;
  FILE* stream;
  char lw_versafe[10];

  memset(lw_versafe, 0, 10);

  tries = 0;
  while (1) {
    memcpy(lw_versafe, lw_ver, 4);
    lw_versafe[1] = '.';
    if (atof(lw_versafe) < 4.12) {
      if (request_file(fname) != SUCCESS) {
        if (++tries < 40) {
          continue;
        } else {
          return (1);
        }
      }
      rc = get_file(TRUE);
      if (rc) {
        if (rc == 5) {
          return (5); //file not found on ICU
        }
        if (rc == 2) {
        }
        if (++tries < 40) {
          continue;
        } else {
          return (1);
        }
      } else {
        stream = fopen(fname, "wb");
        if (stream != NULL) {
          memcpy(&fsize, f_buffer, 2);
          fwrite(&f_buffer[2], 1, fsize, stream);
          fclose(stream);
        }
        return (0);
      }
    } else {
      rc = FetchFile(fname);
      if (rc) {
        if (rc == 5) {
          return (5); //file not found on ICU
        }
        if (++tries < 40) {
          continue;
        } else {
          return (1);
        }
      } else {
        return (0);
      }
    }
  }
}

int request_file(UCHAR* fname) {
  PKT_INFO pkt_info;

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8 + (UCHAR)((++running_sequence) & 0x007); // plus sequence number
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;
  pkt_info.pkt.INFO[4] = 0x044;
  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = GET_FILE_TABLE;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;
  memcpy(&pkt_info.pkt.INFO[11], fname, 20);
  pkt_info.length = 20 + strlen(fname);
  return send_packet(&pkt_info);
}

// int DeleteFileName(UCHAR *fname)
// to be called from int DeleteFile(UCHAR *fname) only
int DeleteFileName(UCHAR* fname) {
  PKT_INFO pkt_info;
  int cebus_status;

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8 + (UCHAR)((++running_sequence) & 0x007); // plus sequence number
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;
  pkt_info.pkt.INFO[4] = 0x044;
  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = DELETE_FILE_TABLE;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;
  memcpy(&pkt_info.pkt.INFO[11], fname, 20);
  pkt_info.length = 20 + strlen(fname) + 1;
  if ((req_id = CEBUS__SendPacket(&pkt_info)) == 0L) {
    ce_bus_reset();
    req_id = CEBUS__SendPacket(&pkt_info);
  }
  ticks_set(5, 7L);
  while (1) {
    if (CEBUS__GetTransmitStatus(req_id, &(XMIT_STAT)cebus_status)) {
      switch (cebus_status) {
      case 3:                                            // failure
        if (req_id = CEBUS__SendPacket(&pkt_info) == 0L) //send queue full
        {
          ce_bus_reset(); //empty it
        }
        return (1);
      case 0: // pending
        if (ticks_check(5)) {
          return (2);
        }
        continue;
      case 1: // idle
        return (3);
      case 2: // success
        return (0);
      }
    } else {
      ce_bus_reset();
      return (1);
    }
  }
  return (0);
}

int DeleteFile(UCHAR* fname) {
  USHORT rc;
  USHORT tries;

  tries = 0;
  while (1) {
    if (DeleteFileName(fname)) {
      if (++tries < 10) {
        continue;
      } else {
        return (2);
      }
    } else {
      if (get_reply(&rc) <= 0) {
        return (rc);
      } else {
        if (++tries < 10) {
          continue;
        } else {
          return (rc);
        }
      }
    }
  }
}

get_file(int show_prog) {
  UCHAR* cp;
  USHORT len, file_size;
  SSHORT i, packet_count;
  int result;
  PKT_INFO incoming_pkt;
  PROGRESS_INDICATOR_T pi = {245 + 115, 220, 200, 0, FG_BLU, 0};

  if (DEMO == TRUE) {
    return SUCCESS;
  }

  if (show_prog == TRUE) {
    PutIndicator(&pi);
  }

  cp = f_buffer;
  len = 29;

  packet_count = 0;

  ticks_set(5, 100L); /* set alarm for first packet */
  while (1) {
    if (kbhit()) {
      if (kbchar() == ESC) {
        msgbox("Data transfer canceled by user.", "Information", MB_OK);
        lv_stop();
      }
    }
    result = CEBUS__GetPacket(&incoming_pkt);
    if (result == 0) {
      if (ticks_check(5)) {
        ce_bus_reset();
        return (1);
      }
      continue;
    } else if (result == CERROR) {
      ce_bus_reset();
      return (1);
    }

    ticks_set(5, 100L); /* reset alarm */

    switch (PacketType(&incoming_pkt)) {
    case INTERVAL_30S:
      process_interval(&incoming_pkt);
      continue;
    case INTERVAL_30S$:
      process_interval$(&incoming_pkt);
      continue;
    case INTERVAL_15M:
      process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
      continue;
    case ALERT_MSG:
      continue;
    case NEW_RATE_MSG:
      continue;
    }

    if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
      continue;
    }
    if ((USHORT)(incoming_pkt.pkt.INFO[1] & 0x007) != (running_sequence & 0x007)) {
      continue;
    }

    if ((incoming_pkt.pkt.INFO[2] == 0x0FD) &&
        ((incoming_pkt.pkt.INFO[1] & 0x0D8) == 0x0D0)) {
      if (incoming_pkt.pkt.INFO[3] == 1) {
        return 5; // file not found
      } else {
        return 2;
      }
    }

    if (incoming_pkt.pkt.INFO[2] & 0x080) {
      memcpy(&file_size, &incoming_pkt.pkt.INFO[3], 2);
      i = (file_size + 2) / 29;
      if (((file_size + 2) % 29) == 0) {
        --i;
      }
      packet_count = i;
      memset(cp, 0x0FF, file_size + 2);
    }

    if ((incoming_pkt.pkt.INFO[2] & 0x07F) != (packet_count & 0x07F)) {
      continue;
    } else {
      memcpy(cp, &incoming_pkt.pkt.INFO[3], len);
      cp += 29;
      if (show_prog == TRUE) {
        if ((packet_count % 4) == 0) {
          pi.progress = 100 - (int)((packet_count / ((file_size / 29) + .01)) * 100);
          UpdateIndicator(&pi);
        }
      }
      if (packet_count == 1) {
        len = (file_size + 2) % 29;
        if (len == 0) {
          len = 29;
        }
      }
      --packet_count;
      if (packet_count < 0) {
        break;
      }
    }
  }
  return (0);
}

int user_backup(void) {
  FILE* stream;
  int ver;
  struct utimbuf file_change_time;
  //int TableToDisk(void *o_buf, int o_size, int count, UCHAR *fname )

  if (TableToDisk(dev_list.devices, sizeof(DEVICE), dev_list.devices_used, "DEVICES.DAT") == FAILURE) {
    return FAILURE;
  }
  file_change_time.modtime = device_chg_icu + 1;
  utime("devices.dat", &file_change_time);

  if (TableToDisk(events, sizeof(EVENT), events_used, "EVENTS.DAT") == FAILURE) {
    return FAILURE;
  }
  file_change_time.modtime = event_chg_icu + 1;
  utime("events.dat", &file_change_time);

  if (TableToDisk(mood_list, sizeof(MOOD_T), moods_used, "MOODS.DAT") == FAILURE) {
    return FAILURE;
  }
  file_change_time.modtime = mood_chg_icu + 1;
  utime("moods.dat", &file_change_time);

  if (TableToDisk(triggers, sizeof(TRIGGER_T), triggers_used, "TRIGGERS.DAT") == FAILURE) {
    return FAILURE;
  }
  file_change_time.modtime = trigger_chg_icu + 1;
  utime("triggers.dat", &file_change_time);

  stream = fopen("SECURITY.DAT", "wb");
  if (stream != NULL) {
    ver = DEVICES_VERSION;
    fwrite(&ver, 2, 1, stream);
    fwrite(&security_device, sizeof(SECURITY_T), 1, stream);
    fclose(stream);
    file_change_time.modtime = security_chg_icu + 1;
    utime("security.dat", &file_change_time);
  } else {
    return FAILURE;
  }
  return SUCCESS;
}

int send_packet(PKT_INFO* packet) {
  int times, cebus_status;

  if (DEMO == TRUE) {
    return SUCCESS;
  }

  times = 0;
  while (times < RETRY_COUNT * 2) {
    if ((req_id = CEBUS__SendPacket(packet)) == 0L) {
      ce_bus_reset();
      ++times;
      continue;
    }
    ticks_set(1, 8L);
    while (1) {
      if (CEBUS__GetTransmitStatus(req_id, &(XMIT_STAT)cebus_status)) {
        switch (cebus_status) {
        case FAILURE: /* FAILURE */
          ++times;
          ce_bus_reset();
          break;
        case PENDING: /* Pending */
          if (ticks_check(1)) {
            ++times;
            ce_bus_reset();
            break;
            //return FAILURE;         // timed out
          }
          continue;
        case IDLE: // IDLE
          ce_bus_reset();
          break;
        case SUCCESS: // SUCCESS
          return SUCCESS;
        }
        break;
      } else // our packet was lost!!!
      {
        ce_bus_reset();
        ++times;
        continue;
      }
    }
    times++;
  }
}

static UCHAR* resync = {"Reconnecting with ICU"};
int synchronize(void) {
  //int     i;
  LWINDOW_T* tmpWindow;

  if (LVLITE == TRUE) {
    return SUCCESS;
  }

  tmpWindow = glbWindow;
  glbWindow = progWindow;
  progWindow[0].title = resync;
  PutWindow(glbWindow);

  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Reading Interval History", 205 + 115, 175, BG_WHT + FG_BLK);

#if 0
   if (update_history(0xFFFF, 9))
   {
      return FAILURE;
   }
   for (i = 0; i < dev_list.devices_used; ++i)
   {
      if (IsSubmeter(&dev_list.devices[i]))
      {
         if (update_history(dev_list.devices[i].device_slot, 9))
         {
            return FAILURE;
         }
      }
   }
#else
  UpdateHistoryFast(-1);
#endif

  mon_devices_used = 0;
  if (read_device_history(TRUE) == FAILURE) {
    return FAILURE;
  }

  area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
  ptext("Reading Whole House Summary", 205 + 115, 175, BG_WHT + FG_BLK);
  if (FetchTable(f_buffer, //&meter,
                 sizeof(meter),
                 METER_TABLE,
                 NULL,
                 0,
                 RETRY_COUNT,
                 TRUE) != 0) {
    msgbox("Timed out reading whole|"
           "house summary from ICU. ",
           "Error", MB_OK);
    return FAILURE;
  } else {
    decompress((void*)&meter, f_buffer, sizeof(METER));
  }
  if (log_on() == FAILURE) {
    msgbox("LaneView was unable to reconnect to ICU.", "Fatal Error", MB_OK);
    lv_stop();
  }

  glbWindow = tmpWindow;
  PutWindow(glbWindow);
  return SUCCESS;
}

#if LOAD_CONTROL_TECO
int read_load_control_info(void) {
  memset(&load_ctrl_tbl, 0, (MAX_LOAD_CTRLS + 1) * sizeof(LOAD_CTRL));
  if (GetTable(&load_ctrl_tbl,
               (MAX_LOAD_CTRLS + 1) * sizeof(LOAD_CTRL),
               LOAD_SHED_TABLE,
               NULL,
               0,
               RETRY_COUNT,
               FALSE) != SUCCESS) {
    return 1;
  }
  return 0;
}

read_device_status_info(USHORT device_id) {
  memset(&device_status, 0, sizeof(DEVICE_STATUS));

  if (GetTable(&device_status,
               sizeof(DEVICE_STATUS),
               DEVICE_STATUS_TABLE,
               &device_id,
               sizeof(USHORT),
               RETRY_COUNT,
               FALSE) != SUCCESS) {
    return 1;
  }
  return 0;
}

int send_load_ctrl_override(USHORT device_id, time_t start_time, time_t end_time) {
  int times;
  LOAD_CTRL_OVERRIDE load_ctrl_override;

  load_ctrl_override.device_id = device_id;
  load_ctrl_override.start_time = start_time;
  load_ctrl_override.stop_time = end_time;

  times = 0;
  while (++times < 5) {
    if (SendTable(LOAD_SHED_OVR_TABLE, &load_ctrl_override, sizeof(LOAD_CTRL_OVERRIDE)) == SUCCESS) {
      return (0);
    }
  }
  return (1);
}
#endif // LOAD_CONTROL_TECO

#define LEN_OBJECT_INFO 12
UCHAR object_info[LEN_OBJECT_INFO] =
    {
        0x050, 0x0E8, 0x050,
        0x002, 0x046, 0x0F4,
        0x032, 0x0F6, DEVICE_TABLE,
        0x020, 0x0F5, 'A'};

static int SendObjectBody(PKT_INFO* pkt_info, UCHAR* object, int o_size);

// add or change (to ICU) devices, events, moods, triggers
int SendObject(void* object, int option, int o_size, int table_type) {
  int retry;
  USHORT ch_status;
  PKT_INFO pkt_info;
  UCHAR history_bucket;
  DEVICE* dev;
  EVENT* evnt;
  MOOD_T* mood;
  TRIGGER_T* trig;

  if (DEMO == TRUE) {
    return SUCCESS;
  }
  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;

  memcpy(&pkt_info.pkt.INFO[0], object_info, LEN_OBJECT_INFO);
  pkt_info.pkt.INFO[8] = (UCHAR)table_type;
  pkt_info.pkt.INFO[11] = (UCHAR)option; //0x41 for add,  0x43 for change

  retry = 0;
  while (1) {
    if ((SendObjectBody(&pkt_info, object, o_size)) == SUCCESS) {
      switch (table_type) {
      case DEVICE_TABLE:
        dev = object;
        if (get_reply2(&ch_status, &history_bucket) == SUCCESS) {
          if ((option == 'A') || (option == 'C')) {
            dev->device_slot = ch_status;
            if (IsSubmeter(dev) == TRUE) {
              if ((history_bucket < 1) || (history_bucket > MAX_HISTORY)) {
                msgbox("Device not saved!             |"
                       "Invalid history bucket address|"
                       "for sub meter device received. ",
                       "Warning", MB_OK);
                return FAILURE;
              } else {
                dev->history_bucket = history_bucket;
              }
            } else {
              if (history_bucket != 0) {
                msgbox("Device not saved!                 |"
                       "Invalid history bucket address for|"
                       "non-meter type device revceived.   ",
                       "Warning", MB_OK);
                return FAILURE;
              } else {
                dev->history_bucket = history_bucket;
              }
            }
          }
          device_chg_icu = get_reply_time;
          return SUCCESS;
        }
        break;
      case EVENT_TABLE:
        if ((option == 'A') || (option == 'C')) {
          evnt = object;
          if (get_reply(&ch_status) == SUCCESS) {
            evnt->event_id = ch_status;
            event_chg_icu = get_reply_time;
            return SUCCESS;
          }
        }
        break;
      case TRIGGER_TABLE:
        if (get_reply(&ch_status) == SUCCESS) {
          trig = object;
          trig->trigger_id = ch_status;
          trigger_chg_icu = get_reply_time;
          return SUCCESS;
        }
        break;
      case EXT_MOOD_TABLE:
        if (get_reply(&ch_status) == SUCCESS) {
          mood = object;
          mood->mood_id = ch_status;
          mood_chg_icu = get_reply_time;
          return SUCCESS;
        }
        break;
      case SECURITY_TABLE:
        if (get_reply(&ch_status) == SUCCESS) {
          security_chg_icu = get_reply_time;
          return SUCCESS;
        }
        break;
      default:
        msgbox("Logic error.      |"
               "Unexpected case in|"
               "SendObject():INIO.C",
               "Sending Object", MB_OK);
        return FAILURE;
        break;
      }
    }
    if (retry++ > (RETRY_COUNT * 2)) {
      msgbox("Timed out waiting for ICU.", "Warning", MB_OK);
      return FAILURE;
    }
  }
}

int SendObjectBody(PKT_INFO* pkt_info, UCHAR* object, int o_size) {
  SSHORT count;
  SSHORT retry;
  UCHAR tmp[15];

  pkt_info->length = 41;

  count = 0;
  while (1) {
    pkt_info->pkt.INFO[LEN_OBJECT_INFO] = (UCHAR)count++;
    memcpy(&pkt_info->pkt.INFO[LEN_OBJECT_INFO + 1], object, 19);
    retry = 0;
    while (1) {
      if (SendPacket(pkt_info) == RT_SUCCESS) {
        object += 19;
        o_size -= 19;
        if (o_size <= 0) {
          //Status("");
          return SUCCESS;
        }
        break; //continue outer loop
      } else if (++retry > RETRY_COUNT) {
        return FAILURE;
      }
      sprintf(tmp, "SendObjectBody() - Retry %2d", retry);
      ExceptionLog(tmp);
      //Status(tmp);
    }
  }
}

#define LMOOD_EVENT_INFO 11
unsigned char MOOD_EVENT_INFO[LMOOD_EVENT_INFO] =
    {
        0x050, 0x0E8, 0x050,
        0x002, 0x046, 0x0F4,
        0x032, 0x0F6, MOOD_STATE_TABLE,
        0x020, 0x0F5};

//mood id of 0x0FFFF ends all moods
int SetMoodState(USHORT mood_id, int active, int prot, USHORT duration) {
  int retry;
  PKT_INFO pkt_info;
  UCHAR tmp[15];

  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;

  memcpy(&pkt_info.pkt.INFO[0], MOOD_EVENT_INFO, LMOOD_EVENT_INFO);
  memcpy(&pkt_info.pkt.INFO[11], &mood_id, sizeof(USHORT));
  pkt_info.pkt.INFO[13] = active;
  pkt_info.pkt.INFO[14] = prot;
  memcpy(&pkt_info.pkt.INFO[15], &duration, sizeof(USHORT));

  pkt_info.length = 9 + 6 + LMOOD_EVENT_INFO;

  for (retry = 0; retry < RETRY_COUNT; retry++) {
    if (send_packet(&pkt_info) == SUCCESS) {
      //Status("");
      return SUCCESS;
    }
    sprintf(tmp, "SetMoodState - Retry %.2d", retry);
    //Status(tmp);
    ExceptionLog(tmp);
  }
  return FAILURE;
}

//==========================================================================
// GetTable()
//
// Routine History:
// Author               Date            Description
//
//   MLauter               12/14/96         Initial write
//
//
//
// Inputs:
//  table_ptr            void *            pointer to table buffer
//  table_size            int               exact size of incoming table
//  table_id            int               system defined table number
//  request_data         UCHAR *            additional data to be sent
//                                       with request
//  data_length            int               length of request_data
//  retries               int               times to loop a wait, if errors
//  show_status            int               TRUE/FALSE:  Display Indicator
//
// Returns:
//  SUCCESS               int               Successful completion of Transfer
//  -1                  int               CAL error code received (0x0FD)
//  FAILURE               int               retries exceeded OR other error
//
//
//
// Description:
//
//   Sends a request for a table from the ICU and then assembles the
//  returned packets into the table area.
//
//   -- Also Processes Interval Data
//
//  -- Also looks for ESC from keyboard for operator aborts.
//==========================================================================
int GetTable(void* table_ptr, int table_size, int table_id, void* request_data,
             int data_length, int retries, int show_status) {
  int times;
  int result;
  char work[80];

  PKT_INFO pkt_info;

  if (DEMO == TRUE) {
    return SUCCESS;
  }
  if ((data_length + 11) > CEB_DATA_BYTES) {
    return FAILURE; // too long to proceed...
  }

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8 + (UCHAR)((++running_sequence) & 0x007);
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;
  pkt_info.pkt.INFO[4] = 0x044;
  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = table_id;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;

  pkt_info.length = 20 + data_length;

  if (data_length > 0) {
    memcpy(&pkt_info.pkt.INFO[11], request_data, data_length);
  }

  for (times = 0; times < retries; ++times) {
    if (kbhit()) {
      if (kbchar() == ESC) {
        msgbox("Data transfer canceled by user.", "Information", MB_OK);
        lv_stop();
      }
    }

    if (SendPacket(&pkt_info) == RT_SUCCESS) {
      result = ReceiveTable(table_ptr, table_size, show_status, table_id);
      switch (result) {
      case RT_SUCCESS:
        //Status("");
        return SUCCESS;

      case RT_NO_MAIL:
        return RT_NO_MAIL;

      case RT_TIMEOUT:
      case RT_CERROR:
        ce_bus_reset();
        break; // Start Over

      case RT_ICU_BUSY:
      case RT_BAD_PWD:
        return result;

      case RT_FD:
      case RT_NOHIST:
        return -1;

      default:
        sprintf(work, "ICU returned error code: %2d", result);
        msgbox(work, "GetTable():INIO.C", MB_OK);
        return FAILURE;
      }
    }
    sprintf(work, "GetTable() - Request %.2d", times + 1);
    ExceptionLog(work);
    //Status(work);
  }
  return FAILURE;
}

//==========================================================================
// SendPacket()
//
// Routine History:
// Author               Date            Description
//
//  BNevins
//   GSpeegle            12/14/96         Handle more errors
//
//
//
// Inputs:
//  packet               PKT_INFO *         pointer to Packet to Send
//
// Returns:
//  RT_SUCCESS            int               Successful Transmit
//  CERROR               int               other error
//
//
//
// Description:
//
//   Sends a Packet to the ICU, RETRY_COUNT times if necessary, and waits
//  for an ACK to come back.
//
//   -- Also Processes Interval Data
//
//  -- Also looks for ESC from keyboard for operator aborts.
//==========================================================================
int pending_detected;
static int SendPacket(PKT_INFO* packet) {
  int times;
  int cebus_status;
  int resend;

  ///////////////////////////////////////////////////////////////
  // GESGESGESGESGESGESGESGESGESGESGESGESGESGESGESGES
  // Next instruction
  ///////////////////////////////////////////////////////////////
  char work[100];
  //int      pending_detected;

  if (DEMO == TRUE) {
    return SUCCESS;
  }
  for (times = 0; times < RETRY_COUNT * 2; ++times) {
    if ((req_id = CEBUS__SendPacket(packet)) == 0L) {
      ce_bus_reset();
      //         Status("CEBUS RESET HEY HEY HEY HEY HEY 1");
      //         Wait(18L*5);
      continue;
    }

    ticks_set(1, 8L);
    resend = FALSE;
    while (1) // Waiting for Success or Failure on Packet Transmit
    {
      if (CEBUS__GetTransmitStatus(req_id, &(XMIT_STAT)cebus_status)) {
        switch (cebus_status) {
        case FAILURE:     // FAILURE -- failed to receive an ACK
          ce_bus_reset(); // maybe this will help
          resend = TRUE;  // Re-transmit
                          //Status("CASE FAILURE");
                          //Wait(18L*5);
          break;

        case PENDING: // Pending (transmit not yet complete)
          if (ticks_check(1)) {
            ce_bus_reset();
            resend = TRUE;              // Re-transmit
                                        ///////////////////////////////////////////////////////////////
                                        // GESGESGESGESGESGESGESGESGESGESGESGESGESGESGESGES
                                        // Next 4 instructions
                                        ///////////////////////////////////////////////////////////////
            if (pending_detected < 100) // don't over-flow
            {
              ++pending_detected;
            }
          }
          //Status("CASE PENDING");
          break;

        case IDLE: // IDLE
          ce_bus_reset();
          resend = TRUE; // Re-transmit
                         //Status("CASE IDLE");
          break;

        case SUCCESS:               // SUCCESS
                                    ///////////////////////////////////////////////////////////////
                                    // GESGESGESGESGESGESGESGESGESGESGESGESGESGESGESGES
                                    // Next 5 instructions
                                    ///////////////////////////////////////////////////////////////
          if (pending_detected > 2) // Seems to happen on very fast processors...
          {
            sprintf(work, "%3d", packet->pkt.INFO[12]);
            sprintf(work, "%3d", packet->pkt.INFO[12]);
          }
          //Status("");
          //Status("CASE SUCCESS");
          return RT_SUCCESS;
        }
      } else // our packet was lost!!!
      {
        //Status("ELSE");
        ce_bus_reset();
        resend = TRUE; // Re-transmit
        break;
      }
      if (resend == TRUE) {
        //Status("RESEND == TRUE");
        break; // break to for loop & start over
      }
    }
  }
  //Status("CERROR");
  return CERROR; // Exceeded RETRY_COUNT
}

//==========================================================================
// ReceiveTable()
//
// Routine History:
// Author               Date            Description
//
//  BNevins               ????
//   MLauter               12/14/96         Rearrange & add Return codes
//
// Inputs:
//  table_ptr            void *            pointer to table buffer
//  table_size            int               exact size of incoming table
//  show_status            int               TRUE/FALSE:  Display Indicator
//  table_id            int               system defined table number
//
// Returns:
//  RT_SUCCESS            int               Successful completion of Transfer
//  RT_TIMEOUT            int               timeout
//  RT_CERROR            int               CEBus error
//   RT_FD               int               ICU sent FD (CAL error)
//   RT_NOHIST            int               if(table_id == HISTORY_TABLE)
//                                       history unavailable.
//
//
//
// Description:
//
//   Receives a Table from the ICU that was previously requested
//   -- Also Processes Interval Data
//  -- Also looks for ESC from keyboard for operator aborts.
//
//   calls lv_stop on ESC key from operator
//
//==========================================================================
int ReceiveTable(void* table_ptr, int table_size, int show_status, int table_id) {
  char work[80];
  UCHAR* password;
  UCHAR* cp;
  USHORT index = 0, len = 0, tmp = 0;
  PKT_INFO incoming_pkt;
  PROGRESS_INDICATOR_T pi = {245 + 115, 220, 200, 0, FG_BLU, 0};
  FAST_HISTORY_DAY_T* fhd;

  int pass_word_check = 0;

  if (DEMO == TRUE) {
    return SUCCESS;
  }

  if (show_status == TRUE) {
    PutIndicator(&pi);
  }

  cp = table_ptr;
  if (table_size > 29) {
    len = 29;
    tmp = index = table_size / 29;
    if ((table_size % 29) == 0) {
      --index;
    }
  } else {
    len = table_size;
    index = 0;
  }

  ticks_set(1, 100L); /* set alarm for first packet 5.55 seconds */
  while (1) {
    if (kbhit()) {
      if (kbchar() == ESC) {
        msgbox("Data transfer canceled by user.", "Information", MB_OK);
        lv_stop();
      }
    }

    memset(&incoming_pkt, 0, sizeof(PKT_INFO));
    switch (CEBUS__GetPacket(&incoming_pkt)) {
    case 0:
      if (ticks_check(1) == TRUE) {
        return RT_TIMEOUT;
      }
      continue;

    case CERROR:
      //Status("Packet Error");
      ExceptionLog("Packet Error");
      ce_bus_reset();
      return RT_CERROR;
    }

    switch (PacketType(&incoming_pkt)) {
    case INTERVAL_30S:
      process_interval(&incoming_pkt);
      continue;
    case INTERVAL_30S$:
      process_interval$(&incoming_pkt);
      continue;
    case INTERVAL_15M:
      process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
      continue;
    case ALERT_MSG:
      continue;
    case NEW_RATE_MSG:
      continue;
    }

    //ignore communications from non-ICU devices
    if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
      continue;
    }

    ticks_set(1, 100L); /* reset alarm */

    if ((USHORT)(incoming_pkt.pkt.INFO[1] & 0x007) != (running_sequence & 0x007)) {
      continue;
    }

    if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && (incoming_pkt.pkt.INFO[3] == 1) && (table_id == HISTORY_TABLE)) {
      return RT_NOHIST; //history not available
    }

    if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && (incoming_pkt.pkt.INFO[3] == 1) && (table_id == VERSION_TABLE)) {
      return RT_ICU_BUSY; // another *view type logged in
    }

    if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && ((incoming_pkt.pkt.INFO[1] & 0x0F8) == 0x0D0) && (table_id == EMAIL_TABLE)) {
      return RT_NO_MAIL; //no mail
    }

    if (
        (
            (incoming_pkt.pkt.INFO[2] == 0x0FD) &&
            (incoming_pkt.pkt.INFO[3] == 5) &&
            (table_id == VERSION_TABLE)) ||
        ((table_id == VERSION_TABLE) &&
         (memcmp(&incoming_pkt.pkt.INFO[2], "\x50\x44\x3A", 3) == 0) // password requested
         )) {
      if (++pass_word_check > 3) {
        return RT_BAD_PWD;
      }
      password = inputbox("Please enter your password.", "Request Login Password", "*");
      if (password[0] == 0) // user hit cancel
      {
        lv_stop();
      }
      Status("Please wait...");
      if (SendPassword(password) == SUCCESS) {
        Status("Password ok.");
        ticks_set(1, 100L); /* set alarm for first packet 5.55 seconds */
        continue;
      } else {
        msgbox("Error logging in.      |"
               "Please restart LaneView.",
               "Password Error", MB_OK);
        lv_stop();
        continue;
      }
    }

    if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && (table_id == FAST_HISTORY_TBL) && (incoming_pkt.length == 41)) {
      // it's ok!!
    } else if (incoming_pkt.pkt.INFO[2] == 0x0FD) {
      return RT_FD;
    }

    if (table_id == GET_DIR_TABLE) // variable length
    {
      if (incoming_pkt.pkt.INFO[2] & 0x080) // first packet
      {
        memcpy(&table_size, &incoming_pkt.pkt.INFO[3], 2);
        if (table_size > 29) {
          len = 29;
          tmp = index = table_size / 29;
          if ((table_size % 29) == 0) {
            --index;
          }
        } else {
          len = table_size;
          index = 0;
        }
      }
    }

#if 0
      if ((table_id == METER_TABLE) ||         // variable length
         (table_id == DEVICE_HISTORY_TABLE))
      {
         if (incoming_pkt.pkt.INFO[2] & 0x080)   // first packet
         {
            memcpy(&table_size, &incoming_pkt.pkt.INFO[3], 2);
            if (table_size > 29)
            {
               len = 29;
               tmp = index = table_size / 29;
               if ((table_size % 29) == 0)
               {
                  --index;
               }
            }
            else
            {
               len = table_size;
               index = 0;
            }
            //sprintf(work, "Size: %u, Packets left %d", table_size, index);
            //Status(work);
         }
      }
#endif

    if (table_id == FAST_HISTORY_TBL) // variable length
    {
      if (incoming_pkt.pkt.INFO[2] & 0x080) // first packet
      {
        fhd = (void*)&incoming_pkt.pkt.INFO[3];
        table_size = 5 + fhd->number_intervals * sizeof(FAST_PERIOD_DATA);
        if (table_size > 29) {
          len = 29;
          tmp = index = table_size / 29;
          if ((table_size % 29) == 0) {
            --index;
          }
        } else {
          len = table_size;
          index = 0;
        }
      }
    }

    if ((table_size > 29) && ((USHORT)(incoming_pkt.pkt.INFO[2] & 0x07F) != (index & 0x07F))) {
      if (((USHORT)(incoming_pkt.pkt.INFO[2] - 1) & 0x7f) != (index & 0x07F)) {
        sprintf(work,
                "Got %d, expected %d",
                incoming_pkt.pkt.INFO[2] & 0x7f,
                (index & 0x07F));
        ExceptionLog(work);
      }
      continue;
    }

    if (show_status) {
      if ((index % 4) == 0) {
        pi.progress = 100 - (int)((index / (tmp + .01)) * 100);
        UpdateIndicator(&pi);
      }
    }

    memcpy(cp, &incoming_pkt.pkt.INFO[3], len);

    if (index == 0) {
      break;
    }

    if (index == 1) {
      len = table_size - (table_size / 29) * 29;
      if (len == 0) {
        len = 29;
      }
    }

    cp += 29;
    --index;
  }
  //Status("");
  return RT_SUCCESS;
}

#define LDEL_TRIGGER_INFO 12
unsigned char DEL_TRIGGER_INFO[LDEL_TRIGGER_INFO] =
    {
        0x050, 0x0E8, 0x050,
        0x002, 0x046, 0x0F4,
        0x032, 0x0F6, TRIGGER_TABLE,
        0x020, 0x0F5, 0x044};

RemoveTrigger(TRIGGER_T* trig) {
  USHORT i;
  int times;
  int result;
  int trig_index = 0;
  PKT_INFO pkt_info;
  USHORT trigger_id;

  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;

  memcpy(&pkt_info.pkt.INFO[0], DEL_TRIGGER_INFO, LDEL_TRIGGER_INFO);
  memcpy(&pkt_info.pkt.INFO[12], &trig->trigger_id, 2);
  pkt_info.length = 9 + 2 + LDEL_TRIGGER_INFO;

  times = 0;
  while (1) {
    result = send_packet(&pkt_info);
    if (result == SUCCESS) {
      result = get_reply(&trigger_id);
      if (result == SUCCESS) {
        break;
      }
    }
    if (times++ > RETRY_COUNT) {
      return FAILURE;
    }
  }

  for (i = 0; i < triggers_used; ++i) {
    if (triggers[i].trigger_id == trig->trigger_id) {
      trig_index = i;
      break;
    }
  }
  for (i = trig_index; i < triggers_used; i++) {
    triggers[i] = triggers[i + 1];
  }
  --triggers_used;
  memset(&triggers[triggers_used], 0, sizeof(TRIGGER_T));
  return user_backup();
}

//==========================================================================
// SendPrg()
//
// Routine History:
// Author               Date            Description
//   GSpeegle            12/23/96
//
// Inputs:
//  Source File            char *            input file name
//  Dest File            char *            file name to give ICU
//
// Returns:
//  0                  int               Successful completion of Transfer
//   1                  int               initial call to SendPacket problem
//   3                  int               subsequent SendPacket problem
//  5                  int               ICU busy
//  100                  int               file not found
//  101                  int               file open error
//                                       history unavailable.
//
//
//
// Description:
//
//   Sends a disk file to ICU, changing name if required.  Much like sendprog.exe
//   -- Also Processes Interval Data
//
//==========================================================================

int SendPrg(char* src, char* dst) {
  int send_status;
  FILE* stream;
  struct stat buf;

  if (stat(src, &buf)) {
    return (100);
  }

  stream = fopen(src, "rb");
  if (stream == NULL) {
    perror("Error opening file");
    return (101);
  }
  send_status = send_it(dst, &buf, stream);
  fclose(stream);
  return (send_status);
}

#define LSTART_PROG 12
unsigned char START_PROG[LSTART_PROG] =
    {
        0x050, 0x0E8, 0x050, 0x002, 0x046,
        0x0F4, 0x032, 0x0F6, 0x0FF, 0x020, 0x0F5, 0};

int send_it(char* dst, struct stat* buf, FILE* stream) {
  char inarea[19];
  int icu_return_code;
  int ecount;
  int size;
  int total_packets;
  int packets_left;
  int error_count;
  int outside_packets;
  PKT_INFO pkt;
  PROGRESS_INDICATOR_T pi = {245 + 115, 220, 200, 0, FG_BLU, 0};

  PutIndicator(&pi);

  packets_left = (int)(buf->st_size / 19);
  if ((buf->st_size % 19) == 0) {
    --packets_left;
  }
  total_packets = packets_left;
  error_count = 0;

  //printf(" and %d packets.\r\n", packets_left + 1);
top:
  pkt.pkt.CONTROL = 9;
  pkt.pkt.DA = 1;
  pkt.pkt.DHC = cebusSetup.OutsideHC;
  pkt.pkt.SA = 1;
  pkt.pkt.SHC = cebusSetup.HouseCode;
  memcpy((void*)&pkt.pkt.INFO[0], START_PROG, LSTART_PROG);
  memcpy((void*)&pkt.pkt.INFO[LSTART_PROG], (void*)&buf->st_size, 4);
  memcpy((void*)&pkt.pkt.INFO[LSTART_PROG + 4], dst, 12);
  pkt.length = LSTART_PROG + 9 + 4 + 12;

  ecount = 0;
  while (1) {
    if (SendPacket(&pkt) != RT_SUCCESS) {
      if (++ecount > 40) {
        //printf("I/O error 1\r\n");
        return (1);
      } else {
        continue;
      }
    } else {
      break;
    }
  }

  pkt.pkt.INFO[LSTART_PROG - 1] = 1; // show body of program coming
  while (packets_left >= 0) {
    size = fread(inarea, 1, 19, stream);
    pkt.pkt.INFO[LSTART_PROG] = packets_left & 0x07f;
    memcpy((void*)&pkt.pkt.INFO[LSTART_PROG + 1], inarea, size);
    pkt.length = LSTART_PROG + 9 + 1 + size;
    ecount = 0;
    while (1) // added to retry sending packets
    {
      if (SendPacket(&pkt) != RT_SUCCESS) {
        if ((++ecount) < 20) {
          continue;
        } else {
          //printf("I/O error 2\r\n");
          return (3);
        }
      } else {
        break;
      }
    }
    if (((pkt.pkt.INFO[LSTART_PROG] % 16) == 0) && (packets_left != 0)) {
      if (icu_return_code = sf_get_reply(packets_left, &outside_packets)) {
        if (icu_return_code == 100) // he's busy now
        {
          return (5);
        }

        //ce_bus_reset();
        if (++error_count < 20) {
          if ((packets_left + 16) >= total_packets) {
            packets_left = total_packets;
            fseek(stream, 0L, SEEK_SET);
            goto top;
          } else {
            if (outside_packets > 0) {
              packets_left += 15;
              size = 19;
              if (fseek(stream, (long)(total_packets - packets_left) * (long)size, SEEK_SET) == 0) {
                continue;
              }
            }
          }
        }
        return (4);
      } else {
        error_count = 0;
      }
    }
    --packets_left;
    if (((total_packets - packets_left) % 4) == 0) {
      pi.progress = 100 - ((packets_left * 100) / total_packets);
      UpdateIndicator(&pi);
    }
  }
  return (0);
}

int sf_get_reply(int which, int* outside_packets) {
  int result;
  PKT_INFO incoming_pkt;

  if (DEMO == TRUE) {
    return 0;
  }
  ticks_set(1, 100L); /* set 100 tick alarm ( seconds) */
  while (1) {
    result = CEBUS__GetPacket(&incoming_pkt);

    if (result == 0) // no packets came in
    {
      if (ticks_check(1)) {
        return (-1);
      }
      continue;
    } else if (result == CERROR) // PLNode error
    {
      //ce_bus_reset();
      //printf("Get Packet Error\r\n");
      return (1);
    } else // got something
    {
      switch (PacketType(&incoming_pkt)) // wazzit unsolicited interval data?
      {
      case INTERVAL_30S:
        process_interval(&incoming_pkt);
        continue;
      case INTERVAL_30S$:
        process_interval$(&incoming_pkt);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }

      if (incoming_pkt.pkt.SHC != (unsigned int)cebusSetup.OutsideHC) // not ICU
      {
        continue;
      }

      if (incoming_pkt.pkt.INFO[2] == 0x0FD) // error (he's busy??)
      {
        return (100);
      }

      if (incoming_pkt.pkt.INFO[2] == 0x0FE) // successful update
      {
        memcpy(outside_packets, &incoming_pkt.pkt.INFO[3], sizeof(int));
        if ((*outside_packets > 0) && (*outside_packets != which)) {
          continue;
        }
        //putch('.');
        return (0);
      }
    }
  }
}

//==========================================================================
// verify_file_date()
//
// Routine History:
// Author               Date            Description
//   GSpeegle            12/23/96
//
// Inputs:
//  fname               char            file's name
//  struct _stat &buf                     _stat table pointer from our file
//
// Accesses:
//   f_buffer            int               Directory table read from ICU
//
// Returns:
//  0                  int               Ours later or equal to ICU's
//   1                  int               ICU has a newer one than us.
//
//
// Description:
//   Looks up a single entry in the ICU directory, that matches our directory entry.
//   Then a date/time comparison is made to see which is newer
//
//==========================================================================
int verify_file_date(char* fname, struct _stat* buf) {
  int i;
  int total;
  int year;
  int month;
  int day;
  int hour;
  int min;
  int sec;
  DIR_TABLE* dt;
  struct tm* f_tm;

  dt = (void*)f_buffer;
  total = (dt->dir_size - 6) / (sizeof(ENTRY));

  for (i = 0; i < total; ++i) {
    if (strcmp(fname, dt->entry[i].fname) == 0) // we found him!
    {
      // ICU file's time
      year = (dt->entry[i].fdate >> 9) + 80;
      month = (dt->entry[i].fdate >> 5) & 0x0F;
      day = dt->entry[i].fdate & 0x01F;
      hour = dt->entry[i].ftime >> 11;
      min = (dt->entry[i].ftime >> 5) & 0x03F;
      sec = (dt->entry[i].ftime & 0x01F) * 2;

      // get our file's time
      f_tm = localtime(&buf->st_mtime); // when was he last modified?

      // Now, see who's newer
      if (year > f_tm->tm_year) {
        return (1);
      } else if (year < f_tm->tm_year) {
        return (0);
      }

      // years are equal... check month
      if (month > (f_tm->tm_mon + 1)) {
        return (1);
      } else if (month < (f_tm->tm_mon + 1)) {
        return (0);
      }

      // months are equal... check day
      if (day > f_tm->tm_mday) {
        return (1);
      } else if (day < f_tm->tm_mday) {
        return (0);
      }

      // days are equal... check hour
      if (hour > f_tm->tm_hour) {
        return (1);
      } else if (hour < f_tm->tm_hour) {
        return (0);
      }

      // hours are equal... check minutes
      if (min > f_tm->tm_min) {
        return (1);
      } else if (min < f_tm->tm_min) {
        return (0);
      }

      // minutes are equal... check seconds
      if (sec > f_tm->tm_sec) {
        return (1);
      } else if (sec < f_tm->tm_sec) {
        return (0);
      }
      return (0); // they must be the same.... no need to update
    }
  }
  return (0); // not found on icu... no use asking for it..
}

//==========================================================================
// CheckRateTableTimes()
//
// Routine History:
// Author               Date            Description
//   GSpeegle            12/23/96
//
// Inputs:
//  none
//
// Accesses:
//   f_buffer            int               Directory table read from ICU
//
// Returns:
//  0                  int               no rate files to update
//   1                  int               must update this month's rate file
//   2                  int               must update last month's rate file
//   3                  int               must update both month's rate files
//
//
//
// Description:
//
//   Compares the directory just received from the ICU & compares update times
//   for this month's & last month's rate files
//
//==========================================================================
int CheckRateTableTimes(void) {
  char work[20];
  int month;
  int year;
  int month_old;
  struct _stat buf;

  month = tod->tm_mon + 1;
  year = tod->tm_year;
  --month; // need to check last month's
  if (month == 0) {
    month = 12;
    --year;
  }
  if (year < 100) {
    sprintf(work, "RT%02d%02d", month, year);
  } else {
    sprintf(work, "RT%02d%04d", month, year + 1900);
  }
  if (_stat(work, &buf) == 0) {
    if (verify_file_date(work, &buf) == 0) {
      month_old = 0;
    } else {
      unlink(work);
      month_old = 2;
    }
  } else {
    month_old = 2;
  }

  time(&ltime);
  tod = localtime(&ltime);
  month = tod->tm_mon + 1;
  year = tod->tm_year;
  if (year < 100) {
    sprintf(work, "RT%02d%02d", month, year);
  } else {
    sprintf(work, "RT%02d%04d", month, year + 1900);
  }
  if (_stat(work, &buf) == 0) {
    if (verify_file_date(work, &buf) == 0) {
      //month_old += 0;
    } else {
      unlink(work);
      month_old += 1;
    }
  } else {
    month_old += 1;
  }
  time(&ltime);            // we modified system tod tables
  tod = localtime(&ltime); // .... so they need to be put back right
  return (month_old);
}

//==========================================================================
// AskToReload
//
// Routine History:
// Author               Date            Description
//   GSpeegle            12/23/96
//
// Inputs:
//  none
//
// Accesses:
//   Backup files
//
// Returns:
//  0                  int               continue with no devices
//   1                  int               Exit now: We sent backups to ICU
//
//
//
// Description:
//
//   1. Called when ICU reports no devices defined
//   2. Offers to send backup files if found, otherwise simply returns to continue
//      logon process
//
//==========================================================================
int AskToReload(void) {
  int rc;
  int custbkup;
  struct _stat buf;

  // no devices on icu... see if we can find some
  custbkup = FALSE;
  rc = _stat("DEVICES.DAT", &buf);
  if (rc != 0) // DEVICES.DAT not found, how about CUSTBKUP.DAT?
  {
    //      rc = _stat("CUSTBKUP.DAT", &buf);
    //      if (rc != 0)      // DEVICES.DAT not found, how about CUSTBKUP.DAT?
    //      {
    return (0); // nothing found might as well continue to log on
                //      }
                //      else
                //      {
                //         custbkup = TRUE;      // all we have is an old custbkup.dat file
                //      }
  }

  rc = msgbox("No devices defined on ICU.           |"
              "Would you like to reload from backups?",
              "Logging onto ICU", MB_YES_NO);
  if (rc == MB_RESULT_YES) {
    RELOAD = 0;
    //area_clear(290, 405, 155, 480, FG_WHT);
    if (custbkup == FALSE) // do it the new way
    {
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Sending Devices", 205 + 115, 175, BG_WHT + FG_BLK);
      rc = SendPrg("DEVICES.DAT", "DEVICES");
      if (rc == 0) {
        Status("Devices sent successfully to ICU");
      } else {
        msgbox("Devices NOT sent to ICU", "A Problem...", MB_OK);
        lv_stop();
      }
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Sending Events", 205 + 115, 175, BG_WHT + FG_BLK);
      rc = SendPrg("EVENTS.DAT", "EVENTS");
      if (rc == 0) {
        Status("Events sent successfully to ICU");
      } else {
        msgbox("Events NOT sent to ICU", "A Problem...", MB_OK);
        lv_stop();
      }
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Sending Moods", 205 + 115, 175, BG_WHT + FG_BLK);
      rc = SendPrg("MOODS.DAT", "MOODS");
      if (rc == 0) {
        Status("Moods sent successfully to ICU");
      } else {
        msgbox("Moods NOT sent to ICU", "A Problem...", MB_OK);
        lv_stop();
      }
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Sending Triggers", 205 + 115, 175, BG_WHT + FG_BLK);
      rc = SendPrg("TRIGGERS.DAT", "TRIGGERS");
      if (rc == 0) {
        Status("Triggers sent successfully to ICU");
      } else {
        msgbox("Triggers NOT sent to ICU", "A Problem...", MB_OK);
        lv_stop();
      }
      area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
      ptext("Sending Security", 205 + 115, 175, BG_WHT + FG_BLK);
      rc = SendPrg("SECURITY.DAT", "SECURITY");
      if (rc == 0) {
        Status("Security sent successfully to ICU");
      } else {
        msgbox("Security NOT sent to ICU", "A Problem...", MB_OK);
        //lv_stop();
      }
      rc = SendTable(0x0FE, NULL, 1); // reboot ICU
      if (rc == SUCCESS) {
        msgbox("Everything successfully sent.|"
               "ICU rebooting.               |"
               "Please restart LaneView.      ",
               "Reloading Data Files", MB_OK);
      } else {
        msgbox("Everything successfully sent.|"
               "PROBLEM: ICU NOT REBOOTING    ",
               "Reloading Data Files", MB_OK);
      }
      lv_stop();
    } else // do it the old way
    {
      if (rc = send_custbkup()) {
        rc = send_custbkup(); // try again
      }
      if (rc == 0) {
        Status("Everything successfully sent to ICU");
      } else {
        msgbox("Unable to reboot ICU.", "A Problem...", MB_OK);
      }
      exit_no = 1;
      lv_stop();
    }
    return (1);
  } else {
    RELOAD = 1;
    return (0); // continue logging on....
  }
}

int send_custbkup(void) {
  char* cp;
  int i;
  int len;
  int bytes;
  int times;
  short bkup_version;
  short bkup_devices;
  short bkup_events;
  USHORT index;
  PKT_INFO out_going;
  FILE* stream;

  stream = fopen("CUSTBKUP.DAT", "rb");
  if (stream != NULL) {
    fread(&bkup_version, 2, 1, stream);
    fread(&bkup_devices, 2, 1, stream);
    fread(&bkup_events, 2, 1, stream);
    fread(&dev_list.devices, sizeof(DEVICE), bkup_devices, stream);
    fread(events, sizeof(EVENT), bkup_events, stream);
    fclose(stream);
  }

  if ((bkup_version != DEVICES_VERSION) && (bkup_version != -3)) // this will work with version -3 OR -4
  {
    return (1);
  }

  out_going.pkt.CONTROL = 9;
  out_going.pkt.SA = 1;
  out_going.pkt.SHC = cebusSetup.HouseCode;
  out_going.pkt.DA = 1;
  out_going.pkt.DHC = cebusSetup.OutsideHC;
  out_going.pkt.INFO[0] = 0x050;
  out_going.pkt.INFO[1] = 0x0E8;
  out_going.pkt.INFO[2] = 0x050;
  out_going.pkt.INFO[3] = 0x002;
  out_going.pkt.INFO[4] = 0x046;
  out_going.pkt.INFO[5] = 0x0F4;
  out_going.pkt.INFO[6] = 0x032;
  out_going.pkt.INFO[7] = 0x0F6;
  out_going.pkt.INFO[9] = 0x020;
  out_going.pkt.INFO[10] = 0x0F5;
  out_going.pkt.INFO[8] = 1;
  memcpy(&out_going.pkt.INFO[11], &bkup_devices, 2);
  memcpy(&out_going.pkt.INFO[13], &bkup_events, 2);
  out_going.length = 24;

  // Send Version information.
  if (send_packet(&out_going)) {
    return (1);
  }

  ticks_set(2, 30L);
  while (ticks_check(2) == 0) {
    ;
  }

  // Send Device information.
  out_going.pkt.INFO[8] = RESTORE_DEVICES_TABLE;
  times = 0;
  while (1) {
    cp = (void*)dev_list.devices;
    bytes = bkup_devices * sizeof(DEVICE);
    if (bytes > 19) {
      len = 19;
      index = bytes / 19;
      if ((bytes % 19) == 0) {
        --index;
      }
    } else {
      len = bytes;
      index = 0;
    }

    for (i = 0; i <= (int)index; ++i) {
      out_going.pkt.INFO[11] = i;
      memcpy(&out_going.pkt.INFO[12], cp, len);
      out_going.length = 9 + 12 + len;

      if (send_packet(&out_going)) {
        break;
      }

      cp += len;
      if (bytes < 19) {
        len = bytes;
      }
      bytes -= len;
      if (bytes == 0) {
        break;
      }
    }

    if (bytes == 0) {
      break;
    }

    if (times > RETRY_COUNT) {
      return (1);
    }
  }

  ticks_set(2, 30L);
  while (ticks_check(2) == 0) {
    ;
  }

  // Send Event information.
  out_going.pkt.INFO[8] = RESTORE_EVENTS_TABLE;
  times = 0;
  while (1) {
    cp = (void*)events;
    bytes = bkup_events * sizeof(EVENT);
    if (bytes > 19) {
      len = 19;
      index = bytes / 19;
      if ((bytes % 19) == 0) {
        --index;
      }
    } else {
      len = bytes;
      index = 0;
    }

    for (i = 0; i <= (int)index; ++i) {
      out_going.pkt.INFO[11] = i;
      memcpy(&out_going.pkt.INFO[12], cp, len);
      out_going.length = 9 + 12 + len;

      if (send_packet(&out_going)) {
        break;
      }

      cp += len;
      if (bytes < 19) {
        len = bytes;
      }
      bytes -= len;
      if (bytes == 0) {
        break;
      }
    }

    if (bytes == 0) {
      break;
    }

    if (times > RETRY_COUNT) {
      return (1);
    }
  }
  return (0);
}

void ReadDevicesFromDisk(void) {
  struct _stat stat_buf;
  int dev_test;
  FILE* stream;
  int goop;

  // Read Customer table.
  dev_test = TableFromDisk(&cust_info,
                           sizeof(CUSTOMER),
                           "custinfo.dat");
  if ((dev_test == 0) && (DEMO == FALSE)) {
    device_chg_pc = 0;
    event_chg_pc = 0;
    mood_chg_pc = 0;
    trigger_chg_pc = 0;
    security_chg_pc = 0;
    return;
  }

  // get file dates from disk
  // if they don't exist, don't panic
  if (_stat("devices.dat", &stat_buf) == 0) {
    device_chg_pc = stat_buf.st_mtime;
  } else {
    device_chg_pc = 0;
  }

  if (_stat("events.dat", &stat_buf) == 0) {
    event_chg_pc = stat_buf.st_mtime;
  } else {
    event_chg_pc = 0;
  }

  if (_stat("moods.dat", &stat_buf) == 0) {
    mood_chg_pc = stat_buf.st_mtime;
  } else {
    mood_chg_pc = 0;
  }

  if (_stat("triggers.dat", &stat_buf) == 0) {
    trigger_chg_pc = stat_buf.st_mtime;
  } else {
    trigger_chg_pc = 0;
  }

  if (_stat("security.dat", &stat_buf) == 0) {
    security_chg_pc = stat_buf.st_mtime;
  } else {
    security_chg_pc = 0;
  }

  stream = fopen("SECURITY.DAT", "rb");
  if (stream != NULL) {
    fread(&goop, sizeof(int), 1, stream);
    fread(&security_device, sizeof(SECURITY_T), 1, stream);
    fclose(stream);
  } else {
    memset(&security_device, 0, sizeof(SECURITY_T));
  }

  //Read Device table
  dev_list.devices_used = TableFromDisk(&dev_list.devices,
                                        sizeof(DEVICE),
                                        "devices.dat");
  // Read Event table.
  events_used = TableFromDisk(&events,
                              sizeof(EVENT),
                              "events.dat");
  //read mood table
  active_moods = 0;
  moods_used = TableFromDisk(&mood_list,
                             sizeof(MOOD_T),
                             "moods.dat");
  //read triggers table
  triggers_used = TableFromDisk(&triggers,
                                sizeof(TRIGGER_T),
                                "triggers.dat");
}

void decompress(char* ocp, char far* icp, USHORT size) {
  int i;
  int j;
  USHORT* intp;
  USHORT osize;
  USHORT input_bytes;
  UCHAR imask;

  intp = (void*)icp; // get size of input from first 2 bytes
  input_bytes = *intp;
  icp += 2;  // point past first 2 bytes;
  osize = 0; // nothing in output buffer

  while (osize < size) {
    if ((osize + 8) < size) {
      memset(ocp, 0, 8);
    } else {
      memset(ocp, 0, size - osize); // don't go past output area's expected size
    }
    imask = *icp;
    ++icp;
    for (i = 0, j = 0x080; i < 8; ++i, j /= 2) {
      if ((imask & j) != 0) {
        *ocp = *icp;
        ++icp;
        ++ocp;
      } else {
        ++ocp;
      }
      ++osize;
      if (osize >= size) {
        break;
      }
    }
  }
  //position(30,1);
  //printf("Output size: %u;  Input bytes: %u\x007", size, input_bytes);
  //getch();
}

static int SendPassword(UCHAR* password) {
  int retry;
  PKT_INFO pkt_info;
  UCHAR statusline[15];

  pkt_info.pkt.CONTROL = 0x009;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;

  //memcpy(&pkt_info.pkt.INFO[0], "\x50\xD0\x3A", 3);
  pkt_info.pkt.INFO[0] = 0x50;
  pkt_info.pkt.INFO[1] = 0xD0 + (UCHAR)(running_sequence & 0x07);
  pkt_info.pkt.INFO[2] = 0x3A;
  memset(&pkt_info.pkt.INFO[3], 0, 8);
  memcpy(&pkt_info.pkt.INFO[3], password, 8);
  pkt_info.length = 20;

  for (retry = 0; retry < RETRY_COUNT; retry++) {
    if (send_packet(&pkt_info) == SUCCESS) {
      return SUCCESS;
    }
    sprintf(statusline, "SendPassword() - Retry %.2d", retry);
    //Status(statusline);
    ExceptionLog(statusline);
  }
  return FAILURE;
}

void ExceptionLog(char* description) {
  FILE* exceptionlog;
  struct tm* tod;

  time(&ltime);
  tod = localtime(&ltime);

  if ((exceptionlog = fopen("except.log", "at")) != NULL) {
    fprintf(exceptionlog, "%0.2d/%0.2d/%0.2d\t%0.2d:%0.2d:%0.2d\t%s\n",
            this_month, today, this_year,
            tod->tm_hour, tod->tm_min, tod->tm_sec,
            description);
    fclose(exceptionlog);
  }
}

//==========================================================================
// FetchFile()
//
// Routine History:
// Author               Date            Description
// GES                  02/02/98         Use new, improved protocol
//
//
//
// Inputs:
//  Name               Type            Description
//   fname               char *            points to the name of file to
//                                    ... get from the ICU
//
// Returns:
//  Name               Type            Description
//   return_code         int               returns the success status
//                                    0 = OK--complete, !0 = Failure
//
//
// Description:
//   Controls the overall logic of the file request/reception.
//  The new protocol has these steps:
//      1. To ICU:      Request file size (ICU stores name of file)
//      2. From ICU:   File size returned.
//      3. To ICU:      Request block of up to 16 packets be sent
//                  NOTE:   The request has the block number AND
//                        a bit mask of needed packets.  This allows
//                        requests for only 1, or a few, packets when
//                        they haven't been received from previous requests.
//      4. From ICU:   Requested packets are sent.  The bit-mask from the request
//                  is used to determine which packets will be sent.  When the
//                  bit mask goes to zero the ICU ends its send process.
//         NOTE:      The data is stored in a temporary file until successful
//                  completion of the entire transfer.
//================================================================================

long int FSize;
long int FPkts;

int FetchFile(char* fname) {
  USHORT rc;
  USHORT tries;
  char* dot;
  char backupfile[14];

  unlink("fetch.tmp");
  tries = 0;
  while (1) {
    if (RequestFSize(fname) != 0) {
      if ((++tries) < 100) {
        ce_bus_reset();
        ticks_set(5, 8L);
        while (ticks_check(5) == 0) {
          ;
        }
        continue;
      } else {
        return (1);
      }
    }
    if (rc = RecFileSize(&FSize)) { // Non-zero rc means there's a problem
      if (rc == 5) {
        //printf("File not found.\r\n");
        return (5);
      }
      if ((++tries) < 100) {
        continue;
      } else {
        return (1);
      }
    }

    if (rc = CollectFile(fname)) {
      return (1);
    } else {
      sprintf(backupfile, "%s", fname);
      dot = strstr(backupfile, ".");
      sprintf(&dot[1], "BAK");

      unlink(backupfile);
      rename(fname, backupfile);

      rename("fetch.tmp", fname);
      unlink("fetch.tmp");
      return (0);
    }
  }
}

long int RequestFSize(UCHAR* fname) {
  PKT_INFO pkt_info;

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8 + (UCHAR)((++running_sequence) & 0x007);
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;
  pkt_info.pkt.INFO[4] = 0x044;
  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = GET_FILE_SIZE;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;
  memcpy(&pkt_info.pkt.INFO[11], fname, 20);
  pkt_info.length = 20 + strlen(fname);
  return (SendPacket(&pkt_info));
}

long int RequestFBlk(int block, USHORT mask) {
  PKT_INFO pkt_info;

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8 + (UCHAR)((++running_sequence) & 0x007);
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;
  pkt_info.pkt.INFO[4] = 0x044;
  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = GET_FILE_BLOCK;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;
  memcpy(&pkt_info.pkt.INFO[11], (void*)&block, 2);
  memcpy(&pkt_info.pkt.INFO[13], (void*)&mask, 2);

  pkt_info.length = 24;

  return (SendPacket(&pkt_info));
}

//==========================================================================
// CollectFile()
//
// Routine History:
// Author               Date            Description
// Unknown               ??/??/??         .....................
// GES                  02/02/98         Use new, improved protocol
//
//
//
// Inputs:
//  Name               Type            Description
//   fname               char *            points to the name of file to
//                                    ... get from the ICU
//
// Returns:
//  Name               Type            Description
//   return_code         int               returns the success status
//                                    0 = OK--complete, !0 = Failure
//
//
// Description:
//   Controls the overall logic of the file request/reception.
//  The new protocol has these steps:
//      1. To ICU:      Request file size (ICU stores name of file)
//      2. From ICU:   File size returned.
//      3. To ICU:      Request block of up to 16 packets be sent
//                  NOTE:   The request has the block number AND
//                        a bit mask of needed packets.  This allows
//                        requests for only 1, or a few, packets when
//                        they haven't been received from previous requests.
//      4. From ICU:   Requested packets are sent.  The bit-mask from the request
//                  is used to determine which packets will be sent.  When the
//                  bit mask goes to zero the ICU ends its send process.
//         NOTE:      The data is stored in a temporary file until successful
//                  completion of the entire transfer.
//================================================================================

#define PKT_DATA_LEN 26
#define BLK_DATA_LEN (26 * 16)

UCHAR block_buffer[BLK_DATA_LEN];
time_t FFStartTime;

int CollectFile(char* fname) {
  //char   work[200];
  int i;
  int j;
  int number_blocks;
  int last_block_size;
  int cur_blk;
  int times;
  int rc;
  int cur_pkts;
  USHORT last_block_mask;
  USHORT mask;
  FILE* stream;
  PROGRESS_INDICATOR_T pi = {245 + 115, 220, 200, 0, FG_BLU, 0};

  PutIndicator(&pi);
  pi.progress = 0;
  UpdateIndicator(&pi);

  //   start_time = 0L;
  //   current_time = 0L;

  //   memset(st, 0, 80);
  time(&FFStartTime);

  number_blocks = (int)((FSize + BLK_DATA_LEN - 1) / (BLK_DATA_LEN));
  FPkts = (int)((FSize + PKT_DATA_LEN - 1) / (PKT_DATA_LEN));

  if ((FSize % (BLK_DATA_LEN)) != 0) {
    last_block_size = (int)(FSize % (BLK_DATA_LEN));
    for (i = 0, mask = 0x08000, last_block_mask = 0; i < 16; ++i, mask = mask >> 1) {
      j = i * (PKT_DATA_LEN);
      if (j >= last_block_size) {
        break;
      }
      last_block_mask |= mask;
    }
  } else {
    last_block_size = BLK_DATA_LEN;
    last_block_mask = 0x0FFFF;
  }

  for (cur_blk = 1; cur_blk <= number_blocks; ++cur_blk) {
    cur_pkts = 0; // pkt count inside of current block
    times = 0;
    if (cur_blk == number_blocks) {
      mask = last_block_mask;
    } else {
      mask = 0x0FFFF;
    }

    while (1) {
      //sprintf(work, "[%s] %d/%d", fname, cur_blk, number_blocks);
      if (RequestFBlk(cur_blk - 1, mask) != 0) {
        if ((++times) < 100) {
          ce_bus_reset();
          ticks_set(5, 8L);
          while (ticks_check(5) == 0) {
            ; // wait for time out
          }
          continue;
        } else {
          return (1);
        }
      }

      rc = RecFBlock(cur_blk - 1, &mask, &pi, &cur_pkts);
      if (rc != 0) {
        if (rc == 9) {
          msgbox("Data transfer canceled by user.", "Information", MB_OK);
          unlink("fetch.tmp");
          lv_stop();
        } else {
          RequestFSize(fname);
          if ((++times) > 100) {
            return (1);
          }
        }
      } else {
#if 0
            pi.progress = (int)((long)(cur_blk * 100L) / FPkts);
            UpdateIndicator(&pi);
            time(&current_time);
            elapsed = current_time - start_time;
            if (elapsed != 0)
            {
               total_time = elapsed * number_blocks;
               total_time /= cur_blk;
               time_left = total_time - elapsed;
            }
            if ((ULONG)(time_left / 60L) < 1000)
            {
               sprintf(st, "Time Left %02ld:%02ld  ",
                  (ULONG)(time_left / 60L),
                  (ULONG)(time_left) % 60L);
               ptext(st, 384, 260, BG_WHT + FG_BLK);
            }
#endif
        //Status(st);
        if (mask == 0) {
          stream = fopen("fetch.tmp", "ab");
          if (stream == NULL) {
            msgbox("Error opening file.", "Critical Error", MB_OK);
            ExceptionLog("Error opening FETCH.TMP");
            lv_stop();
          }
          if (cur_blk == number_blocks) {
            fwrite(block_buffer, 1, last_block_size, stream);
            fclose(stream);
            Status("");
            area_clear(384, 398, 260, 388, BG_WHT + FG_WHT);
            return (0);
          } else {
            fwrite(block_buffer, 1, BLK_DATA_LEN, stream);
            fclose(stream);
            break; //break out of while (this block) to outer "for" loop
          }
        }
      }
    }
  }
}

int RecFileSize(long int* fsize) {
  int result;
  PKT_INFO incoming_pkt;

  ticks_set(5, 55L); /* set 3 tick alarm */
  while (1) {
    result = CEBUS__GetPacket(&incoming_pkt);
    if (result == 0) {
      if (ticks_check(5)) {
        ce_bus_reset();
        return (1);
      }
      continue;
    } else if (result == CERROR) {
      ce_bus_reset();
      return (1);
    } else {
      ticks_set(5, 55L); // set 3 tick alarm
      switch (PacketType(&incoming_pkt)) {
      case INTERVAL_30S:
        process_interval(&incoming_pkt);
        continue;
      case INTERVAL_30S$:
        process_interval$(&incoming_pkt);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }
      if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
        continue;
      }
      if ((USHORT)(incoming_pkt.pkt.INFO[1] & 0x007) != (running_sequence & 0x007)) {
        continue;
      }
      if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && ((incoming_pkt.pkt.INFO[1] & 0x0D8) == 0x0D0)) {
        if (incoming_pkt.pkt.INFO[3] == 1) {
          return (5); // file not found
        } else {
          return (4); // some other problem...
        }
      }

      if (incoming_pkt.pkt.INFO[2] == 0x0FE) {
        memcpy((void*)fsize, &incoming_pkt.pkt.INFO[3], 4);
        return (0);
      }
      continue;
    }
  }
}

//==========================================================================
// int RecFBlock(int block_number, USHORT *packet_mask,
//            PROGRESS_INDICATOR_T *pi, int *cur_pkt_count)
//
// Routine History:
// Author               Date            Description
// GES                  02/02/98         Use new, improved protocol
//
//
//
// Inputs:
//  Name               Type            Description
//   block_number         int               block offset into the file
//    packet_mask         USHORT            mask of packets not yet received
//                     pointer            progress indicator structure
//                     int pointer         pkt count in current block
//
// Returns:
//  Name               Type            Description
//   packet_mask         USHORT            returns the updated mask of packets not yet received
//
//
// Description:
//
//================================================================================

int RecFBlock(int block_number, USHORT* pkt_mask, PROGRESS_INDICATOR_T* pi, int* cur_pkts) {
  int packet_number;
  int result;
  int mask_work;
  int progress;
  PKT_INFO incoming_pkt;
  time_t current_time;
  time_t time_left;
  time_t elapsed;
  time_t total_time;
  time_t last_current;
  UCHAR st[80];

  if (DEMO == TRUE) {
    return SUCCESS;
  }

  last_current = 0;
  ticks_set(5, 100L); /* set timer */
  while (1) {
    if (kbhit()) {
      if (kbchar() == ESC) {
        return (9);
      }
    }
    result = CEBUS__GetPacket(&incoming_pkt);
    if (result == 0) {
      if (ticks_check(5)) {
        ce_bus_reset();
        ticks_set(5, 8L);
        while (ticks_check(5) == 0) {
          ;
        }
        return (1);
      }
      continue;
    } else if (result == CERROR) {
      ce_bus_reset();
      ticks_set(5, 8L);
      while (ticks_check(5) == 0) {
        ;
      }
      return (1);
    } else {
      ticks_set(5, 100L); /* reset alarm */

      switch (PacketType(&incoming_pkt)) {
      case INTERVAL_30S:
        process_interval(&incoming_pkt);
        continue;
      case INTERVAL_30S$:
        process_interval$(&incoming_pkt);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }

      if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
        continue;
      }

      if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && ((incoming_pkt.pkt.INFO[1] & 0x0F8) == 0x0D0)) {
        if (incoming_pkt.pkt.INFO[3] == 1) {
          return (5); // file not found
        } else {
          return (1); // some other error
        }
      }

      if ((USHORT)(incoming_pkt.pkt.INFO[1] & 0x007) != (running_sequence & 0x007)) {
        continue;
      }

      incoming_pkt.pkt.INFO[1] &= 0x0E8;
      if ((memcmp(&incoming_pkt.pkt.INFO[0], "\x50\xE8\xFE", 3) == 0) && (memcmp((void*)&block_number, &incoming_pkt.pkt.INFO[3], 2) == 0)) {
        packet_number = incoming_pkt.pkt.INFO[5];
        memcpy(block_buffer + packet_number * PKT_DATA_LEN,
               &incoming_pkt.pkt.INFO[6], PKT_DATA_LEN);
        // take this packet out of packet mask
        mask_work = 0x08000 >> packet_number;
        mask_work = ~mask_work;
        (*pkt_mask) &= mask_work; // turn corresponding bit off

        ++(*cur_pkts);
        progress = (int)(((block_number * 16L + (*cur_pkts)) * 100L) / FPkts);
        if (pi->progress < progress) {
          pi->progress = progress;
          UpdateIndicator(pi);
        }

        time(&current_time);
        elapsed = current_time - FFStartTime;
        if (elapsed != 0) {
          total_time = (elapsed * FPkts) / ((long)block_number * 16L + (long)(*cur_pkts));
          time_left = total_time - elapsed;
          if (((time_left / 60L) < 1000L) && (current_time != last_current)) {
            //sprintf(st, "%ld, %ld",
            sprintf(st, "Time Left %02ld:%02ld  ",
                    time_left / 60L,
                    time_left % 60L);
            ptext(st, 384, 260, BG_WHT + FG_BLK);
          }
          last_current = current_time;
        }

        if ((*pkt_mask) == 0) {
          return (0);
        }
      }
    }
  }
}

int CheckEmail(void) {
  int read_flag;
  int got_mail = FALSE;
  FILE* f;
  //   MAIL_T   mail;

  //   memset(&mail, 0, sizeof(MAIL_T));
  read_flag = 0;
#if 0
   while (GetTable(&f_buffer,
      2048,
      EMAIL_TABLE,
      NULL,
      0,
      2,
      TRUE)
      == SUCCESS)
   {
      if ((f = fopen("email.db", "ab")) != NULL)
      {
         fwrite(&f_buffer, 1, 2048, f);
         fwrite(&read_flag, 1, 1, f);
         fclose(f);
         got_mail = TRUE;
      }
      else
      {
         msgbox("Can't save mail.      |"
            "Check your disk space.", "Error Saving E-Mail", MB_OK);
      }
      //      memset(&mail, 0, sizeof(MAIL_T));
   }
#else
  memset(f_buffer, ' ', 2048);
  while (FetchTable(&f_buffer,
                    2048,
                    EMAIL_TABLE,
                    NULL,
                    0,
                    2,
                    TRUE) == 0) {
    if ((f = fopen("email.db", "ab")) != NULL) {
      fwrite(&f_buffer, 1, 2048, f);
      fwrite(&read_flag, 1, 1, f);
      fclose(f);
      got_mail = TRUE;
    } else {
      msgbox("Can't save mail.      |"
             "Check your disk space.",
             "Error Saving E-Mail", MB_OK);
    }
    memset(f_buffer, ' ', 2048);
  }
#endif
  return (got_mail);
}

//==========================================================================
// FetchTable()
//
// Routine History:
// Author               Date         Description
// GES                  03/15/98      Use new, improved (RSTP) protocol
//                                 Table Transfer protocol
//                                 (RSTP: Rick Silva Transfer Protocol
// Inputs:
//  Name________________    Type______      Description__________
//  table_buf            char *         where to store the table
//   table_size            int            expected size of table
//   table_id            int            which table to read
//   request_data         char *         parameters to send in request...
//   request_len            int            length of parameter string
//   show_status            int            Not zero means show status of Xfer
//
// Returns:
//  Name               Type         Description
//   return_code         int            0 = OK -- xfer complete
//                                 1 == Requests timed out
//                                 10 No data for that day-UpdateHistoryFast()
//                                 11 No Email
//                                 12 DEVICE_HISTORY_TABLE: Wrong device type
//                                 13       "      bad hist bucket--memory corrupt.
//                                 14       "      device ID not found.
//                                 99 ??What Table
//
//
// Description:
//   Controls the overall logic of the table request/reception.
//  The new protocol has these steps:
//      1. To ICU:      Request table size (ICU stores the table number)
//      2. From ICU:   Table size returned.
//      3. To ICU:      Request block of up to 16 packets be sent
//                  NOTE:   The request has the block number AND
//                        a bit mask of needed packets.  This allows
//                        requests for only 1, or a few, packets when
//                        they haven't been received from previous requests.
//      4. From ICU:   Requested packets are sent.  The bit-mask from the request
//                  is used to determine which packets will be sent.  When the
//                  bit mask goes to zero the ICU ends its send process.
//         NOTE:      The data is stored in a temporary buffer until successful
//                  completion of the entire transfer.
//================================================================================

long int TableSize;

int FetchTable(void* table_buf, int table_size, int table_id, void* request_data,
               int req_length, int retries, int show_status) {
  USHORT rc;
  int tries;

  tries = 0;
  while (1) {
    if (RequestTSize(table_id, request_data, req_length) != 0) {
      if ((++tries) < 100) {
        ce_bus_reset();
        ticks_set(5, 8L);
        while (ticks_check(5) == 0) {
          ; // wait for time out
        }
        continue;
      } else {
        return (1);
      }
    }
    if (rc = RecTableSize(&TableSize)) { // Non-zero rc means there's a problem
      switch (rc) {
      case 10: // no data for that day
      case 11: // no email
      case 12: // DEVICE_HISTORY_TABLE:   wrong device type
      case 13: // DEVICE_HISTORY_TABLE:   bad bucket--corrupt memory
      case 14: // DEVICE_HISTORY_TABLE:   device id not found at all
      case 99: // Unsupported table requested
        return (rc);

      default:
        if ((++tries) < 100) {
          continue;
        } else {
          return (1); // general failure after 100 tries....
        }
      }
    }
    if (rc = CollectTable(table_buf, show_status)) {
      return (1);
    } else {
      return (0);
    }
  }
}

static long int RequestTSize(int table_id, UCHAR* request_data, int req_length) {
  PKT_INFO pkt_info;

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8 + (UCHAR)((++running_sequence) & 0x007);
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;
  pkt_info.pkt.INFO[4] = 0x044;
  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = GET_TABLE_SIZE;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;
  pkt_info.pkt.INFO[11] = table_id;
  if (req_length != 0) {
    memcpy(&pkt_info.pkt.INFO[12], request_data, req_length);
  }
  pkt_info.length = 21 + req_length;

  return (SendPacket(&pkt_info));
}

long int RequestTBlk(int block, USHORT mask) {
  PKT_INFO pkt_info;

  pkt_info.pkt.CONTROL = 9;
  pkt_info.pkt.SA = 1;
  pkt_info.pkt.SHC = cebusSetup.HouseCode;
  pkt_info.pkt.DA = 1;
  pkt_info.pkt.DHC = cebusSetup.OutsideHC;
  pkt_info.pkt.INFO[0] = 0x050;
  pkt_info.pkt.INFO[1] = 0x0E8 + (UCHAR)((++running_sequence) & 0x007);
  pkt_info.pkt.INFO[2] = 0x050;
  pkt_info.pkt.INFO[3] = 0x002;
  pkt_info.pkt.INFO[4] = 0x044;
  pkt_info.pkt.INFO[5] = 0x0F4;
  pkt_info.pkt.INFO[6] = 0x032;
  pkt_info.pkt.INFO[7] = 0x0F6;
  pkt_info.pkt.INFO[8] = GET_TABLE_BLOCK;
  pkt_info.pkt.INFO[9] = 0x020;
  pkt_info.pkt.INFO[10] = 0x0F5;
  memcpy(&pkt_info.pkt.INFO[11], (void*)&block, 2);
  memcpy(&pkt_info.pkt.INFO[13], (void*)&mask, 2);

  pkt_info.length = 24;

  return (SendPacket(&pkt_info));
}

//==========================================================================
// CollectTable()
//
// Routine History:
// Author               Date            Description
// GES                  03/15/98         Use new, improved protocol
//                                    Table Transfer protocol
//                                    (RSTP: Rick Silva Transfer Protocol
//
// Inputs:
//  Name               Type            Description
//   table_buf            char *            points to location to store table to
//   show_status            int               show status while collecting
//
// Returns:
//  Name               Type            Description
//   return_code         int               returns the success status
//                                    0 = OK--complete, !0 = Failure
//
//
// Description:
//   Controls the overall logic of the file request/reception.
//  The new protocol has these steps:
//      1. To ICU:      Request table size (ICU stores number of table
//                  and performs any necessary housekeeping)
//      2. From ICU:   Table size returned.
//      3. To ICU:      Request block of up to 16 packets be sent
//                  NOTE:   The request has the block number AND
//                        a bit mask of needed packets.  This allows
//                        requests for only 1, or a few, packets when
//                        they haven't been received from previous requests.
//      4. From ICU:   Requested packets are sent.  The bit-mask from the request
//                  is used to determine which packets will be sent.  When the
//                  bit mask goes to zero the ICU ends its send process.
//         NOTE:      The data is stored in a temporary buffer until successful
//                  completion of the entire transfer.
//================================================================================

#define PKT_DATA_LEN 26
#define BLK_DATA_LEN (26 * 16)

UCHAR Tblock_buffer[BLK_DATA_LEN];

int CollectTable(char* out_buf, int show_status) {
  //   char   work[200];
  int i;
  int j;
  int number_blocks;
  int last_block_size;
  int cur_blk;
  int times;
  int rc;
  int cur_pkts;
  USHORT last_block_mask;
  USHORT mask;
  PROGRESS_INDICATOR_T pi = {245 + 115, 220, 200, 0, FG_BLU, 0};
  UCHAR st[80];

  if (show_status != 0) {
    PutIndicator(&pi);
    pi.progress = 0;
    UpdateIndicator(&pi);
  }
  memset(st, 0, 80);
  number_blocks = (int)((TableSize + BLK_DATA_LEN - 1) / (BLK_DATA_LEN));
  cur_pkts = 0;

  if ((TableSize % (BLK_DATA_LEN)) != 0) {
    last_block_size = (int)(TableSize % (BLK_DATA_LEN));
    for (i = 0, mask = 0x08000, last_block_mask = 0; i < 16; ++i, mask = mask >> 1) {
      j = i * (PKT_DATA_LEN);
      if (j >= last_block_size) {
        break;
      }
      last_block_mask |= mask;
    }
  } else {
    last_block_size = BLK_DATA_LEN;
    last_block_mask = 0x0FFFF;
  }

  for (cur_blk = 1; cur_blk <= number_blocks; ++cur_blk) {
    cur_pkts = 0;
    times = 0;
    if (cur_blk == number_blocks) {
      mask = last_block_mask;
    } else {
      mask = 0x0FFFF;
    }

    while (1) {
      //sprintf(work, "[%s] %d/%d", fname, cur_blk, number_blocks);
      if (RequestTBlk(cur_blk - 1, mask) != 0) {
        if ((++times) > 100) {
          return (1);
        } else {
          ce_bus_reset();
          ticks_set(5, 8L);
          while (ticks_check(5) == 0) {
            ; // wait for time out
          }
          continue;
        }
      }

      rc = RecTBlock(cur_blk - 1, &mask, show_status, &pi, &cur_pkts);
      if (rc != 0) {
        if (rc == 9) {
          msgbox("Data transfer canceled by user.", "Information", MB_OK);
          lv_stop();
        } else {
          if ((++times) > 100) {
            return (1); // he did a reset
          }
          continue;
        }
      } else if (mask == 0) {
        if (cur_blk == number_blocks) {
          memcpy(out_buf + BLK_DATA_LEN * (cur_blk - 1), Tblock_buffer, last_block_size);
          Status("");
          area_clear(384, 398, 260, 388, BG_WHT + FG_WHT);
          return (0);
        } else {
          memcpy(out_buf + BLK_DATA_LEN * (cur_blk - 1), Tblock_buffer, BLK_DATA_LEN);
          times = 0;
          break; //break out of while (this block) to outer "for" loop
        }
      }
    }
  }
}

int RecTableSize(long int* tsize) {
  int result;
  PKT_INFO incoming_pkt;

  ticks_set(5, 55L); /* set 3 tick alarm */
  while (1) {
    result = CEBUS__GetPacket(&incoming_pkt);
    if (result == 0) {
      if (ticks_check(5)) {
        ce_bus_reset();
        return (1);
      }
      continue;
    } else if (result == CERROR) {
      ce_bus_reset();
      return (1);
    } else {
      ticks_set(5, 55L); // set 3 tick alarm
      switch (PacketType(&incoming_pkt)) {
      case INTERVAL_30S:
        process_interval(&incoming_pkt);
        continue;
      case INTERVAL_30S$:
        process_interval$(&incoming_pkt);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }
      if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
        continue;
      }
      if ((USHORT)(incoming_pkt.pkt.INFO[1] & 0x007) != (running_sequence & 0x007)) {
        continue;
      }
      if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && ((incoming_pkt.pkt.INFO[1] & 0x0F8) == 0x0D0)) {
        return (incoming_pkt.pkt.INFO[3]);
      }

      if (incoming_pkt.pkt.INFO[2] == 0x0FE) {
        memcpy((void*)tsize, &incoming_pkt.pkt.INFO[3], 4);
        return (0);
      }
      continue;
    }
  }
}

//==========================================================================
// int RecTBlock(int block_number, USHORT *packet_mask,
//                        int show_status, PROGRESS_INDICATOR_T *pi,
//                        int *cur_pkts)
//
// Routine History:
// Author               Date            Description
// GES                  02/02/98         Use new, improved protocol
//
//
//
// Inputs:
//  Name               Type            Description
//   block_number         int               block offset into the file
//    packet_mask         USHORT            mask of packets not yet received
//                     pointer            progress_indicator structure
//                     int pointer         packets received this block.
//
// Returns:
//  Name               Type            Description
//   packet_mask         USHORT            returns the updated mask of packets not yet received
//
//
// Description:
//
//================================================================================

int RecTBlock(int block_number,
              USHORT* pkt_mask,
              int show_status,
              PROGRESS_INDICATOR_T* pi,
              int* cur_pkts) {
  int packet_number;
  int result;
  int mask_work;
  int total_packets;
  //    int         pkt_count;
  int new_progress;
  PKT_INFO incoming_pkt;

  if (DEMO == TRUE) {
    return SUCCESS;
  }

  total_packets = (int)((TableSize + (PKT_DATA_LEN - 1)) / (PKT_DATA_LEN));
  if (total_packets == 0) {
    total_packets = 1; // don't "divide by zero"
  }

  ticks_set(5, 100L); /* set timer */
  while (1) {
    if (kbhit()) {
      if (kbchar() == ESC) {
        return (9);
      }
    }
    result = CEBUS__GetPacket(&incoming_pkt);
    if (result == 0) {
      if (ticks_check(5)) {
        ce_bus_reset();
        ticks_set(5, 8L);
        while (ticks_check(5) == 0) {
          ; // wait for time out
        }
        return (10);
      }
      continue;
    } else if (result == CERROR) {
      ce_bus_reset();
      ticks_set(5, 8L);
      while (ticks_check(5) == 0) {
        ; // wait for time out
      }
      return (11);
    } else {
      ticks_set(5, 100L); /* reset alarm */

      switch (PacketType(&incoming_pkt)) {
      case INTERVAL_30S:
        process_interval(&incoming_pkt);
        continue;
      case INTERVAL_30S$:
        process_interval$(&incoming_pkt);
        continue;
      case INTERVAL_15M:
        process_interval_15m((INTERVAL_DATA_15*)&incoming_pkt.pkt.INFO[6]);
        continue;
      case ALERT_MSG:
        continue;
      case NEW_RATE_MSG:
        continue;
      }
      if (incoming_pkt.pkt.SHC != (USHORT)cebusSetup.OutsideHC) {
        continue;
      }
      if ((incoming_pkt.pkt.INFO[2] == 0x0FD) && ((incoming_pkt.pkt.INFO[1] & 0x0D8) == 0x0D0)) {
        if (incoming_pkt.pkt.INFO[3] == 1) {
          return (5); // file not found
        } else {
          return (1); // some other error
        }
      }

      if ((USHORT)(incoming_pkt.pkt.INFO[1] & 0x007) != (running_sequence & 0x007)) {
        continue;
      }

      incoming_pkt.pkt.INFO[1] &= 0x0F8;

      if ((memcmp(&incoming_pkt.pkt.INFO[0], "\x50\xE8\xFE", 3) == 0) && (memcmp((void*)&block_number, &incoming_pkt.pkt.INFO[3], 2) == 0)) {
        packet_number = incoming_pkt.pkt.INFO[5];
        memcpy(Tblock_buffer + packet_number * PKT_DATA_LEN,
               &incoming_pkt.pkt.INFO[6], PKT_DATA_LEN);

        // take this packet out of packet mask
        mask_work = 0x08000 >> packet_number;
        mask_work = ~mask_work;
        (*pkt_mask) &= mask_work; // turn corresponding bit off

        // update progress indicator
        if (show_status != 0) {
          ++(*cur_pkts);
          new_progress = (int)(((block_number * 16 + (*cur_pkts)) * 100.0) / (double)total_packets);
          if (pi->progress != new_progress) {
            pi->progress = new_progress;
            UpdateIndicator(pi);
          }
        }

        if ((*pkt_mask) == 0) {
          return (0);
        }
      }
    }
  }
}
