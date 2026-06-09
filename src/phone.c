//==========================================================================
//   $Workfile:   phone.c  $
//   $Revision:   1.15  $
//
//   HISTORY:
//   Author            Date      Description
//--------------------------------------------------------------------------
//   UNK               UNK         CEBUS API Replacement for PSTN
//
//
//   DESCRIPTION
//
//==========================================================================
//   Copyright 1996 TeCom Inc. All rights reserved
//==========================================================================

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
#include <malloc.h>
#include "cebus.h"
#include "2box.h"
#include "msgbox.h"

#include "commlib.h" // GreenLeaf
#include "fast.h"
//#include "gsci.h"
//#include "modem.h"         // GreenLeaf
#include "inside.h"
#include "insubs.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "lwindow.h"
#include "inio.h"
#include "inutil.h"
#include "mouse.h"

//#define DEBUG_MODE 0
#define INIT_LEN 49
#define DIAL_LEN 45

#define CONNECT_TIMER 10

static void bin2mtel(UCHAR*, ULONG, int);
static ULONG mtel2long(UCHAR* cp, int length);
static void SetUpModem(void);
static void ShowCommError(int errorno, char visible);
static int WaitForString(int timeout, PORT* port, char* string);
static void SendString(PORT* port, char* string);
static int GetModemInput(PORT* port, char* string, int length, unsigned short timer, int dots);

extern int kb_stat(void);
extern int in_phone_view;
extern UCHAR longversion[];
extern char ANSWER;

extern struct CEBUSDATA {
  int IRQ;
  int HouseCode;
  int BasePort;
  int OutsideHC;
} cebusSetup;

PORT* gl_port;

struct FONEVIEW_CONFIG {
  char comm_port;
  char init_string[INIT_LEN];
  char dial_string[DIAL_LEN];
  long int port_speed;
  int dial_attempts;
  long int dial_timeout;
} foneview_cfg;

struct LVREMOTE_CONFIG {
  char comm_port;             //
  char init_string[INIT_LEN]; //
  char dial_string[DIAL_LEN]; //
  long int port_speed;        //
  int dial_attempts;          //
  long int dial_timeout;      //
  char irq;                   //
  USHORT addr;
  char future[126];
} lvremote_cfg;

static void timeout_error(char* cp) {
  msgbox(cp, "Modem error", MB_OK);
  SetDtr(gl_port, 0); // set DTR low
  PortClose(gl_port);
  cebus_found = FALSE;
  lv_stop(); // exit program
}

static void ShowCommError(int errorno, char visible) {
  //if visible = 1 then display

  char error[256];

  memset(error, 0, 256);
  switch (errorno) {
  case -1:
    sprintf(error, "General comm error\n");
    break;
  case -2:
    sprintf(error, "Invalid port\n");
    break;
  case -3:
    sprintf(error, "Port already in use\n");
    break;
  case -4:
    sprintf(error, "Invalid buffer size (port open)\n");
    break;
  case -5:
    sprintf(error, "Out of Memory\n");
    break;
  case -6:
    sprintf(error, "Port closed\n");
    break;
  case -7:
    sprintf(error, "Invalid argument\n");
    break;
  case -8:
    sprintf(error, "Receive buffer empty\n");
    break;
  case -9:
    sprintf(error, "Transmit buffer full\n");
    break;
  case -10:
    sprintf(error, "Time out\n");
    break;
  case -11:
    sprintf(error, "CTS expected\n");
    break;
  case -12:
    sprintf(error, "CD expected\n");
    break;
  case -13:
    sprintf(error, "DSR expected\n");
    break;
  case -14:
    sprintf(error, "UART not detected\n");
    break;
  case -16:
    sprintf(error, "User abort\n");
    break;
  case -19:
    sprintf(error, "WideTrack not available\n");
    break;
  case -20:
    sprintf(error, "Port already in use\n");
    break;
  case -22:
    sprintf(error, "No OK response from modem\n");
    break;
  case -23:
    sprintf(error, "No response from modem\n");
    break;
  case -24:
    sprintf(error, "Function not supported by driver\n");
    break;
  case -25:
    sprintf(error, "Illegal baud rate specified\n");
    break;
  case -26:
    sprintf(error, "Illegal parity specified\n");
    break;
  case -27:
    sprintf(error, "Illegal word length\n");
    break;
  case -28:
    sprintf(error, "Illegal stop bits\n");
    break;
  case -29:
    sprintf(error, "Embedded copyright violation\n");
    break;
  case -30:
    sprintf(error, "Driver software not detected\n");
    break;
  case -31:
    sprintf(error, "Buffer overflow\n");
    break;
  case -32:
    sprintf(error, "Network access failed\n");
    break;
  case -33:
    sprintf(error, "DOS extender reported error\n");
    break;
  case -34:
    sprintf(error, "Illegal port number on Multiport board\n");
    break;
  case -35:
    sprintf(error, "Multiport board already in use\n");
    break;
  case -36:
    sprintf(error, "Handshaking blocked transfer\n");
    break;
  case -38:
    sprintf(error, "Illegal IRQ\n");
    break;
  case -39:
    sprintf(error, "IRQ already in use\n");
    break;
  case -75:
    sprintf(error, "User defined error\n");
    break;
  }

  ExceptionLog(error);

  if (visible = 1) {
    msgbox(error, "Green Leaf Comm Error    ", MB_OK);
  }

  SetDtr(gl_port, 0); // set DTR low
  PortClose(gl_port);
  cebus_found = FALSE;
  lv_stop(); // exit program
}

