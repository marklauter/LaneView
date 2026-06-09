// ==========================================================================
// $Workfile:   2box.h  $
// $Revision:   1.67  $
//
// HISTORY:
// Author            Date      Description
// --------------------------------------------------------------------------
/*
$LOG J:/intrlane/src/include/vcs/2box.h_v $
*/
//
//
// DESCRIPTION
//
// ==========================================================================
//   Copyright 1996 TeCom Inc. All rights reserved
// ==========================================================================

#ifndef _2box_h

#define _2box_h

#define XFI 0

#define MAX_DEVICES 64
#define MAX_EVENTS 512
#define MAX_HISTORY 10
#define MAX_MOODS 32
#define MAX_TRIGGERS 32

#define OUT_INT_MAX 40

#define DEVICES_VERSION -6

#define TRIGGER_STATE_ON 1
#define TRIGGER_STATE_OFF 0

#define TRUE 1
#define FALSE 0
#define BLINKING 0x080
#define ALPHA 0
#define INTEGER 1
#define FLOAT 2
#define DATE 3
#define TIME 4
#define PSWD 5
#define NO_F9 0
#define OK_F9 1

#define ESC '\x01b'
#define CR '\x00d'
#define LF '\x00a'

#define CUR_UP -31
#define CUR_LT -32
#define CUR_RT -33
#define CUR_DN -34
#define ALT_D -35
#define F10_HIT -20
#define F9_HIT -19
#define ESC_HIT -1
#define CR_HIT 0
#define TAB_HIT -18

//#define TICK_SIZE      0.6

typedef unsigned short USHORT;
typedef signed short SSHORT;
typedef unsigned long ULONG;
typedef signed long SLONG;
typedef unsigned char UCHAR;

#if 0
typedef struct
{
   UCHAR            sunrise_hr;
   UCHAR            sunrise_min;
   UCHAR            sunset_hr;
   UCHAR            sunset_min;
   USHORT            high_temp_f;
   USHORT            low_temp_f;
   UCHAR            wind_speed_mph;
   USHORT            wind_direction_az;  // degrees clockwise from north = 0;
   UCHAR            humidity_pct;
   UCHAR            tanning_idx;
   USHORT            pollen_idx;
   USHORT            pollution_idx;
   UCHAR             chance_precipitation_pct;
   UCHAR             forecast[196];
} WEATHER;

typedef struct
{
   UCHAR             index;
   UCHAR             status;
   UCHAR             month;
   UCHAR             date;
   UCHAR             year;
   UCHAR             hr;
   UCHAR             min;
   UCHAR             from[11];
   UCHAR             to[11];
   UCHAR             message[500];
} EMAIL;
#endif

#define DEVICE_LIGHTING 0
#define DEVICE_ENVIRON 1
#define DEVICE_SUBMETER 2

typedef struct // DEVICE_REL_1_0;
{
  USHORT device_slot;
  USHORT watts;       // volts * amps * power_factor
  USHORT amps;        // not used
  UCHAR volts;        // not used
  UCHAR power_factor; // ~ to cos(phase angle)
  UCHAR category;     // 1 = lighting
                      // 2 = environmental
                      // 3 = pool/spa
                      // 4 = sprinkler
                      // 5 = Alert device

  UCHAR priority;    // for load shedding
  UCHAR monitored;   // Y/N monitored
  UCHAR code_number; // X10 address
  UCHAR code_letter; // X10 address  or '@' for CEBus
  USHORT cebus_hc;
  USHORT cebus_unit;
  USHORT cebus_subunit; // TECO submeters monitor up to 4 loads
  UCHAR desc[13];
  UCHAR location[18];
  UCHAR cebus_type; // 0 = CEBus Ack
                    // 1 = Reserved
                    // 2 = Intellon wall dimmer
                    // 3 = Intellon wall relay
                    // 4 = Teco Submeter - power monitor
                    // 5 = Intellon 220 volt Load Shedder
                    // 6 = Teco Submeter II - power monitor

  //                                                            Monitored?
  //cebus_type;         // 0 = CEBus Ack                     NO
  //                    // 1 = Reserved                     NO
  //                    // 2 = Intellon wall dimmer            NO
  //                    // 3 = Intellon wall relay            NO
  //                    // 4 = Intellon 220 volt Load Shedder      NO
  //                    // 5 = Teco Submeter - power monitor      YES
  //                    // 6 = Teco Submeter  II- power monitor   YES
  //                                                            controllable?
  //cebus_type;         // 0 = CEBus Ack                     YES
  //                    // 1 = Reserved                     YES
  //                    // 2 = Intellon wall dimmer            YES
  //                    // 3 = Intellon wall relay            YES
  //                    // 4 = Intellon 220 volt Load Shedder      YES
  //                    // 5 = Teco Submeter - power monitor      NO
  //                    // 6 = Teco Submeter II - power monitor   NO

  UCHAR control_type; // (if controlled)
                      // 0 = std (by event)
                      // 1 = Total Cost per day or max minutes
                      // 2 = Cost per hour or max minutes

  USHORT max_mins_day;    // control_type 1,2, Max Mins per day
  USHORT cost_day_hour;   // control_type 1:  Maximum cents a day
                          // OR: control_type 2: max cents per hour*10
  UCHAR override;         // override smart scheduling
  UCHAR operate_hours[3]; // hours that smart scheduling will occur
                          // 80 00 00 = midnight to 1am
                          //            (24 bits for 24 hours)
  UCHAR serial_no[12];    // Device Serial number
  UCHAR history_bucket;   // history bucket number
  UCHAR emerg_load_shed;  // emergency load shed flag
  USHORT monitor_hc;
  USHORT monitor_unit;
  USHORT monitor_subunit;
  UCHAR extra[4]; // "filler"

  // End of New device data
  USHORT todays_minutes; // total half minutes turned on today
  ULONG todays_cents;    // total pennys spent today
  UCHAR status;          // currently scheduled on/off/dim state
  UCHAR over_ridden;     // mode/mood has affected this one
  UCHAR added_year;
  USHORT added_dayofyear;
  UCHAR added_int;
  UCHAR on_off_status;
  USHORT remainder;
  USHORT remainder1;
  USHORT remainder2;
  SLONG abs_added_int; // when device was created
  UCHAR extra1[4];     // "filler"
} DEVICE_REL_1_0;

