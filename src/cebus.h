/*   Copyright 1996 TeCom Inc. All rights reserved                  */
/******************************************************************************
** CEBus DOSAPI SOFTWARE
**------------------------------------------------------------------------------
** Copyright(c)1993 by TECO Energy Management Services Corporation.
**       ALL RIGHTS RESERVED.
** CEBus is a protected Certification Mark of the Electronics Industries Assocation.
**
**   This Information is proprietary to TECO Energy Management Services
** Corporation, Tampa, Florida and MAY NOT be copied by any method or
** incorporated into another program without the express written consent of
** TECO Energy Management Services Corporation. This Information or any portion
** thereof remains the property of TECO Energy Management Services Corporation.
** The Information contained herein is believed to be accurate and TECO Energy
** Management Services Corporation assumes no responsibility or
** liability for its use in any way and convey no license or title under any
** patent or copyright and make no representation or warranty that this
** Information is free from patent or copyright infringement.
**------------------------------------------------------------------------------
** cebus.h        EIA/IS-60 Structures, Types and Codes defined
**
** NOTES:
** This header file defines the prototypes necessary to interface through this API
** to the ISA CENode device. Refer to EIA/IS-60 for detailed information on the field
** contents outlined below.
**------------------------------------------------------------------------------
** written by:
**       Steven J. Ackerman
**       Consultant
**       ACS
**       4276 Lago Way
**       Sarasota, FL 34241
**
** date      Comments
** ------------------------
** 9-20-93   First written.
** 3-20-95   Changes for A6 CENode firmware.
********************************************************************************
*/

/*************** TYPE DEFINITIONS ********************************************/

typedef unsigned char u_int8;
typedef unsigned int u_int16;
typedef unsigned long u_int32;

typedef enum { CERROR = -1,
               CFALSE = 0,
               CTRUE = 1 } BOOLEAN;

#define CEB_DATA_BYTES 32 /* maximum size of CEBpacket INFO field */
#define IACK_DATA_BYTES 4 /* maximum size of IACKpacket INFO field */

/*
** CEBpacket structure
**
** This structure defines the format of a maximum length CEBus packet and is
** incorporated in the PKT_INFO structure.
*/
typedef struct {

  /* B7   - reserved
   ** B6   - service class
   ** B5   - IR/RF level
   ** B4-B3- packet priority
   ** B2-B0- packet type */
  u_int8 CONTROL;

  /* destination address */
  u_int16 DA;

  /* destination house code */
  u_int16 DHC;

  /* source address */
  u_int16 SA;

  /* source house code */
  u_int16 SHC;

  /* CEBus Data including NPDU & APDU */
  u_int8 INFO[CEB_DATA_BYTES];

} CEBpacket;

/*
** IACKpacket structure
**
** This structure defines the format of a maximum length CEBus IACK packet, this
** is the structure iack pointed to in calls to CEBUS_GetTransmitStatusExtended.
*/
typedef struct {

  /* B7   - reserved
   ** B6   - service class
   ** B5   - IR/RF level
   ** B4-B3- packet priority
   ** B2-B0- packet type */
  u_int8 CONTROL;

  /* destination address */
  u_int16 DA;

  /* destination house code */
  u_int16 DHC;

  /* CEBus Data including NPDU & APDU */
  u_int8 INFO[IACK_DATA_BYTES];

} IACKpacket;

/*
** PKT_INFO structure
**
** This structure encapsulates a CEBpacket with its actual length in bytes
** and is used to pass CEBus packets to and from the CEBus driver via
** the CEBUS__GetPacket, CEBUS_GetPacketExtended, CEBUS__SendPacket,
** CEBUS__SendNPDU and CEBUS__SendHeader
** API calls.
*/

typedef struct {

  /* the packet */
  CEBpacket pkt;

  /* its length in bytes */
  u_int8 length;

} PKT_INFO;

/*
** CEBUS__GetPacketExtended() receive status bits
**
** These bits are returned in the status parameter pointed to in the call to
** CEBUS_GetPacketExtended.
*/
#define RCV_REFUSED 0x01   /* indicates valid packet refused */
#define RCV_OVERRUN 0x02   /* indicates valid packet discarded */
#define RCV_ADDR_DIFF 0x04 /* indicates current packet has different address */

/*
** CENode DLL Service Level
*/
typedef enum {
  STANDARD_MODE, /* Data Link */
  MONITOR_MODE   /* Monitor */
} MODE_TYPE;

/*
** Transmission status values from CEBUS__GetTransmitStatus
**
** These values are returned from a CEBUS_GetTransmitStatus call in
** the XMIT_STAT parameter pointed to in the call. They are combined
** with the XMIT_STAT_EXTENDED values in a call to CEBUS_GetTransmitStatusExtended
*/
typedef enum {
  PENDING, /* packet is pending transmission */
  IDLE,    /* packet is idle */
  SUCCESS, /* packet successfully transmitted */
  FAILURE  /* packet failed transmission */
} XMIT_STAT;