void bin2mtel(UCHAR* cp, ULONG number, int length) {
  int i;

  memset(cp, ' ', length);
  for (i = length - 1; i >= 0; --i) {
    *(cp + i) = (UCHAR)(number % 95) + ' ';
    number /= 95;
    if (number == 0) {
      return;
    }
  }
  return;
}

ULONG mtel2long(UCHAR* cp, int length) {
  int i;
  ULONG ul;

  for (i = 0, ul = 0; i < length; ++i) {
    ul *= 95;
    ul += (*(cp + i) - ' ');
  }
  return (ul);
}

LWINDOW_T fv_Window[1] = {0, 0, 640, 480, //window position
                          NULL,           //screen object
                          NULL,
                          0,      //item count
                          0,      //cursor position 0-79 left to right
                          0,      //element that has current focus
                          fcsBtn, //focus type
                          longversion,
                          NULL, //OnKeyPress();
                          0};

LWINDOW_T fv1_Window[1] = {175, 160, 310, 115, //window position
                           NULL,               //screen object
                           NULL,
                           0,                     //item count
                           0,                     //cursor position 0-79 left to right
                           0,                     //element that has current focus
                           fcsBtn,                //focus type
                           "Connecting with ICU", // title
                           NULL,                  //OnKeyPress();
                           0};