//#define DEV_ADD_CHANGE_LENGTH (sizeof(DEVICE) -2-4-1-1-1-2-1-1-2-2-2-8)

typedef struct // DEVICE
{
  USHORT device_slot;
  USHORT watts;   // volts * amps * power_factor
  USHORT factor;  // factor for M&M's (times 100.
                  //     I.e, 385 == factor of 3.85)
  UCHAR volts;    // used by M&M's
  UCHAR category; // 1 = lighting
                  // 2 = environmental
                  // 3 = pool/spa
                  // 4 = sprinkler
                  // 5 = Alert device
                  // 6 = Security

  UCHAR priority;    // for load shedding
  UCHAR code_number; // X10 address
  UCHAR code_letter; // X10 address  or '@' for CEBus
  USHORT cebus_hc;
  USHORT cebus_unit;
  USHORT cebus_subunit; // TeCom submeters monitor up to 4 loads
  UCHAR desc[13];
  UCHAR location[18];
  UCHAR cebus_type; // 0 = CEBus Ack
                    // 1 = Reserved
                    // 2 = Intellon wall dimmer
                    // 3 = Intellon wall relay
                    // 4 = Intellon 220 volt Load Shedder
                    // 5 = TeCom Submeter - power monitor
                    // 6 = TeCom Submeter II - power monitor

  //                                             Monitored?
  //cebus_type;         // 0 = CEBus Ack                            No
  //                    // 1 = Reserved                             No
  //                    // 2 = Intellon wall dimmer                 No
  //                    // 3 = Intellon wall relay                  No
  //                    // 4 = Intellon 220 volt Load Shedder       No
  //                    // 5 = Teco Submeter - power monitor        Yes
  //                    // 6 = Teco Submeter II - power monitor     Yes
  //
  //                                             Controlled?
  //cebus_type:         // 0 = CEBus Ack                            Yes
  //                    // 1 = Reserved                             Yes
  //                    // 2 = Intellon wall dimmer                 Yes
  //                    // 3 = Intellon wall relay                  Yes
  //                    // 4 = Intellon 220 volt Load Shedder       Yes
  //                    // 5 = Teco Submeter - power monitor        No
  //                    // 6 = Teco Submeter II - power monitor     No

  UCHAR control_type; // (if controlled)
                      // 0 = std (by event)
                      // non-zero means Smart Scheduling activated
                      //      1 = Smart-shopping when not vacation
                      //      2 = Smart-shopping always

  USHORT max_mins_day;    // control_type 1,2, Max Mins per day
  USHORT cost_day_hour;   // control_type 1:  Maximum cents a day
                          // OR: control_type 2: max cents per hour*10
  UCHAR ss_override;      // override smart scheduling
  UCHAR operate_hours[3]; // hours that smart scheduling will occur
                          // 80 00 00 = midnight to 1am
                          // (24 bits for 24 hours)
  UCHAR serial_no[12];    // Device Serial number
  UCHAR history_bucket;   // history bucket number
  UCHAR under_load_ctrl;  // load shed flag
  USHORT monitor_hc;
  USHORT monitor_unit;
  USHORT monitor_subunit;
  time_t override_start;
  time_t override_end;
  UCHAR x10_device_type;     // 0 == Relay
                             // 1 == Dimmer
                             // 2 == Annunciator
                             // 3 == RCS Thermostat
  UCHAR cooling_setbacks[4]; // for Rate Level (low[0] to high[3])
  UCHAR heating_setbacks[4]; // for Rate Level (low[0] to high[3])
  UCHAR extra[11];           // "filler"

  // End of New device data
} DEVICE;

