#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <malloc.h>
#include <sys\stat.h>
#include <graph.h>

#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
#include "copyrite.h"
#include "textbox.h"
#include "pshbtn.h"
#include "msgbox.h"
#include "inio.h"

char clear_attrib = 0x070;
char copyright_text[] = "Copyright ~ TeCom Inc. 1992-1998.";
char all_rights_reserved[] = "All rights reserved.";
UCHAR version[] = "4.4.0";
UCHAR longversion[] = "LaneView Version 4.4.0";
UCHAR about_title[] = "InterLane^ Home Manager Rel. 1.6.0";

static void crBtnOkClick(void);

//Copyright Screen
void copyright(void) {
  int* iptr;

  show_time = FALSE;
  MainLoopOpt = 0;
  ticks_set(7, 2L); //show mouse timer

  //   return;
  GetResource("INTLANE.LGO", f_buffer);
  iptr = (void*)f_buffer;
  hide_mouse();
  show_logo(*(iptr + 1) - 14, // upleft row
            *(iptr + 2),
            *(iptr + 3),
            *(iptr + 4),
            f_buffer + 10);
  unhide_mouse();

  position(31, (80 - strlen(copyright_text)) / 2);
  text(copyright_text, BG_WHT + FG_BLK);
  position(32, (80 - strlen(all_rights_reserved)) / 2);
  text(all_rights_reserved, BG_WHT + FG_BLK);
}