//************************************************************************
//  CEBUS__InitPort() is called first -- must read config file, etc.
//************************************************************************
//  CEBUS__InitPort() opens the modem and dials the ICU
int CEBUS__InitPort(u_int16 port, u_int16 intnumber) {
  char* dial_string;
  char buff[80];
  char i = 0;
  char work[81];
  char dial_attempts = 0;
  int rc;
  long ticks = 0L;
  FILE* fstream;
  //   int       x = 0;
  int try_again_flag = FALSE;
  int rings = 0;
  int redial = FALSE;

  if (in_ce_bus_reset == TRUE) {
    return (1);
  }

  in_phone_view = TRUE;

  glbWindow = fv_Window;
  PutWindow(glbWindow);
  glbWindow = fv1_Window;
  Wait(10L);
  PutWindow(glbWindow);
  glbWindow = NULL;

  memset(&lvremote_cfg, 0, sizeof(struct LVREMOTE_CONFIG));
  memset(&foneview_cfg, 0, sizeof(struct FONEVIEW_CONFIG));

  ptext("Opening configuration file", 205, 175, BG_WHT + FG_BLK);

  fstream = fopen("lvremote.cfg", "rb");
  if (fstream != NULL) {
    fread(&lvremote_cfg, sizeof(struct LVREMOTE_CONFIG), 1, fstream);
    fclose(fstream);
  } else {
    SetUpModem();
  }

  area_clear(201, 233, 175, 467, FG_WHT);
  ptext("Opening comm port...", 205, 175, BG_WHT + FG_BLK);

  //GreenleafSetPortHardware(int port, int IRQ, int address);
  if (FastSetPortHardware(lvremote_cfg.comm_port - 1,
                          lvremote_cfg.irq,
                          lvremote_cfg.addr) != ASSUCCESS) {
    msgbox("Error setting up hardware.", "Opening comm port", MB_OK);
    SetDtr(gl_port, 0); // set DTR low
    PortClose(gl_port);
    cebus_found = FALSE;
    lv_stop(); // exit program
  }

  gl_port = PortOpenGreenleafFast((int)(lvremote_cfg.comm_port - 1),
                                  lvremote_cfg.port_speed,
                                  'N',
                                  8,
                                  1);
  Wait(18L);

  if (gl_port->status == ASSUCCESS) {
    SetRts(gl_port, 1);
    SetDtr(gl_port, 1);
    UseRtsCts(gl_port, 1); // use hardware flow control

    ticks_set(CONNECT_TIMER, (long)(lvremote_cfg.dial_timeout * 18));

    area_clear(201, 233, 175, 467, FG_WHT);
    ptext("Initializing port...", 205, 175, BG_WHT + FG_BLK);
    SendString(gl_port, lvremote_cfg.init_string);

    if (WaitForString(4, gl_port, "OK") == SUCCESS) {
      if (ANSWER == TRUE) {
        ptext("Waiting for call       ", 205, 175, BG_WHT + FG_BLK);
        Wait(24L);
        SendString(gl_port, "AT S0=1");
        ticks_set(3, 4 * 18L);
        if (WaitForString(4, gl_port, "OK") != SUCCESS) {
          msgbox("Ring count not accepted by modem", "", MB_OK);
        }
#if 0
            ticks_set(3, 2L);
#endif
        while (1) {
#if 0
               if (ticks_check(3))
               {
                  switch (x)
                  {
                  case 0:
                     ptext(".     ", 205, 303, BG_WHT + FG_BLK);
                     x++;
                     break;
                  case 1:
                     ptext("..    ", 205, 303, BG_WHT + FG_BLK);
                     x++;
                     break;
                  case 2:
                     ptext("...   ", 205, 303, BG_WHT + FG_BLK);
                     x++;
                     break;
                  case 3:
                     ptext("....  ", 205, 303, BG_WHT + FG_BLK);
                     x++;
                     break;
                  case 4:
                     ptext("..... ", 205, 303, BG_WHT + FG_BLK);
                     x = 0;
                     break;
                  }
                  ticks_set(3, 9L);
               }
#endif
          //               HMInputLine( gl_port, 1, work, 81 );
          rc = GetModemInput(gl_port, work, 81, 0x0FFFF, TRUE);
          if (rc == 27) {
            msgbox("Connect aborted by user.", "Waiting for call.", MB_OK);
            SetDtr(gl_port, 0); // set DTR low
            PortClose(gl_port);
            cebus_found = FALSE;
            lv_stop();         // exit program
          } else if (rc == -1) // timed out
          {
            break;
          }
          if (strstr(strupr(work), "RING") != NULL) {
            rings++;
            sprintf(buff, "RING %d", rings);
            ptext(buff, 221, 175, BG_WHT + FG_BLK);
          }
          if (strstr(strupr(work), "CONNECT") != NULL) {
            Status(work);
            Wait(36L);
            NotBusy();
            return 1;
          }
        }
      } else {
        //get number to dial
        fillz(buff, DIAL_LEN - 1, ' ');
        memcpy(buff, lvremote_cfg.dial_string, DIAL_LEN);
        trim(buff);
        dial_string = inputbox("Enter number to dial.", "Making Connection", buff);
        if (dial_string[0] == 0) {
          SetDtr(gl_port, 0); // set DTR low
          PortClose(gl_port);
          cebus_found = FALSE;
          lv_stop(); // exit program
        }

        Busy();
        strcpy(lvremote_cfg.dial_string, dial_string);
        trim(lvremote_cfg.dial_string);

        //for (dial_attempts = 0; dial_attempts < lvremote_cfg.dial_attempts; ++dial_attempts)
        for (dial_attempts = 0;; ++dial_attempts) {
          sprintf(work, "Dialing ICU: %s", lvremote_cfg.dial_string);
          area_clear(201, 233, 175, 467, FG_WHT);
          ptext(work, 205, 175, BG_WHT + FG_BLK);
          ptext("Press ESC to abort.", 225, 175, BG_WHT + FG_BLK);

          memset(work, 0, 81);
          strcpy(work, "ATDT");
          strcat(work, lvremote_cfg.dial_string);
          SendString(gl_port, work);

          while (ticks_check(CONNECT_TIMER) == 0) //wait for connect with remote ICU...
          {
            rc = GetModemInput(gl_port, work, 81, CONNECT_TIMER, FALSE);
            if (rc == 27) {
              msgbox("Connection aborted by user.", "Dialing ICU", MB_OK);
              Wait(30L);
              SetDtr(gl_port, 0); // set DTR low
              PortClose(gl_port);
              cebus_found = FALSE;
              lv_stop(); // exit program
            } else if (rc == -1) {
              break; // time expired
            }
            if (strstr(strupr(work), "BUSY") != NULL) {
              SetDtr(gl_port, 0); // set DTR low
              if (msgbox("Line returned busy signal. |"
                         "Try again?                 ",
                         "Dialing ICU",
                         MB_YES_NO) == MB_RESULT_YES) {
                redial = TRUE;
                lvremote_cfg.dial_attempts++;
                SetDtr(gl_port, 1); // set DTR True
                ticks_set(CONNECT_TIMER, (long)(lvremote_cfg.dial_timeout * 18));
                break;
                //continue;
              } else {
                SetDtr(gl_port, 0); // set DTR low
                PortClose(gl_port);
                cebus_found = FALSE;
                lv_stop(); // exit program
              }
            }
            if (strstr(work, "CONNECT") != NULL) //connection successful
            {
              //clean up??;
              Status(work);
              Wait(36L);
              NotBusy();
              return 1;
            }
          }
          if (redial == FALSE) {
            SetDtr(gl_port, 0); // set DTR low
            rc = msgbox("Connection error.        |"
                        "Do you want to try again?",
                        "Dialing ICU", MB_YES_NO);
            if (rc == MB_RESULT_NO) {
              break;
            } else {
              redial = TRUE;
              SetDtr(gl_port, 1); // set DTR True
              ticks_set(CONNECT_TIMER, (long)(lvremote_cfg.dial_timeout * 18));
            }
          }
        }
        msgbox("Timed out waiting for    |"
               "connection to remote ICU.",
               "Connection Failed", MB_OK);
        SetDtr(gl_port, 0); // set DTR low
        PortClose(gl_port);
        cebus_found = FALSE;
        lv_stop(); // exit program
      }
    } else {
      msgbox("Failed to initialize modem.", "LaneView Remote Setup", MB_OK);
      SetDtr(gl_port, 0); // set DTR low
      PortClose(gl_port);
      cebus_found = FALSE;
      lv_stop(); // exit program
    }
  } else {
    gl_port = NULL;
    sprintf(work, "Couldn't initialize port:  %d", lvremote_cfg.comm_port);
    msgbox(work, "Comm Port Error", MB_OK);
    cebus_found = FALSE;
    lv_stop(); // exit program
  }
}