typedef struct
{
  USHORT todays_minutes; // total half minutes turned on today
  ULONG todays_cents;    // total pennys spent today
  UCHAR status;          // currently scheduled on/off/dim state
  UCHAR over_ridden;     // mode/mood has affected this one
  UCHAR on_off_status;   // save load shedder mood value
  USHORT remainder;
  USHORT remainder1;
  USHORT remainder2;
  SLONG abs_added_int;   // when device was created
  ULONG running_average; // running average of usage "WHEN ON"
  UCHAR tstat_mode_status;
  UCHAR status_method1; // how it got set as it is...(why)
  UCHAR last_status;    // how last control set it (no matter why)
  UCHAR status_method2; // how it got set as it is...(why)
  UCHAR last_status2;   // how last control set it (no matter why)
  UCHAR extra[15];      // "filler"
} DEVICEX;

typedef struct
{
  // following is zero for a new device
  ULONG current_day[24];  // KWH*10000 for each hour
  ULONG mtd[24];          // KWH*10000 for each hour
  ULONG last_month[24];   // KWH*10000 for each hour
  ULONG ytd[24];          // KWH*10000 for each hour
  ULONG current_day$[24]; // dollars*100000 for each hour
  ULONG mtd$[24];         // dollars*100000 for each hour
  ULONG last_month$[24];  // dollars*100000 for each hour
  ULONG ytd$[24];         // dollars*100000 for each hour
  USHORT remainder;
  USHORT remainder1;
} DEVICE_HISTORY;

typedef struct
{
  USHORT device_slot;         // always 0xffff
  ULONG first_today;          // first dial reading today (midnight)
                              // ... times 10000
  ULONG todays_dollars;       // ... Dollars times 100000
  ULONG current_dial;         // current dial reading @ last interval
                              // ... times 10000
  ULONG last_int_dial;        // ...dial at time of last 15 min inter
                              // ... times 10000
  ULONG month_start_dial;     // ...dial at time of beginning of month
                              // ... times 10000
  ULONG last_mo_start_dial;   // ...dial at time of beginning of last
                              // ... month times 10000
  ULONG current_day[24];      // usage for each hour today
                              // ... kwh times 10000
  ULONG mtd[24];              // total usage for each hour this month
                              // ... kwh times 10000
  ULONG last_month[24];       // total usage for each hour last month
                              // ... kwh times 10000
  ULONG ytd[24];              // total usage for each hour this year
                              // ... kwh times 10000
  ULONG current_day$[24];     // usage for each hour today
                              // ... kwh times 10000
  ULONG mtd$[24];             // total usage for each hour this month
                              // ... kwh times 10000
  ULONG last_month$[24];      // total usage for each hour last month
                              // ... kwh times 10000
  ULONG ytd$[24];             // total usage for each hour this year
                              // ... kwh times 10000
  ULONG tod_current_day$[24]; // cost for each hour today
                              // ... Dollars times 100000
  ULONG tod_mtd$[24];         // total cost for each hour this month
                              // ... Dollars times 100000
  ULONG tod_last_month$[24];  // total cost for each hour last month
                              // ... Dollars times 100000
  ULONG tod_ytd$[24];         // total cost for each hour this year
                              // ... Dollars times 100000
  ULONG var_current_day$[24]; // cost for each hour today
                              // ... Dollars times 100000
  ULONG var_mtd$[24];         // total cost for each hour this month
                              // ... Dollars times 100000
  ULONG var_last_month$[24];  // total cost for each hour last month
                              // ... Dollars times 100000
  ULONG var_ytd$[24];         // total cost for each hour this year
                              // ... Dollars times 100000
  USHORT remainder;
  USHORT flat_remainder1;
  USHORT tod_remainder1;
  USHORT var_remainder1;
  USHORT failures; // failed readings
  UCHAR battery_status;
  ULONG LastCMIValue;
  UCHAR extra[45]; // "filler"
} METER;

typedef struct
{
  USHORT cebus_hc;
  USHORT cebus_unit;
  UCHAR last_interval;
  UCHAR last_day;
} METER_INTERNALS;

typedef struct
{
  USHORT event_id;
  USHORT device;
  UCHAR hour;
  UCHAR min;
  UCHAR type_day;     // SMTWTFSV  bits 0->7 (Sunday bit 0)
  UCHAR action;       // low nibble, 0 = off, 1 = on, 2 = dim
                      // high nibble: Dim Level 0 = none...
                      //                       15 = most;
  UCHAR event_type;   // if high bit (0x80) set, it's a mood
                      // 0 = Standard
                      // 1 = 1 time event
                      // 2 = Dawn
                      // 3 = Dusk
                      // 4 = random threshold
  SSHORT offset_mins; // minutes to add/subtract to dawn, dark
                      // or limit in mins for type 4 events
  UCHAR base_hour;    // for event type 4:
  UCHAR base_min;     // for event type 4:
} EVENT_REL13;

