/*
  projdefs.h

  This file contains project definitions for the VSCP TCP/IP link protocol code.
*/

#ifndef _VSCP_PROJDEFS_H_
#define _VSCP_PROJDEFS_H_

/*
  ----------------------------------------------------------------------------
                             Blinky
  ----------------------------------------------------------------------------
*/

/*!
  Select one of the following modes for the blinky demo. This will determine how the
  blinky demo is built and will operate. You can only select one mode at a time. If you select
  more than one mode, you will get a compile error. If you do not select any mode,
  you will get a compile error.
*/
#define BLINKY_MODE_TCP_SERVER  // VSCP TCP/IP link protocol server mode. The device will listen for incoming TCP/IP connections and respond to them. This is the default mode for the blinky demo.
// #define BLINKY_DEMO_MODE_TCP_CLIENT // VSCP TCP/IP link protocol client mode. The device will connect to a TCP/IP server and send data to it.
// #define BLINKY_DEMO_MODE_UDP // VSCP UDP link protocol mode. The device will listen for incoming UDP packets and respond to them.
// #define BLINKY_DEMO_MODE_MULTICAST // VSCP multicast link protocol mode. The device will listen for incoming multicast packets and respond to them.
// #define BLINKY_DEMO_MODE_MQTT // VSCP MQTT link protocol mode. The device will connect to an MQTT broker and publish/subscribe to topics.

// Uncomment to get debug printouts on USART2
#define VSCP_ENABLE_BLINKY_DEBUG

/**
 * Maximum number of simultaneous TCP/IP connections
 * This is the maximum simultaneous number
 * of connections to the server
 */
#define BLINKY_MAX_TCP_CONNECTIONS 1



/*
  ----------------------------------------------------------------------------
                          WIZnet IP20 settings
  ----------------------------------------------------------------------------

WIZnet IP20 settings. These are used to set up the WIZnet IP20 module. You can
change these settings to match your network configuration. Make sure to set the
MAC address to something unique on your network.

https://docs.wiznet.io/Product/Chip/MCU/Pre-programmed-MCU/W55RP20-S2E/command-manual-en#un
*/

// #define WIZ_IP20_MAC                  0x00, 0x08, 0xdc, 0x37, 0x89, 0xb4
#define WIZ_IP20_IP                   "192.168.1.88"  // LI192.168.11.2\r\n
#define WIZ_IP20_NETMASK              "255.255.255.0" // SM255.255.255.0\r\n
#define WIZ_IP20_GATEWAY              "192.168.1.1"   // GW192.168.11.1\r\n
#define WIZ_IP20_DNS                  "192.168.1.1"   // DS8.8.8.8\r\n
#define WIZ_IP20_IP_ALLOCATION_METHOD "0"             // IM0 = static, IM1 = DHCP, IM2 = PPPoE
#define WIZ_IP20_PORT                 "9598"          // LP5000\r\n

// Operation mode

#ifdef BLINKY_MODE_TCP_SERVER
  #define WIZ_IP20_OPERATION_MODE "1" // OP1\r\n TCP server mode
#elif defined(BLINKY_DEMO_MODE_TCP_CLIENT)
  #define WIZ_IP20_OPERATION_MODE "0" // OP0\r\n TCP client mode
#elif defined(BLINKY_DEMO_MODE_UDP)
  #define WIZ_IP20_OPERATION_MODE "3" // OP3\r\n UDP mode
#elif defined(BLINKY_DEMO_MODE_MULTICAST)
  #define WIZ_IP20_OPERATION_MODE "3" // OP3\r\n Multicast mode  
#elif defined(BLINKY_DEMO_MODE_MQTT)
  #define WIZ_IP20_OPERATION_MODE "5" // OP5\r\n MQTT mode  
#else
  #error "You must define WIZ_IP20_OPERATION_MODE based on the selected mode"
#endif

// Remote host (client) settings (VSCP TCP/IP link server should be running here)
#define WIZ_IP20_REMOTE_HOST_IP   "192.168.1.7" // RI192.168.1.100\r\n
#define WIZ_IP20_REMOTE_HOST_PORT "9598"          // RP5000\r\n

// Serial port settings for WIZnet IP20 module
#define WIZ_IP20_SERIAL_BAUDRATE "12" // BR12\r\n = 115200
#define WIZ_IP20_SERIAL_DATABITS "1"  // DB1\r\n = 8 data bits
#define WIZ_IP20_SERIAL_STOPBITS "0"  // SB0\r\n 0 = 1 stop bit, 1 = 2 stop bits
#define WIZ_IP20_SERIAL_PARITY   "0"  // PR0\r\n 0 = none, 1 = odd, 2 = even
#define WIZ_IP20_SERIAL_FLOWCTRL "0" // FL0\r\n 0 = none, 1 = XON/XOFF, 2 = RTS/CTS, 3 = RTS on TX, 4= RTS on TX(invert)
#define WIZ_IP20_SERIAL_ECHO     "0" // EC0\r\n 0 = not used, 1 = used