void CEBUS__ClosePort(void) {
  if (in_ce_bus_reset == TRUE) {
    return;
  }

  SetDtr(gl_port, 0); // set DTR low
  PortClose(gl_port); // close comm port
  cebus_found = 0;
  return;
}

int send_modem(char* cp, int len) {
  int rc;
  //   int i=1, j;

  if (cebus_found == 0) {
    return (1);
  }

  // send len characters to comm port
  ticks_set(3, 54L); //set time out for clear to send (CTS)
  while (len) {
    if (IsTXEmpty(gl_port)) // is transmit buffer empty
    {
      if ((GetModemStatus(gl_port) & CTS_SET) == 0) // is CTS high ?
      {
        if (ticks_check(3)) {
          //clear to send set to low for too long
          timeout_error("CTS never set high...");
          SetDtr(gl_port, 0); // set DTR low
          PortClose(gl_port);
          cebus_found = FALSE;
          lv_stop(); // exit program
        } else {
          continue; // No ... dont send
        }
      }
      rc = WriteChar(gl_port, *cp); // Send a char to the port
      if (rc < ASSUCCESS) {
        if (gl_port->status == -36) {
          if (ticks_check(3)) {
            //clear to send set to low for too long
            timeout_error("CTS never set high...2...");
          }
          SetDtr(gl_port, 0); // set DTR low
          PortClose(gl_port);
          cebus_found = FALSE;
          lv_stop(); // exit program
                     //continue;
        }
        ShowCommError(gl_port->status, 1);
        return (1);
      }
      ticks_set(3, 54L);
      ++cp; //goto next char in the string
      --len;
    } else {
      if (ticks_check(3)) {
        // transmit buffer is not empty, cannot send data
        timeout_error("TXEmpty never set high...");
        SetDtr(gl_port, 0); // set DTR low
        PortClose(gl_port);
        cebus_found = FALSE;
        lv_stop(); // exit program
                   //return (1);
      }
    }
  }
  return (0);
}