typedef struct
{
  USHORT event_id;
  USHORT device;
  UCHAR hour;
  UCHAR min;
  UCHAR type_day;     // SMTWTFS  bits 0->6 (Sunday bit 0)
  UCHAR action;       // low nibble, 0 = off, 1 = on, 2 = dim
                      // high nibble: Dim Level 0 = none...
                      //                       15 = most;
  UCHAR event_type;   // if high bit (0x80) set, it's a mood
                      // 0 = Standard
                      // 1 = 1 time event
                      // 2 = Dawn
                      // 3 = Dusk
                      // 4 = random threshold
  SSHORT offset_mins; // minutes to add/subtract to dawn, dark
                      // or limit in mins for type 4 events
  UCHAR base_hour;    // for event type 4:
  UCHAR base_min;     // for event type 4:
  UCHAR mask1;        // bit definitions:
                      //      0x080   7 - Vacation
                      //      0x040   6 - Normal
                      //      0x020   5 - Heating Mode
                      //      0x010   4 - Cooling Mode
                      //      0x008   3 - Spring
                      //      0x004   2 - Summer
                      //      0x002   1 - Autumn
                      //      0x001   0 - Winter
  UCHAR mask2;        // bit definitions:
                      //      0x80   7 - Execute in Maximum cost mode
                      //      0x040   6 - Execute in High cost mode
                      //      0x020   5 - Execute in Moderate cost mode
                      //      0x010   4 - Execute in Low cost mode
                      //      0x008   3 - Execute in at home mode
                      //      0x004   2 - Execute in temporarily away mode
                      //      0x002   1 - n/a
                      //      0x001   0 - n/a
  UCHAR mask3;        // for future expansion
} EVENT;

//  double cost_table[256];  defined by power company.   Usually has only a
//                           few entries & will last for several months
//                           (we haven't determined how it will get into
//                           Inside computer, but SHU box won't keep it.
//                      references to cost_tbl_idx refer to entry in
//                      this table that has the current dollars (or
//                           whatever) per kwh.

typedef struct
{
  UCHAR name[20];
  UCHAR addr[30];
  UCHAR acct[20];
  ULONG serial_no;   // IRCU serial number
  UCHAR password[8]; // support login
  USHORT devices_used;
  USHORT events_used;
  USHORT history_slots[MAX_HISTORY]; // outside only
  USHORT hslots_used;                // outside only
  ULONG cost_kwh_rate;               // cost rate * 100,000
  UCHAR cost_index;                  // cost index
  UCHAR rate_type;                   // 0 = fixed, 1 = tod, 2 = variable
  UCHAR franchise_fee_index;         //
  UCHAR city_tax_index;              // 0 = everbody else, 1 = Polk City,
                                     // 2 = Oldsmar
  UCHAR free;                        // unused
  UCHAR day_mask;                    // 1 = Sunday, 2 = Monday, 4 = Tuesday,
                                     // 8 = Wednesday, 16 = Thursday,
                                     // 32 = Friday, 64 = Saturday,
                                     // 128 = Vacation
  UCHAR rate_signal;                 // current rate signal in effect
  USHORT moods_used;                 // count of defined moods
  USHORT triggers_used;              // count of defined triggers
  USHORT moods_active;               // count of active moods
  int meter_scalar;
  UCHAR filler[42];
} CUSTOMER;

typedef struct
{
  ULONG rs_rate;
  // pricing signals  -- was just on/off peak now one of 16 levels (0 -> 15)
  // Midnight to 1 stored in array[0] high nibble, 1:00 am to 2:00 am stored
  // in array[0], low nibble, etc.
  UCHAR rtod_signal[12];
  UCHAR rsv_signal[12];
  ULONG rtod_rate[24];
  ULONG rsv_rate[24];
  ULONG credit_rate;
  ULONG penalty_rate;
} DAYS_RATES;

typedef struct
{
  ULONG rs_rate;
  ULONG rst_off_peak_rate;
  ULONG rst_on_peak_rate;
  ULONG rsv_rate[24];
  ULONG credit_rate;
  ULONG penalty_rate;
  UCHAR flags;
} R11_DAYS_RATES;

typedef struct
{
  ULONG rs_rate;
  ULONG rst_off_peak_rate;
  ULONG rst_on_peak_rate;
  ULONG rsv_rate[24];
} OLD_DAYS_RATES;

