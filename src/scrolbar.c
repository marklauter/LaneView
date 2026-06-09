//scroll bar
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "2box.h"
#include "vga.h"
#include "scrnobjt.h"
#include "frame_3d.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "mloop.h"
#include "scrolbar.h"
#include "lwindow.h"

static UCHAR btntxt[4][2] = {
    0x9C, 0x00, // down
    0x9D, 0x00, // up
    0x9E, 0x00, // left
    0x9F, 0x00  // right
};

static int sb_cursor_pos;
void DisplayScrollBar(SCROLL_BAR_T* sb) {

  if (sb->max <= sb->min) {
    sb->max = sb->min + 1;
  }
  if ((sb->thumb_pos > sb->max) || (sb->thumb_pos < sb->min)) {
    sb->thumb_pos = sb->min;
  }

  if (sb->type == SB_HORIZONTAL) {
    sb_cursor_pos = sb->so.position.left +
                    (int)((sb->so.position.width - 37) *
                          (sb->thumb_pos / (double)(sb->max - sb->min))) +
                    14;
    // bar
    pbutton(sb->so.position.top,
            sb->so.position.left,
            sb->so.position.height,
            sb->so.position.width,
            PB_DN,
            FG_WHT);
    // thumb button
    pbutton(sb->so.position.top + 2,
            sb->so.position.left + (int)((sb->so.position.width - 37) * (sb->thumb_pos / (double)(sb->max - sb->min))) + 14,
            sb->so.position.height - 3,
            10,
            PB_UP,
            BG_WHT + FG_WHT);
    // right button
    pbutton(sb->so.position.top + 2,
            sb->so.position.left + sb->so.position.width - 13,
            sb->so.position.height - 3,
            12,
            PB_UP,
            BG_WHT + FG_WHT);
    ptext(btntxt[3],
          sb->so.position.top + 3,
          sb->so.position.left + sb->so.position.width - 10,
          BG_TRANSPARENT + FG_BLK);

    // left button
    pbutton(sb->so.position.top + 2,
            sb->so.position.left + 2,
            sb->so.position.height - 3,
            12,
            PB_UP,
            BG_WHT + FG_WHT);
    ptext(btntxt[2],
          sb->so.position.top + 3,
          sb->so.position.left + 3,
          BG_TRANSPARENT + FG_BLK);
  } else {
    sb_cursor_pos = sb->so.position.top +
                    (int)((sb->so.position.height - 37) *
                          (sb->thumb_pos / (double)(sb->max - sb->min))) +
                    14;

    // bar
    pbutton(sb->so.position.top,
            sb->so.position.left,
            sb->so.position.height,
            sb->so.position.width,
            PB_DN,
            FG_WHT);
    // thumb button
    pbutton(sb->so.position.top + (int)((sb->so.position.height - 37) * (sb->thumb_pos / (double)(sb->max - sb->min))) + 14,
            sb->so.position.left + 2,
            10,
            sb->so.position.width - 3,
            PB_UP,
            BG_WHT + FG_WHT);
    // bottom button
    pbutton(sb->so.position.top + sb->so.position.height - 13,
            sb->so.position.left + 2,
            12,
            sb->so.position.width - 3,
            PB_UP,
            BG_WHT + FG_WHT);
    ptext(btntxt[0],
          sb->so.position.top + sb->so.position.height - 13,
          sb->so.position.left + 5,
          BG_TRANSPARENT + FG_BLK);

    // top button
    pbutton(sb->so.position.top + 2,
            sb->so.position.left + 2,
            12,
            sb->so.position.width - 3,
            PB_UP,
            BG_WHT + FG_WHT);
    ptext(btntxt[1],
          sb->so.position.top + 1,
          sb->so.position.left + 5,
          BG_TRANSPARENT + FG_BLK);
  }
}