void CEBUS__InitDevice(u_int16 shc, u_int16 sa, CEBUS_CONFIG* config) {
  if (in_ce_bus_reset == TRUE) {
    return;
  }
  return;
}

int CEBUS__GetPacket(PKT_INFO* pkt_info) {
  char work[80];
  int i, c;
  int j;
  ULONG sent_crc;
  ULONG calc_crc;

  if (IsRXEmpty(gl_port)) // check receive buffer
  {

#ifdef DEBUG_MODE
    position(1, 1);
    printf("CD=%02X", GetCd(gl_port));
#endif
    //receive buffer empty - return
    return (0);
  }

#ifdef DEBUG_MODE
  fillz(work, 79, ' ');
  position(24, 1);
  rev(work);
#endif

  i = 0;
  j = 0;
  ticks_set(3, 54L); // set char receive timeout to 10 seconds //54L

  while (1) {
    if (IsRXEmpty(gl_port)) // check receive buffer
    {
      //if(GetCd(gl_port) == 0)
      //   return(-1);
      if (ticks_check(3)) {
#ifdef DEBUG_MODE
        position(5, 20);
        for (j = 0; j < i; ++j) {
          position(23, j + 1);
          c = work[j];
          if (c == 0) {
            c = 237; // "NULL" character
          }
          putch(c);
        }
        printf("<<i=%d,CD=%02X", i, GetCd(gl_port));
        //putch(7);
        //getch();
#endif
        return (-1);
      }
      continue;
    }

    c = ReadChar(gl_port); // Greenleaf Comm function
    if (c < ASSUCCESS)     // some kind of error
    {
      ShowCommError(gl_port->status, 1);
      continue;
    }
    ticks_set(3, 54L); //54L

    if ((i == 0) && (c != 0x80)) {
      // wait for the 0x080
#ifdef DEBUG_MODE
      position(5, ++j);
      if (c == 0) {
        c = 237; // "NULL" character
      }
      printf("%c", c);
#endif
      continue;
    }

    work[i++] = c;
    if (i < 2) {
      continue; // need to get length yet.
    }

    if (work[1] > 42) {
      work[1] = 42;
    }

    if (i >= (2 + (work[1] - 9) + 6)) // if work[1] is < 9, he wouldn't have sent it.
    {
#ifdef DEBUG_MODE
      position(24, i);
      norm((char*)&c);
#endif
      // a crc check here
      sent_crc = mtel2long(work + 2 + work[1] - 9, 6);
      calc_crc = CalculateBlockCRC32(work[1] - 9 + 2, 0xFFFFFFFFL, work);
      if (sent_crc == calc_crc) {
        memcpy((char*)pkt_info->pkt.INFO, work + 2, work[1] - 9);
        pkt_info->length = work[1];
        pkt_info->pkt.SHC = cebusSetup.OutsideHC;
        pkt_info->pkt.SA = 1;
        pkt_info->pkt.DHC = cebusSetup.HouseCode;
        pkt_info->pkt.DA = 1;
        pkt_info->pkt.CONTROL = 0xFF;
        return (1);
      } else // failed CRC check
      {
        Status("Failed CRC check");
        return (0);
      }
    } else {
#ifdef DEBUG_MODE
      if (c == 0) {
        c = 237; // "NULL" character
      }
      position(24, i);
      norm((char*)&c);
#endif
    }
  }
}

//**********************************************************************************
// This function sandwhiches a CEBus packet header and CRCx32
// EX:   [80][len][CEBus PKT.INFO][CRCx32]
//**********************************************************************************
unsigned long CEBUS__SendPacket(PKT_INFO* pkt_info) {
  char msg[1 + 1 + 32 + 6 + 80]; //ICU_AT + Len + CEBus_PKT.INFO + CRC + extra
  ULONG crc;

  msg[0] = (UCHAR)0x080;     //get ICU's attention
  msg[1] = pkt_info->length; //

  if ((msg[1] - 9) < 0) {
    return (1L); // Invalid Packet?? tell him ok
  } else {
    memcpy(msg + 2, pkt_info->pkt.INFO, pkt_info->length - 9);
  }

  crc = CalculateBlockCRC32(2 + pkt_info->length - 9, 0xFFFFFFFFL, msg);
  bin2mtel(msg + 2 + pkt_info->length - 9, crc, 6);

  if (send_modem(msg, 2 + 6 + pkt_info->length - 9)) {
    return (0L);
  }
  return (1L);
}