typedef struct
{
  ULONG cust_charge_std;
  ULONG cust_charge_tod;
  ULONG cust_charge_var;
  ULONG fla_gross_receipt_tax_pct;
  ULONG franchise_fee[20];
  ULONG fuel_cost_std;
  ULONG fuel_cost_tod_peak;
  ULONG fuel_cost_tod_offpeak;
  ULONG fuel_cost_var_peak;
  ULONG fuel_cost_var_offpeak;
  ULONG fuel_charge_pct;
  ULONG fuel_charge_2_pct;
  ULONG city_tax_pct[20];

  ULONG peak_kwh[13];
  ULONG off_peak_kwh[13];
  ULONG flat_rate_mtd$[13];
  ULONG tod_rate_mtd$[13];
  ULONG var_rate_mtd$[13];
  UCHAR filler[50];
} OLD_BILLING_INFO;

typedef struct
{
  ULONG peak_kwh[13];
  ULONG off_peak_kwh[13];
  ULONG flat_rate_mtd$[13];
  ULONG tod_rate_mtd$[13];
  ULONG var_rate_mtd$[13];
  ULONG load_ctrl_credit_kwh[13];
  ULONG load_ctrl_penalty_kwh[13];
  ULONG load_ctrl_credit$[13];
  ULONG load_ctrl_penalty$[13];
} BILLING_INFO;

typedef struct
{
  ULONG cust_charge_std;
  ULONG cust_charge_tod;
  ULONG cust_charge_var;
  ULONG fla_gross_receipt_tax_pct;
  ULONG franchise_fee[20];
  ULONG fuel_cost_std;
  ULONG fuel_cost_tod_peak;
  ULONG fuel_cost_tod_offpeak;
  ULONG fuel_cost_var_peak;
  ULONG fuel_cost_var_offpeak;
  ULONG fuel_charge_pct;
  ULONG fuel_charge_2_pct;
  ULONG city_tax_pct[20];
} BILLING_CONSTANTS;

typedef struct
{
  USHORT device_slot; //  0xffff for meter
  ULONG readingRsecs; // reading for whole house or
                      // seconds for cebus_type 6
  ULONG todays_kwh;   // kwh * 10000
  ULONG kwh;          // kwh * 10000
  ULONG dollars;
  UCHAR hour;
  UCHAR newday;
  ULONG cost_kwh;        // $ * 100000
  UCHAR shedding_status; // 0 = no shedding
  UCHAR day_mask;
} INTERVAL_DATA;

typedef struct
{
  ULONG tod;
  ULONG var;
  UCHAR hour;
  UCHAR mode_byte1;
  UCHAR mode_byte2;
  UCHAR mode_byte3;
} INTERVAL_DATA$;

typedef struct
{
  UCHAR yr;
  UCHAR mo;
  UCHAR da;
  UCHAR hr;
  UCHAR qthr;
  UCHAR rate_type;
  USHORT device_id;
  ULONG kwh;
  ULONG kwh_cost;
  UCHAR flags;
} INTERVAL_DATA_15;

typedef struct // used in HISTORY_DAY BELOW
{
  ULONG cost_kwh; // $ * 100000
  ULONG kwh;      // kwh * 10000 for this period
  UCHAR rate_type;
  // (0 = fixed, 1 = tod, 2 = var) * 2 + rate_signal * 16
  UCHAR flags;
  // flags:
  //         0,1      device's priority at that time
  //         2,3,4   Load control signal Level in effect
  //         5      Penalty assessed
  //         6      Credit Given
  //         7      From Meter
} PERIOD_DATA;

typedef struct        // 15 minute interval history records
{                     // which need to be put into a history
                      // file & indexed for graphic displays
  USHORT device_slot; //  0xffff for meter
  UCHAR year;         //
  UCHAR month;
  UCHAR day;
  UCHAR first_period;   // beginning period number
                        // 0 == midnight to 12:15a
                        // ...
                        // 95 == 11:45p to midnight
  UCHAR number_periods; // 96 = full day, < 96 = partial day
  PERIOD_DATA period[96];
} HISTORY_DAY;

////////Fast History Day //////////////////////////////////////////////

typedef struct // used in FAST_HISTORY_DAY_T, below
{
  ULONG cost_per_kwh;         // Dollars * 100,000
  ULONG credit_per_kwh;       // Dollars * 100,000
  ULONG penalty_per_kwh;      // Dollars * 100,000
  USHORT meter_pulses;        // each pulse is 1.2 watt-hrs (.0012 KWH)
  USHORT device_watt_hrs[10]; // one entry per history bucket
                              // stored as 1/10 watt-hr (.0001 KWH)
  UCHAR rate_type;            // 0 = std, 1 = tod, 2 = var
  UCHAR shedding_signal;      // Load ctrl level (0=none)
  UCHAR flags[10];            // one entry per history bucket:
                              //  flags   Bit#   Description
                              //         0,1      device's priority at the time
                              //         2,3,4   Load control level in effect
                              //         5      1 = Penalty Assessed
                              //         6      1 - Credit granted
                              //         7      1 = from meter.
} FAST_PERIOD_DATA;