#define WIZ_IP20_LINK_PACKING_TIME "0" // PT0\r\n data packing time in milliseconds
#define WIZ_IP20_LINK_PACKING_SIZE "0" // PS0\r\n data packing size in bytes (0 not used)
#define WIZ_IP20_LINK_PACKING_CHAR                                                                                     \
  "0" // PD0a\r\n data packing char (0 not used, 0x0a = newline) The designated character is not included in data.
#define WIZ_IP20_LINK_INACTIVITY                "0"    // IT0\r\n inactivity time in seconds (0 not used)
#define WIZ_IP20_LINK_RETRY_CNT                 "3"    // TR0\r\n retry count (0 not used)
#define WIZ_IP20_LINK_KEEP_ALIVE                "1"    // KA0\r\n keep alive (0 not used, 1 = enabled)
#define WIZ_IP20_LINK_KEEP_ALIVE_INTERVAL       "1000" // KI1000\r\n keep alive interval in milliseconds
#define WIZ_IP20_LINK_KEEP_ALIVE_RETRY_INTERVAL "1000" // KE5000\r\n keep alive retry interval in milliseconds

#define WIZ_IP20_LINK_SSL_CLOSE "0" // SO0\r\n SSL close (0 not used, 1 = enabled)

// TCP client settings (not used here)
// #define WIZ_IP20_LINK_CLIENT_RECONNECT  "3000" // RI3000\r\n TCP client reconnect interval in milliseconds

#define WIZ_IP20_LINK_CONNECT_STR    "<CONNECT>\r\n"    // SD<CONNECT>\r\n string to send on new connection
#define WIZ_IP20_LINK_DISCONNECT_STR "<DISCONNECT>\r\n" // DD<DISCONNECT>\r\n string to send on disconnection
// #define WIZ_IP20_LINK_ETH_CONNECT_STR "\r\n" //SE\r\n string to send on new Ethernet connection (not used here)

// #define WIZ_IP20_TCPSRV_ENABLE_PASSWORD  "0" // CP0\r\n password for TCP server (0 not used)
// #define WIZ_IP20_TCPSRV_SET_PASSWORD  "secret" // NPsecret\r\n password for TCP server (max 8 characters)
// #define WIZ_IP20_SEARCH_ID_CODE  "vscp" // SPvscp\r\n search id code (max 8 characters)
// #define WIZ_IP20_DEBUG_ENABLE "0" // DG0\r\n enable debug messages (0 not used, 1 = enabled)

// MQTT settings (not used here)
#define WIZ_IP20_MQTT_USER       "vscp"      // QUmqtt_user\r\n MQTT username (max 128 characters)
#define WIZ_IP20_MQTT_PASSWORD   "secret"  // QPmqtt_password\r\n MQTT password (max 128 characters)
#define WIZ_IP20_MQTT_CLIENT_ID  "wiz_ip20" // QCmqtt_client_id\r\n MQTT client ID (max 128 characters)
#define WIZ_IP20_MQTT_KEEP_ALIVE "0"              // QK0\r\n MQTT keep alive (0 not used, 1 = enabled)
#define WIZ_IP20_MQTT_QOS        "0"              // Q0\r\n MQTT QoS (0, 1, or 2)
#define WIZ_IP20_MQTT_PUB_TOPIC  "vscp/{{guid}}/{{class}}/{{type}}"     // PUtopic\r\n MQTT publish topic (max 128 characters)
#define WIZ_IP20_MQTT_SUB_TOPIC0 "vscp/{{guid}}/input"    // U0topic\r\n MQTT subscribe topic 1 (max 128 characters)
#define WIZ_IP20_MQTT_SUB_TOPIC1 "mqtt/topic1"    // U1topic\r\n MQTT subscribe topic 2 (max 128 characters)
#define WIZ_IP20_MQTT_SUB_TOPIC2 "mqtt/topic2"    // U2topic\r\n MQTT subscribe topic 3 (max 128 characters)

/**
  ----------------------------------------------------------------------------
                        VSCP firmware helper defines
  ----------------------------------------------------------------------------
  Defines for firmware helper. These are used to link in support for
  features in the firmware helper. If you do not need support for a feature,
  you can save some memory by not linking it in.
*/

// We link in binary support from firmware helper.
#define VSCP_FWHLP_BINARY_FRAME_SUPPORT

// We link in crypto support from firmware helper.
#define VSCP_FWHLP_CRYPTO_SUPPORT

// Crypto support is needed for the TCP/IP link protocol
#define VSCP_FWHLP_CRYPTO_SUPPORT

// SSL support is needed for crypto support
// #define VSCP_FWHLP_CRYPTO_USE_OPENSSL

// ON ESP32 we use the built in crypto support
// #define VSCP_FWHLP_CRYPTO_USE_PSA_CRYPTO