int CEBUS__GetTransmitStatus(unsigned long id, XMIT_STAT* status) {
  *status = 2;
  return (2);
}

//=========================================================================================
//
//   static void SetUpModem(void)
//
//   INPUTS                  VARIABLE         TYPE
//   N/A                     N/A               N/A
//
//   SetUpModem automatically finds a good modem on the end user's PC.
//   It also prompts the end user for modem related info such as
//   dialup timeout, default phone number and modem init string.
//
//=========================================================================================
static void SetUpModem(void) {
  int i;
  //   int j;
  int flag = 0;
  char work[100];
  //   char mbuff[81];
  char* string_holder;
  //   char c;
  long ticks = 0L;
  FILE* fstream;
  int result;

  memset(work, 0, 100);

  fstream = fopen("foneview.cfg", "rb");
  if (fstream != NULL) {
    fread(&foneview_cfg, sizeof(struct FONEVIEW_CONFIG), 1, fstream);
    fclose(fstream);
    result = msgbox("Do you want to use your old|FoneView configuration with the new|LaneView Remote?  If FoneView has been|working, answer YES", "FoneView to LV Remote", MB_YES_NO);
    if (result == MB_RESULT_YES) {
      memcpy(&lvremote_cfg, &foneview_cfg, sizeof(struct FONEVIEW_CONFIG));
      switch (lvremote_cfg.comm_port) {
      case 1:
        lvremote_cfg.irq = 4;
        lvremote_cfg.addr = 0x3F8;
        break;
      case 3:
        lvremote_cfg.irq = 4;
        lvremote_cfg.addr = 0x3E8;
        break;
      case 2:
        lvremote_cfg.irq = 3;
        lvremote_cfg.addr = 0x2F8;
        break;
      case 4:
        lvremote_cfg.irq = 3;
        lvremote_cfg.addr = 0x2E8;
        break;
      }
      fstream = fopen("lvremote.cfg", "wb");
      if (fstream != NULL) {
        fwrite(&lvremote_cfg, sizeof(struct LVREMOTE_CONFIG), 1, fstream);
        fclose(fstream);
        unlink("foneview.cfg");
        msgbox("LaneView Remote is setup and|ready to make its first call.||Press OK at the next|dialog box to dial.", "LaneView Remote Setup", MB_OK);
      }
    }
  } else {
    result = MB_RESULT_NO;
  }
  if (result == MB_RESULT_NO) {
    foneview_cfg.comm_port = -1;
    sprintf(foneview_cfg.init_string, "%s", "AT&F");
    sprintf(foneview_cfg.dial_string, "(xxx) xxx-xxxx");
    foneview_cfg.port_speed = 19200;
    foneview_cfg.dial_attempts = 2;
    foneview_cfg.dial_timeout = 45;

    msgbox("LaneView Remote will now search|for modems and ask a few questions|before making its first call.|For beginners, we recommend|using the default values.",
           "LaneView Remote Setup", MB_OK);

    area_clear(201, 233, 175, 467, FG_WHT);
    ptext("Searching for modem...", 205, 175, BG_WHT + FG_BLK);

    Wait(9L);

    for (i = 0; i < 4; i++) {
      sprintf(work, "Checking comm port %d ", i + 1);
      ptext(work, 219, 175, BG_WHT + FG_BLK);
      Wait(9L);
      gl_port = PortOpenGreenleafFast((int)(i), 2400, 'N', 8, 1);
      if (gl_port->status == ASSUCCESS) {
        ptext("Polling modem.        ", 219, 175, BG_WHT + FG_BLK);
        Wait(9L);
        SendString(gl_port, "AT");
        ticks_set(CONNECT_TIMER, 5 * 18L);
        if (WaitForString(5, gl_port, "OK") == SUCCESS) {
          flag = 1; // found a good modem
        } else {
          flag = 2; // no modem found
        }
        PortClose(gl_port);
        if (flag == 1) {
          sprintf(work, "Found modem at comm port %d|Use this modem?", i + 1);
          if (msgbox(work, "LaneView Remote Setup", MB_YES_NO) == MB_RESULT_YES) {
            foneview_cfg.comm_port = i + 1;
            break;
          }
        }
      }
    }
    if (foneview_cfg.comm_port == -1) {
      msgbox("No modems found on your PC.|Check your PC and run|lvrsetup.exe manually.", "LaneView Remote Setup", MB_OK);
      SetDtr(gl_port, 0); // set DTR low
      PortClose(gl_port);
      cebus_found = FALSE;
      lv_stop(); // exit program
    } else {
      lvremote_cfg.comm_port = foneview_cfg.comm_port;
      switch (lvremote_cfg.comm_port) {
      case 1:
        lvremote_cfg.irq = 4;
        lvremote_cfg.addr = 0x3F8;
        break;
      case 3:
        lvremote_cfg.irq = 4;
        lvremote_cfg.addr = 0x3E8;
        break;
      case 2:
        lvremote_cfg.irq = 3;
        lvremote_cfg.addr = 0x2F8;
        break;
      case 4:
        lvremote_cfg.irq = 3;
        lvremote_cfg.addr = 0x2E8;
        break;
      }
      sprintf(work, "%d", lvremote_cfg.irq);

      string_holder = inputbox("Enter the comm port's IRQ.", "LaneView Remote Setup", work);
      if (string_holder[0] != 0) {
        lvremote_cfg.irq = atoi(string_holder);
        string_holder = inputbox("Enter modem init string.", "LaneView Remote Setup", foneview_cfg.init_string);
        if (string_holder[0] != 0) {
          strncpy(lvremote_cfg.init_string, string_holder, INIT_LEN);
          string_holder = inputbox("Enter default phone number.", "LaneView Remote Setup", foneview_cfg.dial_string);
          if (string_holder[0] != 0) {
            strncpy(lvremote_cfg.dial_string, string_holder, DIAL_LEN);
            sprintf(work, "%d", foneview_cfg.port_speed);
            string_holder = inputbox("Enter default connection speed.", "LaneView Remote Setup", work);
            if (string_holder[0] != 0) {
              lvremote_cfg.port_speed = atoi(string_holder);
              sprintf(work, "%d", foneview_cfg.dial_attempts);
              string_holder = inputbox("Enter number of dialup attempts.", "LaneView Remote Setup", work);
              if (string_holder[0] != 0) {
                lvremote_cfg.dial_attempts = atoi(string_holder);
                sprintf(work, "%d", foneview_cfg.dial_timeout);
                string_holder = inputbox("Enter dailup timeout value (seconds).", "LaneView Remote Setup", work);
                if (string_holder[0] != 0) {
                  lvremote_cfg.dial_timeout = atoi(string_holder);
                  fstream = fopen("lvremote.cfg", "wb");
                  if (fstream != NULL) {
                    fwrite(&lvremote_cfg, sizeof(struct LVREMOTE_CONFIG), 1, fstream);
                    fclose(fstream);
                    unlink("foneview.cfg");
                    msgbox("LaneView Remote is setup and|ready to make its first call.||Press OK at the next|dialog box to dial.", "LaneView Remote Setup", MB_OK);
                  } else {
                    msgbox("Cannot write LaneView Remote|Configuration to disk.|Check disk space & try again.", "Disk Access Error", MB_OK);
                    SetDtr(gl_port, 0); // set DTR low
                    PortClose(gl_port);
                    cebus_found = FALSE;
                    lv_stop(); // exit program
                  }
                } else {
                  SetDtr(gl_port, 0); // set DTR low
                  PortClose(gl_port);
                  cebus_found = FALSE;
                  lv_stop(); // exit program
                }
              } else {
                SetDtr(gl_port, 0); // set DTR low
                PortClose(gl_port);
                cebus_found = FALSE;
                lv_stop(); // exit program
              }
            } else {
              SetDtr(gl_port, 0); // set DTR low
              PortClose(gl_port);
              cebus_found = FALSE;
              lv_stop(); // exit program
            }
          } else {
            SetDtr(gl_port, 0); // set DTR low
            PortClose(gl_port);
            cebus_found = FALSE;
            lv_stop(); // exit program
          }
        } else {
          SetDtr(gl_port, 0); // set DTR low
          PortClose(gl_port);
          cebus_found = FALSE;
          lv_stop(); // exit program
        }
      } else {
        SetDtr(gl_port, 0); // set DTR low
        PortClose(gl_port);
        cebus_found = FALSE;
        lv_stop(); // exit program
      }
    }
  }
}

