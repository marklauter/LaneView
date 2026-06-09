// ==========================================================================
// $Workfile:   graphs.h  $
// $Revision:   1.7  $
//
// HISTORY:
// Author            Date      Description
// --------------------------------------------------------------------------
//
//
//
// DESCRIPTION
// ==========================================================================
//   Copyright 1996 TeCom Inc. All rights reserved
// ==========================================================================

#ifndef __GRAPHS_H
#define __GRAPHS_H

extern int in_device_detail;
extern int detail_mode;

void device_detail(int device_ndx);
void update_device_detail(int device_ndx);
void horizontal_graph(int graph_type);
void horiz_house_graph(
    int pixrow,
    double device_kw,
    double device$,
    double full_kw,
    double full$);

void daily_usage(void);
void daily_cost(void);
void MonthlyCost(void);
void MonthlyUsage(void);
void MonthlyCostEffective(void);
void MeterHistory(void);
void ShowDaysRates(void);

#endif
