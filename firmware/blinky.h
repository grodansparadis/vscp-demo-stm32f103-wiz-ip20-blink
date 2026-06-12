/*
  File: tcpsrv.c

  VSCP Wireless CAN4VSCP Gateway (VSCP-WCANG)

  This file is part of the VSCP (https://www.vscp.org)

  The MIT License (MIT)
  Copyright (C) 2022-2026 Ake Hedman, the VSCP project <info@vscp.org>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef __VSCPBLINK_H__
#define __VSCPBLINK_H__

#include <vscp.h>
#include <vscp-fifo.h>
#include <vscp-firmware-helper.h>
#include <vscp-link-protocol.h>
#include <vscp-firmware-level2.h>

// Buffer
#define TCPIP_BUF_MAX_SIZE (2048u)

/**
 * VSCP TCP link protocol character buffer size
 */
#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE 512
#endif

/**
 * Max number of events in the receive fifo
 */
#define RECEIVE_FIFO_SIZE 16

/**
 * Max number of events in each of the transmit fifos
 */
#define TRANSMIT_FIFO_SIZE 16

#define TCPSRV_WELCOME_MSG                                                                                             \
  "Welcome to the VSCP CAN4VSCP Gateway\r\n"                                                                           \
  "Copyright (C) 2000-2026 Grodans Paradis AB\r\n"                                                                     \
  "https://www.grodansparadis.com\r\n"                                                                                 \
  "+OK\r\n"

// System defaults

#define DEFAULT_NODE_NAME        "VSCP CAN4VSCP Gateway"
#define DEFAULT_ENCRYPTION_LEVEL VSCP_ENCRYPTION_NONE // 0 = none, 1 = AES128, 2 = AES192, 3 = AES256
#define DEFAULT_MODULE_ZONE      0                    // VSCP zone for module
#define DEFAULT_MODULE_SUBZONE   0                    // VSCP subzone for module

#define DEFAULT_VSCP_LINK_PORT     9598
#define DEFAULT_VSCP_LINK_USER     "vscp"
#define DEFAULT_VSCP_LINK_PASSWORD "secret"

typedef struct {

  // Module
  char nodeName[32];   // User name for node
  uint8_t nodeZone;    // VSCP zone for node
  uint8_t nodeSubzone; // VSCP subzone for node
  uint8_t guid[16];    // GUID for node (default: Constructed from MAC address)
  uint8_t encryptLvl;  // Encryption level for UDP messages (0 = none, 1 = AES128, 2 = AES192, 3 = AES256)
  uint8_t pmk[16];     // System security key for encryption (AES128)
  uint8_t pmkLen;      // For future use, Now always 16 (AES128)
  uint32_t bootCnt;    // Number of restarts (not editable)

  // VSCP link protocol
  uint16_t vscplinkPort;                             // VSCP link protocol port
  char vscplinkUser[VSCP_LINK_MAX_USER_NAME_LENGTH]; // VSCP link protocol user
  char vscplinkPw[VSCP_LINK_MAX_PASSWORD_LENGTH];    // VSCP link protocol password

} node_persistent_config_t;

/*
  Socket context
  This is the context for each open socket/channel.
  Here there is only one possible channel open at the
  same time, but in a more complex implementation there could be
  multiple channels
*/
typedef struct _ctx {
  int id;           // Context ID (index in context array) Here always zero
  int sock;         // Socket (here always zero)
  int binary;       // Binary mode if non zero, text mode if zero
  uint8_t guid[16]; // GUID for client (copied from node GUID, can be used to track which client is which if multiple
                    // clients are supported)
  char user[VSCP_LINK_MAX_USER_NAME_LENGTH]; // Username storage
  vscp_fifo_t fifoEventsIn;                  // VSCP event receive fifo
  vscp_fifo_t fifoEventsOut;                 // VSCP event send fifo
  unsigned  bValidated : 1;                            // User is validated
  unsigned  bRcvLoop : 1;                              // Receive loop is enabled if non zero
  uint8_t privLevel;                         // User privilege level 0-15
  vscpEventFilter filter;                    // Filter for events sent to client
  vscp_statistics_t statistics;              // VSCP Statistics
  vscp_status_t status;                      // VSCP status
  uint32_t last_rcvloop_time;                // Time in milliseconds of last received event
} ctx_t;

// Message for connection when max number of clients is reached
#define MSG_MAX_CLIENTS "Max number of clients reached. Disconnecting.\r\n"

// This string can be  displayed when the user types "HELP" or "?" in the command
// line interface if the standard help text takes to much resources.
#define BLINKY_HELP "See https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_tcpiplink for help\r\n"

#endif