//=========================================================================================
//
//   WaitForString()
//
//   INPUTS                  VARIABLE         TYPE
//   time to wait in seconds      timeout            int
//   comm port to check         port            PORT
//   string to wait for         string            char *
//
//
//   WaitForString waits for a string from the modem.
//
//=========================================================================================
static int WaitForString(int timeout, PORT* port, char* string) {
  int j;
  char portbuffer[100];
  char c;

  memset(portbuffer, 0, sizeof(portbuffer));
  c = 0;
  j = 0;

  ticks_set(3, timeout * 18L);

  while (1) {
    if (ticks_check(3)) // timed out
    {
      return FAILURE;
    }
    if (!IsRXEmpty(port)) {
      c = ReadChar(port);
      if (c == 0x00A) {
        continue; // throw out Linefeeds
      }
      if (c == 0x00D) {
        memset(portbuffer, 0, sizeof(portbuffer));
        c = 0;
        j = 0;
        continue; // keep waiting for another string
      }
      portbuffer[j++] = c;
      if (j > 99) {
        return FAILURE;
      }
    } else {
      continue;
    }
    if (strstr(strupr(portbuffer), strupr(string))) // the modem returned the string we want
    {
      //         printf("%s\n",portbuffer);
      return SUCCESS;
    }
  }
}