typedef struct // 15 minute interval history records
{              // which need to be put into a history
               // file & indexed for graphic displays
  UCHAR year;  //
  UCHAR month;
  UCHAR day;
  UCHAR first_interval;   // beginning period number
  UCHAR number_intervals; // 96 = full day, < 96 = partial day
  FAST_PERIOD_DATA period[96];
} FAST_HISTORY_DAY_T;
////////Fast History Day ///////////////////////////////////////////////

typedef struct
{
  USHORT device_slot;
  UCHAR action; // 0 = Off
                // 1 = On
                // 0xD2 = Dim to level "D"
} MOOD_ENTRY;

typedef struct
{
  USHORT mood_id;      // unique identifier;
  USHORT default_mins; // not used
  UCHAR name[20];      // Operator defined.
  UCHAR items;         // count of entries in this mood
  UCHAR type;          // not used
  MOOD_ENTRY entry[32];
} MOOD_T;

typedef struct
{
  UCHAR trigger_action; // 0 = No Action
                        // 1 = start mood (see mood_id)
                        // 2 = end a mood (see mood_id)
                        // 3 = end all moods
                        // 4 = set day type to today
                        // 5 = master mode
                        // 6 = A/C mode
                        // 7 = Season mode
                        // 8 = Occupied mode
  USHORT mood_id;       // if needed (action 1 or 2 above)
  USHORT execute_type;  // 0 = not protected
                        //         (schedule will over-ride)
                        // 1 = protected (won't end --
                        //         schedule won't affect state)
                        /////////// mode changes //////////////
                        // 5: 0 = Normal;  1 = Vacation
                        // 6: 0 = Cooling; 1 = Heating
                        // 7: 0 = Spring;  1 = Summer; 2 = Fall; 3 = Winter
                        // 8: 0 = Occupied; 1 = Temporarily away
                        /////////// mode changes //////////////
  USHORT minutes;       // Zero = forever (only for exec_type 1)
} SECURITY_ACTION_T;

typedef struct
{
  USHORT trigger_type;              // device identifier: CEGate = 0
  UCHAR code_letter;                // X10 house address
  UCHAR code_number;                // X10 code number
  USHORT house_code;                // cebus house code
  USHORT unit;                      // cebus unit
  SECURITY_ACTION_T arm_level[6];   // actions on entry to ARMED levels
                                    // 0 = Disarmed
                                    // 1 = Armed Away
                                    // 2 = Armed Stay
                                    // 3 = Armed Instant
                                    // 4 = Armed Maximum
                                    //Don't ask->      // 5 = Alarm Condition (Not used now)
  SECURITY_ACTION_T alarm_level[4]; // actions on entry to ARLARMED levels
                                    // 0 = No Alarm
                                    // 1 = Fire Alarm
                                    // 2 = Intrusion Alarm
                                    // 3 = Trouble Alarm
} SECURITY_T;

typedef struct
{
  USHORT trigger_id;    // trigger_id
  USHORT trigger_type;  // NOT USED (device identifier)
  UCHAR code_letter;    // X10 house address '@' for cebus
  UCHAR code_number;    // X10 code number
  USHORT house_code;    // cebus house code
  USHORT unit;          // cebus unit
  UCHAR incoming_state; // 0 = Off, 1 = On
                        // see: #define TRIGGER_STATE_ON   1
                        // see: #define TRIGGER_STATE_OFF   0
  UCHAR trigger_action; // 1 = start mood (see mood_id)
                        // 2 = end a mood (see mood_id)
                        // 3 = end all moods
                        // 4 = set day type to today
                        // 5 = master mode
                        // 6 = A/C mode
                        // 7 = Season mode
                        // 8 = Occupied mode
  USHORT mood_id;       // only for trigger_action 1 or 2
  USHORT execute_type;  // when starting a mood, mood mode to use:
                        //   0 = not protected (schedule prevails)
                        //   1 = protected (won't end)
                        /////////// mode changes //////////////
                        // 5: 0 = Normal;  1 = Vacation
                        // 6: 0 = Cooling; 1 = Heating
                        // 7: 0 = Spring;  1 = Summer; 2 = Fall; 3 = Winter
                        // 8: 0 = Occupied; 1 = Temporarily away
                        /////////// mode changes //////////////
  USHORT minutes;       // When starting mood (0 = forever)
                        // only for execute_type = 1
} TRIGGER_T;

typedef struct Dawn_Dusk_Time_Struct {
  UCHAR dawn_hour;
  UCHAR dawn_min;
  UCHAR dusk_hour;
  UCHAR dusk_min;
} DAWN_DUSK_TIME;

