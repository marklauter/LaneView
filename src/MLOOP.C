// ==========================================================================
// $Workfile:   MLOOP.C  $
// $Revision:   1.16  $
//
// HISTORY:
// Author            Date              Description
// --------------------------------------------------------------------------
// Mark Lauter         Nov 14, 1996      Necessary for emain entry
//
//
// DESCRIPTION
// ==========================================================================
//   Copyright 1996-1998 TeCom Inc. All rights reserved
// ==========================================================================

/*
$Log:   J:/intrlane/src/laneview/vcs/MLOOP.C_V  $
 *
 *    Rev 1.16   May 01 1998 13:47:58   tmges
 * 1.  Mark's changes to support attaching
 *      a scroll bar to other items. i.e,
 *      Memoboxes & Listboxes.
*/

#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>

#include "cebus.h"
#include "2box.h"
#include "inio.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "scrolbar.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "mloop.h"
#include "ismenu.h"
#include "msgbox.h"
#include "dropdown.h"
#include "inutil.h"

int IsWindowsRunning(void) {
  union REGS regs;

  regs.x.ax = 0x1600;
  int86(0x2F, &regs, &regs);
  if (regs.x.ax == 0x1600) {
    return FALSE;
  }
  return TRUE;
}
static void KeyLog(int key);
extern void UpdateTop(long* last_shown);
extern int kb_stat(void);
extern void demo(void);
extern int kbchar(void);
extern LWINDOW_T IntroWindow[1];

extern long ltime;
extern int im_busy;
extern struct tm* tod;
extern CURSOR_DEF_T arrow_ms;
extern CURSOR_DEF_T finger_ms;
extern CURSOR_DEF_T bar_ms;
extern int which_mouse;

FILE* fhelp;
UCHAR lhelp[50];
UCHAR syscom[100];

SCROLL_BAR_T* sbptr;

int item_index = 0; //item counter
int last_index = -1;