void ScrollBarOnClick(SCROLL_BAR_T* sb) {
  COORDINATE_T pos;

  if (sb->type == SB_HORIZONTAL) {
    pos.top = sb->so.position.top;
    pos.width = 12;
    pos.height = sb->so.position.height;

    // check left button
    pos.left = sb->so.position.left + 2;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      if (sb->thumb_pos > sb->min) {
        sb->thumb_pos -= sb->small_chg;
      }
      (*sb->OnScroll)(SCROLL_LEFT, SCROLL_SMALL, sb->thumb_pos);
      DisplayScrollBar(sb);
      return;
    }

    // check right button
    pos.left = sb->so.position.left + sb->so.position.width - 13;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      if (sb->thumb_pos < sb->max) {
        sb->thumb_pos += sb->small_chg;
      }
      (*sb->OnScroll)(SCROLL_RIGHT, SCROLL_SMALL, sb->thumb_pos);
      DisplayScrollBar(sb);
      return;
    }
    // check bar
    pos.left = sb->so.position.left;
    pos.width = sb->so.position.width;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      if ((pos.left + (int)((sb->so.position.width - 37) * (sb->thumb_pos / (double)(sb->max - sb->min))) + 14) > mouse_info.x) {
        if (sb->thumb_pos - sb->large_chg > sb->min) {
          sb->thumb_pos -= sb->large_chg;
        } else {
          sb->thumb_pos = sb->min;
        }
        (*sb->OnScroll)(SCROLL_RIGHT, SCROLL_LARGE, sb->thumb_pos);
      } else {
        if (sb->thumb_pos + sb->large_chg < sb->max) {
          sb->thumb_pos += sb->large_chg;
        } else {
          sb->thumb_pos = sb->max;
        }
        (*sb->OnScroll)(SCROLL_LEFT, SCROLL_LARGE, sb->thumb_pos);
      }
      DisplayScrollBar(sb);
      return;
    }
  } else {
    pos.left = sb->so.position.left;
    pos.width = sb->so.position.width;
    pos.height = 12;

    // check top button
    pos.top = sb->so.position.top + 2;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      if (sb->thumb_pos > sb->min) {
        sb->thumb_pos -= sb->small_chg;
      }
      (*sb->OnScroll)(SCROLL_UP, SCROLL_SMALL, sb->thumb_pos);
      DisplayScrollBar(sb);
      return;
    }

    // check bottom button
    pos.top = sb->so.position.top + sb->so.position.height - 13;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      if (sb->thumb_pos < sb->max) {
        sb->thumb_pos += sb->small_chg;
      }
      (*sb->OnScroll)(SCROLL_DOWN, SCROLL_SMALL, sb->thumb_pos);
      DisplayScrollBar(sb);
      return;
    }

    // check bar
    pos.top = sb->so.position.top;
    pos.height = sb->so.position.height;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      if ((pos.top + (int)((sb->so.position.height - 37) * (sb->thumb_pos / (double)(sb->max - sb->min))) + 14) > mouse_info.y) {
        if (sb->thumb_pos - sb->large_chg > sb->min) {
          sb->thumb_pos -= sb->large_chg;
        } else {
          sb->thumb_pos = sb->min;
        }
        (*sb->OnScroll)(SCROLL_UP, SCROLL_LARGE, sb->thumb_pos);
      } else {
        if (sb->thumb_pos + sb->large_chg < sb->max) {
          sb->thumb_pos += sb->large_chg;
        } else {
          sb->thumb_pos = sb->max;
        }
        (*sb->OnScroll)(SCROLL_DOWN, SCROLL_LARGE, sb->thumb_pos);
      }
      DisplayScrollBar(sb);
      return;
    }
  }
}

void ScrollBarOnMouseDown(SCROLL_BAR_T* sb) {
  COORDINATE_T pos;

  sb->so.got_focus = TRUE;
  DisplayScrollBar(sb);

  if (sb->type == SB_HORIZONTAL) {
    pos.top = sb->so.position.top;
    pos.width = 12;
    pos.height = sb->so.position.height;

    // check left button
    pos.left = sb->so.position.left + 2;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      // left button
      pbutton(sb->so.position.top + 2,
              sb->so.position.left + 2,
              sb->so.position.height - 3,
              12,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(btntxt[2],
            sb->so.position.top + 4,
            sb->so.position.left + 4,
            BG_TRANSPARENT + FG_BLK);
    }

    // check right button
    pos.left = sb->so.position.left + sb->so.position.width - 13;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      // right button
      pbutton(sb->so.position.top + 2,
              sb->so.position.left + sb->so.position.width - 13,
              sb->so.position.height - 3,
              12,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(btntxt[3],
            sb->so.position.top + 4,
            sb->so.position.left + sb->so.position.width - 9,
            BG_TRANSPARENT + FG_BLK);
    }
  } else {
    pos.left = sb->so.position.left;
    pos.width = sb->so.position.width;
    pos.height = 12;

    // check top button
    pos.top = sb->so.position.top + 2;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      // top button
      pbutton(sb->so.position.top + 2,
              sb->so.position.left + 2,
              12,
              sb->so.position.width - 3,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(btntxt[1],
            sb->so.position.top + 2,
            sb->so.position.left + 6,
            BG_TRANSPARENT + FG_BLK);
    }

    // check bottom button
    pos.top = sb->so.position.top + sb->so.position.height - 13;
    if ((MouseOver(&pos) == TRUE) && (sb->OnScroll != NULL)) {
      // bottom button
      pbutton(sb->so.position.top + sb->so.position.height - 13,
              sb->so.position.left + 2,
              12,
              sb->so.position.width - 3,
              PB_DN,
              BG_WHT + FG_WHT);
      ptext(btntxt[0],
            sb->so.position.top + sb->so.position.height - 12,
            sb->so.position.left + 6,
            BG_TRANSPARENT + FG_BLK);
    }
  }
}

void ScrollBarOnMouseUp(SCROLL_BAR_T* sb) {
}

void BlinkScrollBar(void) {
  static UCHAR sb_cursor;

  if ((glbWindow->so[glbWindow->focus.current]->type != SCROLL_BARV) ||
      (glbWindow->so[glbWindow->focus.current]->type != SCROLL_BARH) ||
      (glbWindow->so[glbWindow->focus.current]->visible == FALSE)) {
    return;
  }
  ticks_set(BlinkTimer, BLINK_RATE);
  if (sb_cursor) //is cursor on
  {
    sb_cursor = 0; // turn cursor off
    if (glbWindow->so[glbWindow->focus.current]->type == SCROLL_BARH) {
      area_clear(glbWindow->so[glbWindow->focus.current]->position.top + 4,
                 glbWindow->so[glbWindow->focus.current]->position.top + glbWindow->so[glbWindow->focus.current]->position.height - 4,
                 sb_cursor_pos + 2,
                 sb_cursor_pos + 7,
                 FG_WHT);
    }
  } else {
    if (glbWindow->so[glbWindow->focus.current]->type == SCROLL_BARH) {
      sb_cursor = 1; // turn cursor on
      area_clear(glbWindow->so[glbWindow->focus.current]->position.top + 4,
                 glbWindow->so[glbWindow->focus.current]->position.top + glbWindow->so[glbWindow->focus.current]->position.height - 4,
                 sb_cursor_pos + 2,
                 sb_cursor_pos + 7,
                 FG_GRY);
    }
  }
}