typedef struct Dawn_Dusk_Week_Struct {
  DAWN_DUSK_TIME dawn_dusk_time[7];
} DAWN_DUSK_WEEK;

#define MAX_CALLINS 672

#define CI_ONCE 0
#define CI_REPEAT 1

typedef struct
{
  UCHAR ci_hour;
  UCHAR ci_min;
  UCHAR ci_mday;
  UCHAR ci_type;
  //UCHAR            ci_unused[2];
} CALLIN_TIME;

typedef struct
{
  UCHAR mon;
  UCHAR day;
} HDAY;

typedef struct
{
  UCHAR number;
  HDAY hday[12];
} HOLIDAY_TAB;

typedef struct
{
  USHORT visual_id;       // Visual device id
  USHORT audio_id;        // Audio Device id
  UCHAR do_visual;        // 0 = no, 1 = yes
                          //         OR 0x80 + Dim level
  UCHAR do_sound;         // 0 = no, 1 = yes
  UCHAR flashes;          // 0 = constant OR 1, 2, 3, 4
  UCHAR beeps;            // 0 = constant OR 4
  UCHAR notify_minutes;   // How often 15, 30, 45, 60 minutes
  UCHAR notify_hours[24]; // 0 = NO, 1 = YES for each hour
} MSG_SETUP;

typedef struct
{
  UCHAR dollars;
  UCHAR cents;
  UCHAR description[25];
  UCHAR rating;
  UCHAR length;
  UCHAR hour;
  UCHAR min;
  UCHAR purchase;
} PAY_PER_VIEW;

typedef struct
{
  UCHAR dollars;
  UCHAR cents;
  UCHAR description[25];
  UCHAR subscribe;
} PREMIUM_SERVICE;

typedef struct
{
  USHORT device_id;
  UCHAR status;
  UCHAR under_load_ctrl;
} DEVICE_STATUS;

#define MAX_LOAD_CTRLS 10

typedef struct
{
  time_t start_time;
  UCHAR level;
  USHORT duration; // in minutes
} LOAD_CTRL;

typedef struct
{
  USHORT device_id;
  time_t start_time;
  time_t stop_time;
} LOAD_CTRL_OVERRIDE;

typedef struct
{
  USHORT id;         // id of active mood
  USHORT duration;   // length of time mood should be active
                     //                  (0 = forever)
  time_t start_time; // time when mood activated
} ACTIVE_MOOD;

typedef struct
{
  USHORT VisualDeviceID;
  UCHAR VisualMinutesOn;
  UCHAR VisualMinutesOff;
  UCHAR VisualHours[3];

  USHORT AudibleDeviceID;
  UCHAR AudibleMinutesOn;
  UCHAR AudibleMinutesOff;
  UCHAR AudibleHours[3];
  UCHAR Expansion[6];
} EMAIL_INDICATORS_T;

////////////////////////////////////////
// Screen Colors
////////////////////////////////////////
#define BG_TRANSPARENT 0x060

#define BG_BLK 0x000
#define BG_BLU 0x010
#define BG_GRN 0x020
#define BG_CYN 0x030
#define BG_RED 0x040
#define BG_MAG 0x050
//#define BG_BRN         0x060 // given up for transparent
#define BG_WHT 0x070
#define BG_GRY 0x080
#define BG_LBLU 0x090
#define BG_LGRN 0x0A0
#define BG_LCYN 0x0B0
#define BG_LRED 0x0C0
#define BG_LMAG 0x0D0
#define BG_YEL 0x0E0
#define BG_HWHT 0x0F0

#define FG_BLK 0x000
#define FG_BLU 0x001
#define FG_GRN 0x002
#define FG_CYN 0x003
#define FG_RED 0x004
#define FG_MAG 0x005
#define FG_BRN 0x006
#define FG_WHT 0x007
#define FG_GRY 0x008
#define FG_LBLU 0x009
#define FG_LGRN 0x00A
#define FG_LCYN 0x00B
#define FG_LRED 0x00C
#define FG_LMAG 0x00D
#define FG_YEL 0x00E
#define FG_HWHT 0x00F

extern UCHAR far* get_window(USHORT, USHORT, USHORT, USHORT);

extern void put_window(UCHAR far*);

extern void text(UCHAR*, USHORT);

extern void ticks_set(USHORT, ULONG);

extern USHORT ticks_check(USHORT);

extern void fillz(UCHAR*, USHORT, UCHAR);

/////////////////////////////////////////
// Message type
/////////////////////////////////////////
#define INTERVAL_30S 100
#define INTERVAL_15M 101
#define ALERT_MSG 102
#define NEW_RATE_MSG 103
#define INTERVAL_30S$ 104