void MainLoop(void) {
  long last_shown = 0;
  int mouse_down = -1; // receives the item index
  int mouse_down2 = -1;
  int c;
  int do_it;
  //   int      changed_cursor;
  //   char   tmp[255];
  int scrollbar_return_index = 0;

  DROPDOWN_BOX_T* ddb;

  which_mouse = M_ARROW;

  ticks_set(BlinkTimer, BLINK_RATE); //cursor blinks every 1.5 seconds
  ticks_set(9, 6L);

  while (1) {
    mouse_data(&mouse_info); //get mouse info and store it in mouse_info

    if (ticks_check(BlinkTimer)) {
      BlinkCursor(); // blink text box cursor
                     //BlinkScrollBar();
    }

    show_mouse(); //do the smiley eye balls =)

    if (im_busy == FALSE) {
      //         changed_cursor = FALSE;

      do_it = FALSE;
      if (last_index == -1) {
        do_it = TRUE;
      } else if (!((MouseOver(&glbWindow->so[last_index]->position) == TRUE) &&
                   (glbWindow->so[last_index]->visible == TRUE))) {
        do_it = TRUE;
      }

      if (do_it) {
        if ((MouseOver(&glbWindow->so[item_index]->position) == TRUE) &&
            (glbWindow->so[item_index]->visible == TRUE)) {
          //               changed_cursor = TRUE;
          if ((glbWindow->so[item_index]->type != TEXT_BOX) && (glbWindow->so[item_index]->type != MEMO_BOX)) {
            if (which_mouse != M_FINGER) {
              which_mouse = M_FINGER;
              mouse_cursor(&finger_ms);
            }
          } else {
            if (which_mouse != M_TEXTBAR) {
              which_mouse = M_TEXTBAR;
              mouse_cursor(&bar_ms);
            }
          }
          last_index = item_index;
        } else
        //            if (changed_cursor == FALSE)//change to arrow
        {
          if (which_mouse != M_ARROW) {
            which_mouse = M_ARROW;
            last_index = -1;
            mouse_cursor(&arrow_ms);
          }
        }
      }
      //if left mouse button pressed
      if (mouse_info.buttons == 1) {
        if (mouse_down == -1) {
          if ((MouseOver(&glbWindow->so[item_index]->position) == TRUE) &&
              (glbWindow->so[item_index]->OnMouseDown != NULL) &&
              (glbWindow->so[glbWindow->focus.current]->DisplayObject != NULL) &&
              (glbWindow->so[item_index]->visible == TRUE)) {
            if (item_index != glbWindow->focus.current) {
              if (glbWindow->so[item_index]->type != SCROLL_BARV) {
                glbWindow->so[glbWindow->focus.current]->got_focus = FALSE;
                (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
              } else {
                sbptr = (SCROLL_BAR_T*)glbWindow->so[item_index];
                if (sbptr->attached_object == 0) {
                  glbWindow->so[glbWindow->focus.current]->got_focus = FALSE;
                  (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
                }
              }
            }
            if ((glbWindow->so[item_index]->type != SCROLL_BARH) &&
                (glbWindow->so[item_index]->type != SCROLL_BARV) &&
                (glbWindow->so[item_index]->type != TIME_ENTRY)) {
              mouse_down = item_index;
              glbWindow->focus.current = item_index;
              if (glbWindow->so[glbWindow->focus.current]->OnMouseDown != NULL) {
                (*glbWindow->so[glbWindow->focus.current]->OnMouseDown)(glbWindow->so[glbWindow->focus.current]); // call the object's mouse down event handler
              }
            } else {
              if (glbWindow->so[glbWindow->focus.current]->OnMouseDown != NULL) {
                //scrollbar_return_index = glbWindow->focus.current;
                mouse_down2 = item_index;
                glbWindow->focus.current = item_index;
                if (mouse_down2 != -1) {
                  (*glbWindow->so[glbWindow->focus.current]->OnMouseDown)(glbWindow->so[glbWindow->focus.current]); // call the object's mouse down event handler
                }
                if ((MouseOver(&glbWindow->so[glbWindow->focus.current]->position)) &&
                    (glbWindow->so[glbWindow->focus.current]->OnClick != NULL) &&
                    ticks_check(9)) {
                  // call selected object's OnClick() event
                  (*glbWindow->so[glbWindow->focus.current]->OnClick)(glbWindow->so[glbWindow->focus.current]);
                  ticks_set(9, 2L);
                }
              }
            }
          }
        } else //mouse was pressed over an object, but now the mouse (still pressed) was moved off the object
        {
          if ((MouseOver(&glbWindow->so[glbWindow->focus.current]->position) == FALSE) &&
              (glbWindow->so[glbWindow->focus.current]->OnMouseUp != NULL)) {
            mouse_down = -1;
            // call the object's MouseUp() event handler
            (*glbWindow->so[glbWindow->focus.current]->OnMouseUp)(glbWindow->so[glbWindow->focus.current]);
          }
        }
      }

      //if mouse button is released after being pressed while over a screen object
      if (mouse_info.buttons == 0) {
        if ((glbWindow->so[glbWindow->focus.current]->type != SCROLL_BARH) &&
            (glbWindow->so[glbWindow->focus.current]->type != SCROLL_BARV) &&
            (glbWindow->so[glbWindow->focus.current]->type != TIME_ENTRY)) {
          if (mouse_down != -1) {
            mouse_down = -1;
            //make sure mouse is over screen object
            if ((MouseOver(&glbWindow->so[glbWindow->focus.current]->position)) &&
                (glbWindow->so[glbWindow->focus.current]->OnClick != NULL)) {
              // call selected object's OnClick() event
              (*glbWindow->so[glbWindow->focus.current]->OnClick)(glbWindow->so[glbWindow->focus.current]);
            }
          }
        } else {
          if (mouse_down2 != -1) {
            mouse_down2 = -1;
            //make sure mouse is over screen object
            //sbt_ptr = (SCROLL_BAR_T *)*(glbWindow->so[glbWindow->focus.current]);
            if (glbWindow->so[glbWindow->focus.current]->OnMouseUp != NULL) {
              (*glbWindow->so[glbWindow->focus.current]->OnMouseUp)(glbWindow->so[glbWindow->focus.current]);
              sbptr = (SCROLL_BAR_T*)glbWindow->so[glbWindow->focus.current];
              if ((glbWindow->so[glbWindow->focus.current]->type == SCROLL_BARV) &&
                  (sbptr->attached_object != 0)) {
                glbWindow->so[glbWindow->focus.current]->got_focus = FALSE;
                (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
                glbWindow->focus.current = sbptr->attached_object - 1;
                if (glbWindow->so[glbWindow->focus.current]->got_focus != TRUE) {
                  glbWindow->so[glbWindow->focus.current]->got_focus = TRUE;
                  (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
                }
                //Status("Kilroy was here");
              }
              //else
              //{
              //Status("");
              //}
            }
          }
        }
      }
    }

    CheckCEBus();

    time(&ltime);
    tod = localtime(&ltime);
    if (MainLoopOpt) {
      if (last_shown != ltime) {
        UpdateTop(&last_shown);
      }
    }

    /*
         This next section filters key strokes.

         "system" keystrokes are handled here

         "window" keystrokes are passed to the
            current window's OnKeyPress() event
            handler

         "object" keystrokes are passed by the
            current window's OnKeyPress() event
            handler to the object's OnKeyPress()
            event handler
      */
    if (kb_stat()) {
      c = kbchar();
      KeyLog(c);
      switch (c) {
      case 9: // tab
        TabRight();
        break;
      case 27: //ESC
        if (glbWindow == IntroWindow) {
          GetPassword();
        }
        //               else
        //               {
        //                  IntroScreen();
        //               }
        break;
      case 0:
        c = kbchar();
        KeyLog(c);
        switch (c) {
        case 68: // F10
          lv_stop();
        case 15: //shift tab
          TabLeft();
          break;
        case 59: // F1 key (help)
          if (IsWindowsRunning() == TRUE) {
            fhelp = fopen("lvhlp.dat", "rb");
            if (fhelp != NULL) {
              hide_mouse();
              memset(lhelp, 0, 50);
              memset(syscom, 0, 100);
              fread(lhelp, sizeof(char), 50, fhelp);
              sprintf(syscom, "winhelp ", lhelp);
              strcat(syscom, lhelp);
              hide_mouse();
              system(syscom);
              unhide_mouse();
              fclose(fhelp);
            } else {
              msgbox("MS Windows must be running|to support this function.", "Help Error", MB_OK);
            }
          }
          break;
        default:
          unkbchar(c);
          if ((glbWindow->OnKeyPress != NULL) &&
              (glbWindow->so[glbWindow->focus.current]->visible == TRUE)) {
            (*glbWindow->OnKeyPress)(glbWindow->so[glbWindow->focus.current], 0);
          }
        }
        break;
      case CR: // carriage return
      case 32: // SPACE
      default: // call current window's OnKeyPress() event handler
        if ((glbWindow->OnKeyPress != NULL) &&
            (glbWindow->so[glbWindow->focus.current]->visible == TRUE)) {
          (*glbWindow->OnKeyPress)(glbWindow->so[glbWindow->focus.current], c);
        }
        break;
      }
    }
    if (glbWindow->so[item_index]->type != DROP_DOWN_BOX) {
      if (++item_index > glbWindow->item_count - 1) {
        item_index = 0;
      }
    } else {
      ddb = (DROPDOWN_BOX_T*)(glbWindow->so[item_index]);
      if (ddb->dropped == FALSE) {
        if (++item_index > glbWindow->item_count - 1) {
          item_index = 0;
        }
      }
    }
  }
}

void TabRight(void) {
  TEXT_BOX_T* tb;
  glbWindow->so[glbWindow->focus.current]->got_focus = FALSE;
  (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
  if (glbWindow->focus.current < glbWindow->item_count - 1) //its ok to inc focus.current
  {
    while (1) {
      ++glbWindow->focus.current;
      if (glbWindow->focus.current == glbWindow->item_count) {
        glbWindow->focus.current = 0;
      }
      if (glbWindow->so[glbWindow->focus.current]->visible == TRUE) {
        break;
      }
    }
  } else //reached end of items..... reset to first item
  {
    glbWindow->focus.current = 0;
    while (1) {
      if (glbWindow->so[glbWindow->focus.current]->visible == TRUE) {
        break;
      }
      ++glbWindow->focus.current;
      if (glbWindow->focus.current == glbWindow->item_count - 1) //went through all items and none were visible
      {
        return;
      }
    }
  }
  glbWindow->so[glbWindow->focus.current]->got_focus = TRUE;
  if ((glbWindow->so[glbWindow->focus.current]->type == TEXT_BOX) &&
      (over_write == TRUE)) {
    tb = (TEXT_BOX_T*)glbWindow->so[glbWindow->focus.current];
    glbWindow->cursor_pos = tb->col - 1;
  }
  (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
}

void TabLeft(void) {
  TEXT_BOX_T* tb;
  glbWindow->so[glbWindow->focus.current]->got_focus = FALSE;
  (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
  if (glbWindow->focus.current > 0) {
    while (1) {
      --glbWindow->focus.current;
      if (glbWindow->so[glbWindow->focus.current]->visible == TRUE) {
        break;
      }
      if (glbWindow->focus.current == 0) {
        glbWindow->focus.current = glbWindow->item_count - 1;
      }
    }
  } else {
    glbWindow->focus.current = glbWindow->item_count - 1;
    while (1) {
      if (glbWindow->so[glbWindow->focus.current]->visible == TRUE) {
        break;
      }
      --glbWindow->focus.current;
      if (glbWindow->focus.current == 0) //went through all items and none were visible
      {
        return;
      }
    }
  }
  glbWindow->so[glbWindow->focus.current]->got_focus = TRUE;
  if ((glbWindow->so[glbWindow->focus.current]->type == TEXT_BOX) &&
      (over_write == TRUE)) {
    tb = (TEXT_BOX_T*)glbWindow->so[glbWindow->focus.current];
    glbWindow->cursor_pos = tb->col - 1;
  }
  (*glbWindow->so[glbWindow->focus.current]->DisplayObject)(glbWindow->so[glbWindow->focus.current]);
}

static void KeyLog(int key) {
  FILE* fbtnlog;
  struct tm* tod;

  time(&ltime);
  tod = localtime(&ltime);

  if ((fbtnlog = fopen("user.log", "at")) != NULL) {
    if ((key > 31) && (key < 127)) {
      fprintf(fbtnlog, "%0.2d/%0.2d/%0.2d\t%0.2d:%0.2d:%0.2d\tKEY: %d\t\t%c\t\tWINDOW: %s\n",
              this_month, today, this_year,
              tod->tm_hour, tod->tm_min, tod->tm_sec,
              key, key,
              glbWindow->title);
    } else {
      fprintf(fbtnlog, "%0.2d/%0.2d/%0.2d\t%0.2d:%0.2d:%0.2d\tKEY: %d\t\t\t\tWINDOW: %s\n",
              this_month, today, this_year,
              tod->tm_hour, tod->tm_min, tod->tm_sec,
              key,
              glbWindow->title);
    }
    fclose(fbtnlog);
  } else {
    printf("Error opening userlog.dat");
  }
}

int tb_cursor = 1; // 1 = cursor blink on, 0 = cursor blink off
extern void MemoBlink(void);
void BlinkCursor(void) {
  if (glbWindow->so[glbWindow->focus.current]->type == MEMO_BOX) {
    MemoBlink();
  }
  if ((glbWindow->so[glbWindow->focus.current]->type != TEXT_BOX) ||
      (glbWindow->so[glbWindow->focus.current]->visible == FALSE) ||
      (glbWindow->so[glbWindow->focus.current]->position.top == 0)) {
    return;
  }
  hide_mouse();
  if (tb_cursor) //is cursor on
  {
    tb_cursor = 0; // turn cursor off
    vline((glbWindow->cursor_pos * 8) - 1,
          glbWindow->so[glbWindow->focus.current]->position.top + 1,
          glbWindow->so[glbWindow->focus.current]->position.top +
              glbWindow->so[glbWindow->focus.current]->position.height - 2,
          FG_BLK);
  } else {
    tb_cursor = 1; // turn cursor on
    vline((glbWindow->cursor_pos * 8) - 1,
          glbWindow->so[glbWindow->focus.current]->position.top + 1,
          glbWindow->so[glbWindow->focus.current]->position.top +
              glbWindow->so[glbWindow->focus.current]->position.height - 2,
          FG_HWHT);
  }
  unhide_mouse();
  ticks_set(BlinkTimer, BLINK_RATE);
}