// Enable JSON support in firmware helper.
// #define VSCP_FWHLP_JSON_SUPPORT

// Enable XML support in firmware helper.
// #define VSCP_FWHLP_XML_SUPPORT

/**
  ----------------------------------------------------------------------------
                       VSCP Link Protocol defines
  ----------------------------------------------------------------------------
*/

/*!
  Define to show custom help. The callback is called so you can respond
  with your custom help text.  This can be used to save memory if you work
  on a memory constraint environment.

  If not defined, standard help is shown.
*/
// #define VSCP_LINK_CUSTOM_HELP_TEXT

/*!
  Size for input buffer and outputbuffer for events.
  Must be at least one for each fifo
*/
#define VSCP_LINK_MAX_IN_FIFO_SIZE  (16u)
#define VSCP_LINK_MAX_OUT_FIFO_SIZE (16u)

/**
  ----------------------------------------------------------------------------
                       VSCP Firmware level II defines
  ----------------------------------------------------------------------------
  Defines for firmware level II
*/

/*!
  Name of device for level II capabilities announcement event.
  Max 32 characters.
*/
#define THIS_FIRMWARE_DEVICE_NAME "VSCP blinky tcp/ip link demo"

/**
 * Firmware version
 */
#define THIS_FIRMWARE_MAJOR_VERSION   (0u)
#define THIS_FIRMWARE_MINOR_VERSION   (0u)
#define THIS_FIRMWARE_RELEASE_VERSION (1u)
#define THIS_FIRMWARE_BUILD_VERSION   (0u)

/**
 * User id (this is only defaults)
 */
#define THIS_FIRMWARE_USER_ID0 (1u)
#define THIS_FIRMWARE_USER_ID1 (2u)
#define THIS_FIRMWARE_USER_ID2 (3u)
#define THIS_FIRMWARE_USER_ID3 (4u)
#define THIS_FIRMWARE_USER_ID4 (5u)

/**
 * Manufacturer id
 */
#define THIS_FIRMWARE_MANUFACTURER_ID0 (11u)
#define THIS_FIRMWARE_MANUFACTURER_ID1 (22u)
#define THIS_FIRMWARE_MANUFACTURER_ID2 (33u)
#define THIS_FIRMWARE_MANUFACTURER_ID3 (44u)

/**
 * Manufacturer subid
 */
#define THIS_FIRMWARE_MANUFACTURER_SUBID0 (55u)
#define THIS_FIRMWARE_MANUFACTURER_SUBID1 (66u)
#define THIS_FIRMWARE_MANUFACTURER_SUBID2 (77u)
#define THIS_FIRMWARE_MANUFACTURER_SUBID3 (88u)

/**
 * Set bootloader algorithm
 */
#define THIS_FIRMWARE_BOOTLOADER_ALGORITHM (0u)

/**
 * Device family code 32-bit
 */
#define THIS_FIRMWARE_DEVICE_FAMILY_CODE (0ul)

/**
 * Device type code 32-bit
 */
#define THIS_FIRMWARE_DEVICE_TYPE_CODE (0ul)

/**
  Interval for heartbeats in seconds
*/
#define THIS_FIRMWARE_INTERVAL_HEARTBEATS (60u)

/**
 * Interval for capabilities report in seconds
 */
#define THIS_FIRMWARE_INTERVAL_CAPS (60u)

/**
 * Buffer size
 */
#define THIS_FIRMWARE_BUFFER_SIZE VSCP_MAX(vscp.h)

/**
 * Enable logging
 */
#define THIS_FIRMWARE_ENABLE_LOGGING

/**
 * Enable error reporting
 */
#define THIS_FIRMWARE_ENABLE_ERROR_REPORTING

/**
 * @brief Uncomment to enable writing to write protected areas
 *
 * Writing manufacturer data and GUID
 */
#define THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS

/**
 * @brief Send server probe
 *
 */
#define THIS_FIRMWARE_VSCP_DISCOVER_SERVER

/**
 * GUID for this node (no spaces)
 */
//#define THIS_FIRMWARE_GUID                                                                                             \
//  { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x08, 0xdc, 0x12, 0x34, 0x56, 0x00, 0x01 }

/**
 * URL to MDF file (max 32 bytes
 */
#define THIS_FIRMWARE_MDF_URL "eurosource.se/blinky0.json"

/**
 * 16-bit firmware code for this device
 */
#define THIS_FIRMWARE_CODE (0)

/**
 * 16-bit firmware code for this device
 */
#define THIS_FIRMWARE_FAMILY_CODE (0)

/**
 * 16-bit firmware code for this device
 */
#define THIS_FIRMWARE_FAMILY_TYPE (0)

/// Buffer size for incoming TCP/IP data (not used here)
#define THIS_FIRMWARE_TCPIP_LINK_MAX_BUFFER         2048u
#define THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD 1

#endif // _VSCP_PROJDEFS_H_