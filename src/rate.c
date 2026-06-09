#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <malloc.h>

#include "cebus.h"
#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "dropdown.h"
#include "listbox.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
//#include "moodmenu.h"
#include "frame_3d.h"
#include "devcat.h"
#include "ctrlschd.h"
#include "device.h"
#include "dvc_evnt.h"
#include "msgbox.h"
#include "checkbox.h"
#include "insubs.h"
#include "inio.h"
#include "inutil.h"
#include "rate.h"
#include "calendar.h"

extern LWINDOW_T progWindow[1];
extern struct CEBUSDATA cebusSetup;

int read_rates(int month, int day, int year, DAYS_RATES* dr) {
  UCHAR work[20];
  UCHAR errmsg[100];
  int leap, result = 0;
  ULONG lpos;
  FILE* stream;
  LWINDOW_T* tmpWindow;
  //char   *tmptitle = {"Reading Rate File"};

  memset(dr, 0, sizeof(DAYS_RATES));
  if ((year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }
  if (month == 0) {
    month = 12;
    --year;
  }
  if (year < 100) {
    sprintf(work, "RT%02d%02d", month, year);
  } else {
    sprintf(work, "RT%02d%04d", month, year + 1900);
  }

  tmpWindow = NULL;
  if ((stream = fopen(work, "rb")) == NULL) {
    tmpWindow = glbWindow;
    if (glbWindow != progWindow) {
      glbWindow = progWindow;
      //glbWindow->title = tmptitle;
      PutWindow(glbWindow);
    }
    area_clear(201 + 115, 233 + 115, 175, 467, FG_WHT);
    ptext("Reading Rate Files", 205 + 115, 175, BG_WHT + FG_BLK);
    sprintf(errmsg, "Reading File: %s", work);
    ptext(errmsg, 219 + 115, 175, BG_WHT + FG_BLK);

    result = read_file(work);
    if (result) // if fail
    {
      sprintf(errmsg, "Error reading file: %s from ICU.|Error number: %d", work, result);
      msgbox(errmsg, "Warning", MB_OK);
      ExceptionLog(errmsg);
      if (tmpWindow != NULL) {
        glbWindow = tmpWindow;
        PutWindow(glbWindow);
      }
      return result; // return fail
    }
    if ((stream = fopen(work, "rb")) == NULL) {
      sprintf(errmsg, "Error reading file: %s|Error number: %d", work, result);
      msgbox(errmsg, "Warning", MB_OK);
      ExceptionLog(errmsg);
      if (tmpWindow != NULL) {
        glbWindow = tmpWindow;
        PutWindow(glbWindow);
      }
      return 9; // return fail
    }
  }

  if (day <= day_tab[leap][month]) {
    lpos = (day - 1) * sizeof(DAYS_RATES);
  } else {
    lpos = (day_tab[leap][month] - 1) * sizeof(DAYS_RATES);
  }

  fseek(stream, lpos, SEEK_SET);
  fread(dr, sizeof(DAYS_RATES), 1, stream);
  fclose(stream);
  if (tmpWindow != NULL) {
    glbWindow = tmpWindow;
    PutWindow(glbWindow);
  }
  return 0;
}