//=========================================================================================
//
//   SendString()
//
//   INPUTS                  VARIABLE         TYPE
//   comm port to send to      port            PORT
//   string to send            string            char *
//
//
//   SendString sends a string to the specified port and appends at CR.
//
//=========================================================================================
static void SendString(PORT* port, char* string) {
  int i;
  int c;

  for (i = 0; i < (int)strlen(string); i++) {
    c = WriteChar(port, string[i]); // Send a char to the port
    if (c < ASSUCCESS) {
      ShowCommError(gl_port->status, 1);
      SetDtr(gl_port, 0); // set DTR low
      PortClose(gl_port);
      cebus_found = FALSE;
      lv_stop(); // exit program
    }
  }
  c = WriteChar(port, 13);
  //   printf("%s\n",string);
}

//=========================================================================================
//
//   GetModemInput()
//
//   INPUTS                  VARIABLE      TYPE
//   comm port to check         port         PORT
//   string to write to         string         char *
//   maximum length of string   length         int
//   timer number for time-outs   timer         USHORT
//   display dots while waiting   dots         TRUE/FALSE
//
//
//   GetModemInput gets a string from the modem and puts it
//   into the user specified char pointer.
//
//   The User is responsible for making sure that length value
//   is correct.  If it's not, the supplied char array will
//   be written beyond its limits.  This would be bad.
//
//=========================================================================================
int GetModemInput(PORT* port, char* string, int length, unsigned short timer, int dots) {
  int j;
  int x;
  char c;

  c = 0;
  j = 0;
  x = 0;

  memset(string, 0, length);
  //ticks_set(4, timeout * 18L);

  if (dots) {
    ticks_set(3, 2L);
  }

  while (1) {
    if (kb_stat() != 0) {
      c = kbchar();
      if (c == 0) {
        kbchar();
        continue;
      } else if (c == 27) // ESC
      {
        return (27);
      }
    }
    if (timer != 0x0FFFF) {
      if (ticks_check(timer) != 0) {
        return (-1);
      }
    }
    if (!IsRXEmpty(port)) {
      c = ReadChar(port);
      if (c == 0x00A) {
        continue;
      }
      if (c == 0x00D) {
        return (0);
      }
      string[j++] = c;
      if (j > length - 1) {
        return (0);
      }
    }
    if (dots) {
      if (ticks_check(3)) {
        switch (x) {
        case 0:
          ptext(".     ", 205, 303, BG_WHT + FG_BLK);
          x++;
          break;
        case 1:
          ptext("..    ", 205, 303, BG_WHT + FG_BLK);
          x++;
          break;
        case 2:
          ptext("...   ", 205, 303, BG_WHT + FG_BLK);
          x++;
          break;
        case 3:
          ptext("....  ", 205, 303, BG_WHT + FG_BLK);
          x++;
          break;
        case 4:
          ptext("..... ", 205, 303, BG_WHT + FG_BLK);
          x = 0;
          break;
        }
        ticks_set(3, 9L);
      }
    }
  }
}