/////////////////////////////////////////
// Table Type
/////////////////////////////////////////
#define VERSION_TABLE 1
#define CUSTOMER_TABLE 2
#define DEVICE_TABLE 3
#define EVENT_TABLE 4
#define METER_TABLE 5
#define TIME_TABLE 6
#define HISTORY_TABLE 7
#define VOLTS_TABLE 8
#define M_M_TABLE 10
//#define WEATHER_TABLE         11
#define EMAIL_INDICATOR 12
#define DEVICE_HISTORY_TABLE 13
#define LOGGED_ON_TABLE 14
#define DAYS_RATES_TABLE 15
#define LOAD_SHED_TABLE 16
#define DEVICE_STATUS_TABLE 17
#define LOAD_SHED_OVR_TABLE 18
#define MOOD_TABLE 19
#define ALERT_TABLE 20
#define MSG_SETUP_TABLE 21
#define DAWN_DUSK_TABLE 22
#define HOLIDAY_TABLE 23
#define DAY_TYPE_TABLE 24
#define PANIC_TABLE 25
#define BILLING_TABLE 26
#define RESTORE_DEVICES_TABLE 27
#define RESTORE_EVENTS_TABLE 28
#define GET_FILE_TABLE 29
#define GET_SPECIAL_TABLE 30
#define DELETE_FILE_TABLE 31
#define EMAIL_TABLE 32
#define DELETE_MSG_TABLE 33
#define VL_HISTORY_TABLE 34
#define SHORT_HISTORY_TABLE 35
#define ACTIVE_MOODS_LIST 36
#define EXT_MOOD_TABLE 37
#define GET_DIR_TABLE 38
#define DO_MKDIR 39
#define DO_RMDIR 40
#define FILE_RENAME_TABLE 41
#define FILE_COPY_TABLE 42
#define UPDATE_TIMES_TABLE 43
#define TRIGGER_TABLE 44
#define SECURITY_TABLE 45
#define MOOD_STATE_TABLE 46
#define VERSION_TABLE2 47
#define PAGER_DEBUG_FUNCT 48
#define RATE_LEVEL_POINTS 49
//#define TSTAT_OFFSET_TABLE   50
//#define HVAC_CHG_TABLE         51
#define SEASON_MODE_TABLE 52
#define MASTER_MODE_TABLE 53
#define OCCUPIED_MODE_TABLE 54
#define HVAC_MODE_TABLE 55
#define IMMEDIATE_CTRL_TBL 56
#define ALL_DEVICES_STATUS 57
#define PASSWORD_TBL 58
#define FAST_HISTORY_TBL 59
#define CAll_ALT_NO_NOW 60
#define GET_SYSTEM_MODES_TBL 61
#define EXECUTE_SYSTEM_RESET 62
#define ALERT_CONFIGURATION 63
#define GET_FILE_SIZE 64
#define GET_FILE_BLOCK 65
#define GET_TABLE_SIZE 66
#define GET_TABLE_BLOCK 67
#define CALL_ALT_NUM_NOW 68
#define PHILLIPS_TABLE 128

////////////////////////////////////////////////
// Device Categories
////////////////////////////////////////////////
#define ALL_CATEGORIES 0
#define LIGHTING_CATEGORY 1
#define ENVIRONMENTAL_CATEGORY 2
#define POOL_SPA_CATEGORY 3
#define SPRINKLER_CATEGORY 4
#define ALERT_DEVICE_CATEGORY 5
#define SECURITY 6

////////////////////////////////////////////////
// X10 Device Types
////////////////////////////////////////////////

#define X10_RELAY 0
#define X10_DIMMER 1
#define X10_ANNUNCIATOR 2
#define X10_RCS_TSTAT 3
#define X10_RCS_2WAY 4
#define X10_DECORA 5
#define X10_RCS_CENTI_TSTAT 6

////////////////////////////////////////////////
// X10 Device Types
////////////////////////////////////////////////

#define CEBUS_DIMMER 2
#define CEBUS_RELAY 3
#define CEBUS_LOAD_SHEDDER 4
#define CEBUS_OLD_MM 5
#define CEBUS_NEW_MM 6

////////////////////////////////////////////////
// Source of device's current state
////////////////////////////////////////////////

#define SM_REGULAR_EVENT 1
#define SM_DEVICE_RESET 8
#define SM_END_LOADCTRL 2
#define SM_LOADCTRL 3
#define SM_SMART_SCHED 4
#define SM_ENDMOOD6 6

#define SM_IMMEDIATE 11
#define SM_PROTECTED_MOOD 12
#define SM_UNPROTECTED_MOOD 13
#define SM_END_MOOD 14

// "reasons" for device reset
#define SM_TRIGGER 20
#define SM_SECURITY_MODE 21
#define SM_SECURITY_ALARM 22
#define SM_NEW_RATE 23
#define SM_REBOOT 24
#define SM_REQUEST 25

#define SM_XFI_IMMED_CTRL 26
#define SM_XFI_MOOD 27

#define SM_EMAIL_ALERT 30
#endif