/*
** Additional transmission status values from CEBUS__GetTransmitStatusExtended
**
** These values are returned in the high byte of the XMIT_STAT parameter pointed
** to when the low byte indicates XMIT_STAT of FAILURE.
*/
typedef enum {
  NOFAIL,               /* no failure detected */
  REMOTE_BUSY,          /* remote was busy, details in IACKpacket */
  REMOTE_REJECT,        /* remote cannot accept, details in IACKpacket */
  EXCESSIVE_RETRIES,    /* exceeded CH_ACCESS_NUM_ACK without IACK */
  EXCESSIVE_COLLISIONS, /* exceeded MAX_RESTARTS attempts */
  NO_IACK,              /* no iack received in time expected */
  BAD_IACK,             /* bad iack received, frag or bad crc in IACK time */
  WRONG_IACK,           /* illegal type code or sequence number for IACK type */
  UNKNOWN_FAILURE,      /* unknown failure */
  PHYSICAL_FAILURE,     /* physical layer failure */
  TRANSMIT_ABORTED,     /* transmission was aborted by host */
  MAX_RETRANS_EXCEEDED  /* exceeded MAX_RESTARTS attempts */
} XMIT_STAT_EXTENDED;

/*
** CEBUS_CONFIG structure
**
** This structure is used to configure the CEBus driver Data Link Layer via
** the CEBUS_InitDevice & CEBUS_ReadConfiguration API calls.
*/
typedef struct {

  /* Data Link Layer Group Address */
  u_int16 GROUP_ADDR_MASK;

  /* Data Link Layer Mode of Operation */
  MODE_TYPE MODE;

  /* Data Link Layer maximum restart attempts */
  u_int8 MAX_RESTARTS;

  /* Data Link Layer maximum transmit retries */
  u_int8 MAX_RETRIES;

} CEBUS_CONFIG;

/*
** CEBUS_CONFIG_EXTENDED structure
**
** This structure is used to configure the CEBus driver Data Link Layer via
** the CEBUS_InitDeviceExtended & CEBUS_ReadConfigurationExtended API calls.
*/
typedef struct {

  /* Data Link Layer Group Address */
  u_int16 GROUP_ADDR_MASK;

  /* Data Link Layer Mode of Operation */
  MODE_TYPE MODE;

  /* Data Link Layer maximum restart attempts */
  u_int8 MAX_RESTARTS;

  /* Data Link Layer maximum transmit retries */
  u_int8 MAX_RETRIES;

  /* enable/disable attention sequence for link status conditions */
  /*  and statistic counter overflows */
  BOOLEAN LINK_STATUS_ENABLE;

  /* enable/disable immediate retry for */
  /*  CEB_ADR_ACK_DATA and CEB_ADR_UNACK_DATA packets */
  BOOLEAN NUM_RETRIES;

  /* enable/disable multiple channel access retries for */
  /*  CEB_ADR_ACK_DATA and CEB_ADR_UNACK_DATA packets */
  BOOLEAN AUTO_RETRY;

  /* number CEB_ADR_ACK_DATA packets and channel accesses for */
  /*  each Data Link transmit requested */
  /*  0=1, 1=2, 2=4, 3=8 channel accesses */
  u_int8 CH_ACCESS_NUM_ACK;

  /* number CEB_ADR_UNACK_DATA packets and channel accesses for */
  /*  each Data Link transmit requested */
  /*  0=1, 1=2, 2=4, 3=8 channel accesses */
  u_int8 CH_ACCESS_NUM_UNACK;

  /* minimum required time between transmit channel attempts */
  /*  0=0mS, 1=100mS, 2=200mS, 3=300mS */
  u_int8 CH_ACCESS_PERIOD;

  /* rf sideband control */
  /*  0 = normal */
  /*  1 = dynamic sideband select */
  /*  2 = fixed upper sideband */
  /*  3 = fixed lower sideband */
  u_int8 RF_SIDEBAND_CONTROL;

  /* reserved, must be zero */
  u_int8 RESERVED_0;

} CEBUS_CONFIG_EXTENDED;

/*
** STAT_INFO structure
**
** This structure is used to retrieve the CEBus driver Data Link Layer statistics via
** the CEBUS_GetStatistics API call.
*/
typedef struct {
  u_int8 frag_counter;
  u_int8 bad_crc_counter;
  u_int8 backoff_counter;
  u_int8 retry_counter;
} STAT_INFO;

/********************* CONSTANTS **************************************/

/*
** Control Field (per EIA/IS-60)
*/

