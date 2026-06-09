// ==========================================================================
// $Workfile:   MemoBox.C  $
// $Revision:   1.4  $
//
// HISTORY:
// Author            Date              Description
// --------------------------------------------------------------------------
// Mark Lauter         March 11, 1998      Necessary for emain entry
//
//
// DESCRIPTION
// ==========================================================================
//   Copyright 1998 TeCom Inc. All rights reserved
// ==========================================================================

/*
$Log:   J:/intrlane/src/laneview/vcs/MemoBox.C_v  $
 *
 *    Rev 1.4   May 01 1998 13:46:34   tmges
 * 1.  Many changes as a part of the original
 *      creation of this object.
 *
 *    Rev 1.3   Apr 22 1998 17:12:32   tmges
 * Changes to clean-up the entry of
 * messages and cursor handling and
 * display updates.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#include "2box.h"
#include "vga.h"
#include "frame_3d.h"
#include "inutil.h"
#include "scrnobjt.h"
#include "pshbtn.h"
#include "memobox.h"
#include "mouse.h"
#include "lwindow.h"
#include "mloop.h"
#include "msgbox.h"
#include "textbox.h"

void out(MEMOBOX_T* memo);

static void DeleteKey(MEMOBOX_T* memo);
static void BackSpace(MEMOBOX_T* memo);
static void OverWriteChar(MEMOBOX_T* memo, int c);
static void InsertChar(MEMOBOX_T* memo, int c);
static int FindAWord(MEMOBOX_T* memo, int ndx, int width);
static void EraseCursor(MEMOBOX_T* memo);
static void DrawCursor(MEMOBOX_T* memo);

void MemoBoxScroll(MEMOBOX_T* memo, int direction) {
}

extern int tb_cursor;

void EraseCursor(MEMOBOX_T* memo) {
  int cr;

  if (memo->enabled == TRUE) {
    cr = memo->cursor_row - memo->display_top;
    vline((memo->cursor_col * 8) + memo->so.position.left + 4,
          memo->so.position.top + 5 + (cr * 16) + 1,
          memo->so.position.top + 5 + (cr * 16) + 12,
          FG_HWHT);
    ticks_set(BlinkTimer, BLINK_RATE);
    tb_cursor = FALSE;
  }
}

void DrawCursor(MEMOBOX_T* memo) {
  int cr;

  if (memo->enabled == TRUE) {
    cr = memo->cursor_row - memo->display_top;
    vline((memo->cursor_col * 8) + memo->so.position.left + 4,
          memo->so.position.top + 5 + (cr * 16) + 1,
          memo->so.position.top + 5 + (cr * 16) + 12,
          FG_BLK);
    ticks_set(BlinkTimer, BLINK_RATE);
    tb_cursor = TRUE;
  }
#if 0
   if (memo->sbobj != NULL)
   {
      memo->sbobj->thumb_pos =
         (int)(((long)(memo->cursor_row) * (long)memo->sbobj->max) / (long)(memo->rows));
      DisplayScrollBar(memo->sbobj);
   }
#endif
}

void MemoDisplayLine(MEMOBOX_T* memo, int current_line, int line_len,
                     int t, int l, int w, int refresh_next,
                     UCHAR bg_color, UCHAR* text_line, int text_left,
                     int display_lines) {
  int old_length;
  int cl;

  cl = current_line - memo->display_top;

  if (cl >= display_lines) {
    return; // past visible area
  } else if (current_line < memo->display_top) {
    return; // before visible area
  }

  old_length = memo->line_length[current_line]; // Previous length of current line

  if ((current_line < ((int)(memo->cursor_row))) && (line_len != old_length)) {
    area_clear(t + 3 + (cl * 16),
               t + 18 + (cl * 16),
               l + 2,
               l + w - 2,
               bg_color >> 4);
    ptext(text_line, (t + 5) + (cl * 16),
          text_left, bg_color + FG_BLK);
  } else if (current_line == ((int)(memo->cursor_row))) {
    area_clear(t + 3 + (cl * 16),
               t + 18 + (cl * 16),
               l + 2,
               l + w - 2,
               bg_color >> 4);
    ptext(text_line, (t + 5) + (cl * 16),
          text_left, bg_color + FG_BLK);
    //msgbox("Case 2","Debugging", MB_OK);
  } else if (
      (current_line > ((int)(memo->cursor_row))) && ((line_len != old_length) || refresh_next)) {
    area_clear(t + 3 + (cl * 16),
               t + 18 + (cl * 16),
               l + 2,
               l + w - 2,
               bg_color >> 4);
    ptext(text_line, (t + 5) + (cl * 16),
          text_left, bg_color + FG_BLK);
    //msgbox("Case 3","Debugging", MB_OK);
  }
}

void MemoRefreshAll(MEMOBOX_T* memo) {
  int i;

  for (i = 0; i < 256; ++i) {
    memo->line_start[i] = 0;
    memo->line_length[i] = -1;
  }
}

void MemoBoxPaint(MEMOBOX_T* memo) {
  int t;
  int l;
  int h;
  int w;

  //char tmp[100];
  //char tmp2[100];

  UCHAR text_line[100];

  int current_byte;
  int word_len;
  int line_len;

  int text_left;

  int display_cols;
  int display_lines;
  int current_line;
  int refresh_next;

  UCHAR bg_color;

  if (memo->so.visible == TRUE) {
    t = memo->so.position.top;
    l = memo->so.position.left;
    h = memo->so.position.height;
    w = memo->so.position.width;

    text_left = l + 5;

    display_lines = (h - 8) / 16;
    memo->display_height = display_lines;
    display_cols = (w - 8) / 8;

    current_line = 0;
    current_byte = 0;

    hide_mouse();
    if (memo->enabled == TRUE) {
      bg_color = BG_HWHT;
    } else {
      bg_color = BG_WHT;
    }
    pbutton_nc(t, l, h, w, PB_DN, bg_color);

    if (memo->length == 0) {
      memo->length = strlen(memo->text);
    }
    if (memo->length == 0) {
      if (((l + w - 2) - (l + 2)) > 1) {
        area_clear(t + 2, t + h - 2, l + 2, l + w - 2, bg_color >> 4);
      }
      unhide_mouse();
      return;
    }

    line_len = 0;
    memset(text_line, 0, 100);

    refresh_next = FALSE;
    word_len = -1;
    //      while(   (current_line < display_lines) &&   // don't exceed the display hght
    //            (word_len != 0) )
    while ((word_len != 0) && (current_line < MEMO_MAX_LINES)) {
      //find a word of text before adding it to the line of text...
      word_len = FindAWord(memo, current_byte, display_cols);

      // next, see if the word can be added to the current line of text
      // if not, then print the current line and add the word to the next line
      if (word_len > 0) // No CR....
      {
        // see if the word length + the line length exceeds avaiable columns
        if ((line_len + word_len - 1) < display_cols) {
          // add the word to the line
          memcpy(&text_line[line_len], &memo->text[current_byte], word_len);
        } else // print what we've got and start over
        {
          // print the current line
          MemoDisplayLine(memo,
                          current_line,
                          line_len,
                          t, l, w, refresh_next,
                          bg_color,
                          text_line,
                          text_left,
                          display_lines);

          memo->line_length[current_line] = line_len;
          if (current_byte != memo->line_start[current_line + 1]) {
            refresh_next = TRUE;
          } else {
            refresh_next = FALSE;
          }
          memo->line_start[current_line + 1] = current_byte;
          current_line++;
          memset(text_line, 0, 80);
          line_len = 0;
          memcpy(&text_line[0], &memo->text[current_byte], word_len);
        }
        line_len += word_len;
        current_byte += word_len;

      } else if (word_len < 0) // we found a CR
      {
        word_len = abs(word_len);

        if ((line_len + word_len - 1) < display_cols) {
          memcpy(&text_line[line_len], &memo->text[current_byte], word_len);
        }

        line_len += word_len;
        current_byte += word_len;

        // print the current line
        MemoDisplayLine(memo,
                        current_line,
                        line_len - 1, // don't count the CR, here
                        t, l, w, refresh_next,
                        bg_color,
                        text_line,
                        text_left,
                        display_lines);

        memo->line_length[current_line] = line_len - 1;
        if (current_byte != memo->line_start[current_line + 1]) {
          refresh_next = TRUE;
        } else {
          refresh_next = FALSE;
        }
        memo->line_start[current_line + 1] = current_byte;
        current_line++;            // there's one more line now...
        memset(text_line, 0, 100); // reset the text_line for more input
        line_len = 0;
      } else // Last line ... no more lines, no CR.
      {
        MemoDisplayLine(memo,
                        current_line,
                        line_len,
                        t, l, w, refresh_next,
                        bg_color,
                        text_line,
                        text_left,
                        display_lines);

        memo->line_length[current_line] = line_len;
        memo->line_start[current_line + 1] = current_byte - word_len;
        current_line++;
      }
    }

    if (current_line == MEMO_MAX_LINES) {
      putch(7);
      Status("Maximum number of lines reached");
    }

    if ((current_line - memo->display_top) < (display_lines)) // - 1))
    {                                                         // clear the bottom of the screen
      area_clear(t + 3 + ((current_line - memo->display_top) * 16),
                 t + h - 2,
                 l + 2,
                 l + w - 2,
                 bg_color >> 4);
    }
    memo->rows = current_line;
    unhide_mouse();
  } //if (memo->so.visible == TRUE)
}

void MemoBoxMouseDown(MEMOBOX_T* memo) {
}

void MemoDownArrow(MEMOBOX_T* memo) {
  int row;

  //move to the next line....

  EraseCursor(memo);

  row = memo->cursor_row + 1;

  if (row < memo->rows) {
    memo->cursor_row = row;
    if (memo->line_length[memo->cursor_row] < memo->cursor_col) {
      memo->cursor_col = memo->line_length[memo->cursor_row];
    }
    if ((memo->cursor_row - memo->display_top) >= memo->display_height) {
      ++(memo->display_top);
      MemoRefreshAll(memo);
      MemoBoxPaint(memo);
    }
  }
  DrawCursor(memo);
  Status("");
}

void MemoUpArrow(MEMOBOX_T* memo) {
  int row;

  //move to the prev line....

  EraseCursor(memo);
  row = memo->cursor_row - 1;
  if (row >= 0) {
    memo->cursor_row = row;
    if (memo->line_length[memo->cursor_row] < memo->cursor_col) {
      memo->cursor_col = memo->line_length[memo->cursor_row];
    }
    if (memo->cursor_row < memo->display_top) {
      --(memo->display_top);
      MemoRefreshAll(memo);
      MemoBoxPaint(memo);
    }
  }
  DrawCursor(memo);
}

void MemoLeftArrow(MEMOBOX_T* memo) {
  int col;

  //move to the prev col....
  EraseCursor(memo);
  col = memo->cursor_col - 1;
  if (col >= 0) {
    memo->cursor_col = col;
    DrawCursor(memo);
  } else // go up one line and to the end
  {
    if ((memo->cursor_row - 1) >= 0) {
      memo->cursor_col = memo->line_length[memo->cursor_row - 1];
      MemoUpArrow(memo);
    }
  }
}

void MemoRightArrow(MEMOBOX_T* memo) {
  int col;

  //move to the next col....
  EraseCursor(memo);

  col = memo->cursor_col + 1;
  if (col <= memo->line_length[memo->cursor_row]) {
    memo->cursor_col = col;
    DrawCursor(memo);
  } else // go down one line and to the beginning
  {
    if (((memo->cursor_row + 1) <= (((memo->so.position.height - 8) / 16) - 1)) && ((memo->cursor_row + 1) < memo->rows)) {
      memo->cursor_col = 0;
      MemoDownArrow(memo);
    }
  }
}

void RepositionCursor(int position, MEMOBOX_T* memo) {
  int new_row;

  for (new_row = 0; new_row < memo->rows; ++new_row) {
    if ((position <= (memo->line_start[new_row] + memo->line_length[new_row])) && (position >= memo->line_start[new_row])) {
      EraseCursor(memo);
      memo->cursor_row = new_row;
      memo->cursor_col = position - memo->line_start[new_row];
      DrawCursor(memo);
      break;
    }
  }
}

void MemoPageUp(MEMOBOX_T* memo) {
  int row;
  int top;

  EraseCursor(memo);
  row = memo->cursor_row + 1 - memo->display_height;
  top = memo->display_top + 1 - memo->display_height;

  if (row < 0) {
    row = 0;
  }
  if (top < 0) {
    top = 0;
  }

  memo->cursor_row = row;
  memo->display_top = top;
  if (memo->line_length[memo->cursor_row] < memo->cursor_col) {
    memo->cursor_col = memo->line_length[memo->cursor_row];
  }

  MemoRefreshAll(memo);
  MemoBoxPaint(memo);
  DrawCursor(memo);
}

void MemoPageDown(MEMOBOX_T* memo) {
  int row;
  int top;

  EraseCursor(memo);

  row = memo->cursor_row + memo->display_height - 1;
  top = memo->display_top + memo->display_height - 1;

  if (row >= (memo->rows)) {
    row = (memo->rows) - 1;
  }
  if (row < 0) {
    row = 0;
  }

  if (top >= (memo->rows)) // don't change a thing.
  {
    top = memo->display_top;
    row = memo->cursor_row;
  }

  if ((top) < 0) {
    top = 0;
  }

  memo->cursor_row = row;
  memo->display_top = top;
  if (memo->line_length[memo->cursor_row] < memo->cursor_col) {
    memo->cursor_col = memo->line_length[memo->cursor_row];
  }

  MemoRefreshAll(memo);
  MemoBoxPaint(memo);
  DrawCursor(memo);
}

void MemoBoxKeyPress(MEMOBOX_T* memo, int c) {
  if (memo->enabled == TRUE) {
    if (c == 0) // Extended character
    {
      c = kbchar(); // get the rest of it....
      switch (c) {
      case 77: // right arrow
        MemoRightArrow(memo);
        break;
      case 80: // down arrow
        MemoDownArrow(memo);
        break;
      case 75: // left arrow
        MemoLeftArrow(memo);
        break;
      case 72: // up arrow
        MemoUpArrow(memo);
        break;
      case 71: // home
        EraseCursor(memo);
        memo->cursor_col = 0;
        DrawCursor(memo);
        break;
      case 79: // end
        EraseCursor(memo);
        memo->cursor_col = memo->line_length[memo->cursor_row];
        DrawCursor(memo);
        break;
      case 73: // page up
        MemoPageUp(memo);
        break;
      case 81: // page down
        MemoPageDown(memo);
        break;
      case 82: //over_write
        if (over_write == 0) {
          over_write = 1;
          ptext("OVR", 460, 605, BG_WHT + FG_BLU);
        } else {
          over_write = 0;
          ptext("INS", 460, 605, BG_WHT + FG_BLU);
        }
        break;
      case 83: //delete
        DeleteKey(memo);
        break;
      }
      MemoBoxScroll(memo, 0);
    } else // Non-extended character
    {
      if (((c >= ' ') && (c < 127)) || (c == CR)) {
        if (c == CR) {
          c = LF;
        }
        if (over_write == TRUE) {
          OverWriteChar(memo, c);
        } else {
          if ((signed int)strlen(memo->text) < memo->max_length) {
            InsertChar(memo, c);
          } else {
            putch(7);
            Status("The message area is full!");
          }
        }
      } else if (c == 8) // back space
      {
        BackSpace(memo);
      }
    }
    if (memo->OnChange != NULL) {
      (*memo->OnChange)();
    }
  }
}

void MemoBoxMouseUp(MEMOBOX_T* memo) {
  ;
}

void MemoBoxClick(MEMOBOX_T* memo) {
  int row;

  if (memo->enabled == TRUE) {
    EraseCursor(memo);

    row = (mouse_info.y - memo->so.position.top) / 16;
    if ((row <= (((memo->so.position.height - 8) / 16) - 1)) && (row < memo->rows)) {
      memo->cursor_row = row;
    }
    memo->cursor_col = (mouse_info.x - memo->so.position.left) / 8;
    if (memo->line_length[memo->cursor_row] == -1) {
      memo->cursor_col = 0;
    } else if (memo->line_length[memo->cursor_row] < memo->cursor_col) {
      memo->cursor_col = memo->line_length[memo->cursor_row];
    }
    DrawCursor(memo);
  }
}

void MemoBlink(void) {
  MEMOBOX_T* memo;

  memo = (MEMOBOX_T*)glbWindow->so[glbWindow->focus.current];
  if (memo->enabled == TRUE) {
    if (glbWindow->so[glbWindow->focus.current]->type == MEMO_BOX) {
      hide_mouse();
      if (tb_cursor) //is cursor on
      {
        tb_cursor = 0; // turn cursor off
        EraseCursor(memo);
      } else {
        tb_cursor = 1; // turn cursor on
        DrawCursor(memo);
      }
      unhide_mouse();
      ticks_set(BlinkTimer, BLINK_RATE);
    }
  }
}

// FindAWord returns the length of the word
// ndx is an index into the memo.text
// display_cols is the width in columns of the memo box
// FindAWord also strips those annoying LF's from the text
static int FindAWord(MEMOBOX_T* memo, int ndx, int display_cols) {
  int word_len = 0;
  int current_byte;

  //get a word of text before adding it to the line of text...
  current_byte = ndx;

  // first, add all non white space chars to the word
  if (memo->text[current_byte] == LF) {
    if (memo->text[current_byte + 1] == CR) {
      memo->text[current_byte + 1] = ' ';
    }
    word_len++;
    word_len *= -1;
  } else if (memo->text[current_byte] == CR) {
    memo->text[current_byte] = LF;
    word_len++;
    word_len *= -1;
  } else {
    while ((memo->text[current_byte] != ' ')  // add all the characters that ARE NOT spaces to the word
           && (word_len < display_cols)       // don't exceed the display columns width
           && (memo->text[current_byte] != 0) // stop because its over now...
           && (memo->text[current_byte] != LF) && (memo->text[current_byte] != CR)) {
      current_byte++;
      word_len++;
    }
    // next, add all the white space that will fit on the line
    while ((memo->text[current_byte] == ' ')  // add all the characters that ARE spaces to the word
           && (word_len < display_cols)       // don't exceed the display columns width
           && (memo->text[current_byte] != 0) // stop because its over now...
           && (memo->text[current_byte] != LF) && (memo->text[current_byte] != CR)) {
      current_byte++;
      word_len++;
    }
  }

  return (word_len);
}

static void InsertChar(MEMOBOX_T* memo, int c) {
  int row;
  int col;
  int line_start;
  int position;

  for (row = memo->cursor_row + 1; row < memo->rows; ++row) {
    ++memo->line_start[row]; // inserting, line start should be 1 char later
  }

  row = memo->cursor_row;
  col = memo->cursor_col;
  line_start = memo->line_start[row];
  position = line_start + col;

  // first, move all the text over one space to make room for the next char
  memmove(&memo->text[position + 1],
          &memo->text[position],
          memo->max_length - (line_start + col + 1));

  // next, copy in the new value to insert spot
  memo->text[line_start + col] = c;

  // next, paint the memo box
  // painting should recalculate all the line lengths
  MemoBoxPaint(memo);

  if (c != LF) {
    // move the cursor position over
    MemoRightArrow(memo);

    RepositionCursor(position + 1, memo);
  } else {
    EraseCursor(memo);
    memo->cursor_col = 0;
    DrawCursor(memo);
    MemoDownArrow(memo);
  }
}

#if 0
void out(MEMOBOX_T* memo)
{
   char tmp[100];
   sprintf(tmp, "ls:%d + col:%d = %d rows:%d",
      memo->line_start[memo->cursor_row],
      memo->cursor_col,
      memo->line_start[memo->cursor_row] + memo->cursor_col,
      memo->rows);

   //Status(tmp);
}
#endif

// the complement to InsertChar()
static void OverWriteChar(MEMOBOX_T* memo, int c) {
  int row;
  int col;
  int line_start;
  int position;

  row = memo->cursor_row;
  col = memo->cursor_col;
  line_start = memo->line_start[row];
  position = line_start + col;

  if (position < memo->max_length) // can't go past end!!
  {
    // replace the current char position with c
    memo->text[position] = c;

    // next, paint the memo box
    // painting should recalculate all the line lengths
    MemoBoxPaint(memo);

    if (c != LF) {
      // move the cursor position over
      MemoRightArrow(memo);
      RepositionCursor(position + 1, memo);
    } else // LF entered
    {
      EraseCursor(memo);
      memo->cursor_col = 0;
      DrawCursor(memo);
      MemoDownArrow(memo);
    }
  } else {
    putch(7);
    Status("The message area is full!");
  }
}

static void BackSpace(MEMOBOX_T* memo) {
  int row;
  int col;
  int line_start;
  int position;

  // adjust line start offsets for later processing.
  for (row = memo->cursor_row + 1; row <= memo->rows; ++row) {
    --memo->line_start[row]; // we're deleting, so line start should be 1 char later
  }

  row = memo->cursor_row;
  col = memo->cursor_col;
  line_start = memo->line_start[row];
  position = line_start + col;

  if ((position) > 0) {
    memcpy(&memo->text[position - 1],
           &memo->text[position],
           (memo->max_length + 1) - (position));
    memo->text[memo->max_length - 1] = 0;

    // next, paint the memo box
    // painting should recalculate all the line lengths
    MemoBoxPaint(memo);

    // move the cursor position over
    MemoLeftArrow(memo);

    RepositionCursor(position - 1, memo);
  }
}

static void DeleteKey(MEMOBOX_T* memo) {
  int row;
  int col;
  int line_start;
  int position;

  row = memo->cursor_row;
  col = memo->cursor_col;
  line_start = memo->line_start[row];
  position = line_start + col;

  memcpy(&memo->text[line_start + col],
         &memo->text[line_start + col + 1],
         memo->max_length - (line_start + col));
  memo->text[memo->max_length - 1] = 0;

  // next, paint the memo box
  // painting should recalculate all the line lengths
  MemoBoxPaint(memo);
  if (position > (int)strlen(memo->text)) {
    position = (int)strlen(memo->text);
  }
  RepositionCursor(position, memo);
}
