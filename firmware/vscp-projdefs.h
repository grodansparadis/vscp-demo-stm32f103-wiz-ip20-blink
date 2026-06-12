/*
  projdefs.h

  This file contains project definitions for the VSCP TCP/IP link protocol code.
*/

#ifndef _VSCP_PROJDEFS_H_
#define _VSCP_PROJDEFS_H_

/**
  ----------------------------------------------------------------------------
                             Blinky
  ----------------------------------------------------------------------------
*/

// Uncomment to get debug printouts on USART2
#define VSCP_ENABLE_BLINKY_DEBUG

/**
 * Maximum number of simultaneous TCP/IP connections
 * This is the maximum simultaneous number
 * of connections to the server
 */
#define BLINKY_MAX_TCP_CONNECTIONS 1

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
//#define VSCP_FWHLP_CRYPTO_USE_OPENSSL

// ON ESP32 we use the built in crypto support
//#define VSCP_FWHLP_CRYPTO_USE_PSA_CRYPTO

// Enable JSON support in firmware helper. 
//#define VSCP_FWHLP_JSON_SUPPORT

// Enable XML support in firmware helper.
//#define VSCP_FWHLP_XML_SUPPORT

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
#define THIS_FIRMWARE_GUID                                                                                             \
  { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x08, 0xdc, 0x12, 0x34, 0x56, 0x00, 0x01 }

/**
 * URL to MDF file (max 32 bytes
 */
#define THIS_FIRMWARE_MDF_URL "eurosource.se/blinky0.mdf"

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