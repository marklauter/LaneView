// Progress Indicator Bar
// created 6 Nov 96 | TMMSL
// Copyright 1996 TeCom Inc

#ifndef __progindicator_h
#define __progindicator_h

typedef struct
{
  COORDINATE_T position;
  int color;
  UCHAR progress; // int from 0 to 100
} PROGRESS_INDICATOR_T;

void PutIndicator(PROGRESS_INDICATOR_T* pi);
void UpdateIndicator(PROGRESS_INDICATOR_T* pi);
#endif