/* Packet Type */
#define CEB_IACK 0x00
#define CEB_ACK_DATA 0x01
#define CEB_UNACK_DATA 0x02
#define CEB_RESERVED_TYPE 0x03
#define CEB_FAILURE 0x04
#define CEB_ADDR_ACK_DATA 0x05
#define CEB_ADDR_IACK 0x06
#define CEB_ADDR_UNACK_DATA 0x07

/* Packet Priority */
#define CEB_HIGH_PRIORITY 0x00
#define CEB_STANDARD_PRIORITY 0x08
#define CEB_DEFERRED_PRIORITY 0x10

/* IR/RF level */
#define CEB_WIRED_LEVEL 0x00
#define CEB_UNWIRED_LEVEL 0x20

/* service class */
#define CEB_BASIC_SERVICE 0x00
#define CEB_EXTENDED_SERVICE 0x40

/* sequence number */
#define CEB_SEQUENCE 0x80

/*
**  Network Layer Protocol Data Unit (NPDU) constants
*/
#define CEB_BROUTER_1_ABSENT 0x00
#define CEB_BROUTER_1_PRESENT 0x01

#define CEB_BROUTER_2_ABSENT 0x00
#define CEB_BROUTER_2_PRESENT 0x02

#define CEB_NO_ALLOWED_MEDIA 0x00
#define CEB_ALLOWED_MEDIA 0x04

#define CEB_NONEXTENDED_SERVICE 0x00
#define CEB_EXTENDED_NPDU_SERVICE 0x08

#define CEB_NOT_SEGMENTED_SVC 0x10
#define CEB_SEGMENTED_FIRST_PKT 0x00

#define CEB_REQUEST_ID_ROUTING 0x00
#define CEB_ID_PACKET_ROUTING 0x20
#define CEB_DIRECTORY_ROUTING 0x40
#define CEB_FLOOD_ROUTING 0x60

#define CEB_UNPRIVILEGED 0x00
#define CEB_PRIVILEGED 0x80

/*
** Application Layer Protocol Data Unit (APDU) constants
*/

/* APDU Type */
#define CEB_ERROR_TYPE 0x00
#define CEB_REJECT_TYPE 0x08
#define CEB_RESULT_TYPE 0x10
#define CEB_RECEIPT_ACK_TYPE 0x18
#define CEB_IMPLICIT_INVOKE_TYPE 0x20
#define CEB_EXPLICIT_INVOKE_TYPE 0x28
#define CEB_CONDITIONAL_INVOKE_TYPE 0x30
#define CEB_EXPLICIT_RETRY_TYPE 0x38

/* APDU Format */
#define CEB_BASIC_MODE_BF 0xc0
#define CEB_PRIV_MODE_PF 0x40
#define CEB_VAR_LENGTH_BV 0x80
#define CEB_PRIV_VAR_LENGTH_PV 0x00

/************** Function Prototypes *********************/

/* open hardware CCM interface */
BOOLEAN CEBUS__InitPort(u_int16 port, u_int16 intnumber);

/* close hardware CCM interface */
void CEBUS__ClosePort(void);

/* initialize (configure) CCM device */
void CEBUS__InitDevice(u_int16 shc, u_int16 sa, CEBUS_CONFIG* config);
void CEBUS__InitDeviceExtended(u_int16 shc,
                               u_int16 sa,
                               CEBUS_CONFIG_EXTENDED* config);

/* retrieve CCM device configuration */
void CEBUS__ReadConfiguration(u_int16* shc, u_int16* sa, CEBUS_CONFIG* config);
void CEBUS__ReadConfigurationExtended(u_int16* shc,
                                      u_int16* sa,
                                      CEBUS_CONFIG_EXTENDED* config);

/* enable/disable receipt of ADR_ACK/ADR_UNACK packets */
void CEBUS__ReceiveEnable(void);
void CEBUS__ReceiveDisable(void);

/* retrieve any received packets */
BOOLEAN CEBUS__GetPacket(PKT_INFO* pkt_info);
BOOLEAN CEBUS__GetPacketExtended(PKT_INFO* pkt_info,
                                 unsigned long* time,
                                 unsigned int* status);

/* send packets */
unsigned long CEBUS__SendPacket(PKT_INFO* pkt_info);
unsigned long CEBUS__SendNPDU(PKT_INFO* pkt_info);
unsigned long CEBUS__SendHeader(PKT_INFO* pkt_info);

/* abort any current and queued transmission packets */
void CEBUS__AbortTransmission(void);

/* retrieve transmitted packet's status */
BOOLEAN CEBUS__GetTransmitStatus(unsigned long id, XMIT_STAT* status);
BOOLEAN CEBUS__GetTransmitStatusExtended(unsigned long id,
                                         XMIT_STAT* status,
                                         IACKpacket* iack);

/* retrieve CCM device statistics */
void CEBUS__GetStatistics(STAT_INFO* stat_info);

/* Write a new byte value to Register 46 */
void CEBUS__WriteRegister46(u_int16 reg46val);
