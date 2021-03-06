#ifndef _LVPS_MANAGER_H
#define _LVPS_MANAGER_H

#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#ifndef __APPLE__
#include <error.h>
#include "aDIO_library.h"
#endif /* __APPLE__ */
#include "log.h"

#define ONE_MILLISEC 1000

/* ON/OFF signal bit addresses */
#define HK_PORT_ON 0x01 /* P0.0 - 0000 0001 */
#define HK_PORT_OFF 0x02 /* P0.1  - 0000 0010 */
#define ZYNQ_PORT_ON 0x04 /* P0.2  - 0000 0100 */
#define ZYNQ_PORT_OFF 0x08 /* P0.3  - 0000 1000 */
#define CAMERA_PORT_ON 0x10 /* P0.4  - 0001 0000 */
#define CAMERA_PORT_OFF 0x20 /* P0.5  - 0010 0000 */

/* CHECK signal bit addresses */
#define HK_PORT_CHECK 0x0B /* P1.0  - 0000 1011 */
#define ZYNQ_PORT_CHECK 0x0C /* P1.1  - 0000 1100 */
#define CAMERA_PORT_CHECK 0x0D /* P1.2  - 0000 1101 */

#define PORT1_INPUT 0x00

#define HIGH_VAL 0xFF
#define LOW_VAL 0x00

/**
 * handles the switching on/off of instrument subsytems using the CPU's 
 * aDIO ports and the aDIO library.
 * all port definitions are given in LvpsManager.h
 */
class LvpsManager {
public:

  /**
   * status options for a subsystem
   */
  enum Status : uint8_t {
    OFF = 0,
    ON = 1,
    UNDEF = 2,
  };
  /**
   * subsystems that can be switched on/off
   */
  enum SubSystem : uint8_t {
    ZYNQ = 0,
    CAMERAS = 1,
    HK = 2,
  };

  /**
   * stores the current Zynq status
   */
  Status zynq_status;
  /**
   * stores the current cameras status
   */
  Status cam_status;
  /**
   * stores the current HK status, currently not connected
   */
  Status hk_status;

  /**
   * stores the last read bits from Port0
   */
  uint8_t P1Bits[4];
  
  LvpsManager();
  Status GetStatus(SubSystem sub_system);
  int SwitchOn(SubSystem sub_system);
  int SwitchOff(SubSystem sub_system);
  bool Check(SubSystem sub_system);

private:
  /**
   * the aDIO minor number for use with the aDIO library functions
   */
  static const uint32_t minor_number = 0;
#ifndef __APPLE__
  /**
   * the aDIO device handle
   */
  DeviceHandle aDIO_Device;
#endif /* __APPLE__ */
  /**
   * options to write to the digital port
   */
  enum PortValue : uint8_t {
    HIGH = 0xFF,
    LOW = 0x00,
  };
  
  int InitPorts();
  int CloseDev();
  int SetDirP1(uint8_t port_config);
  int ReadP1();
  int SetDirP0(uint8_t port_config);
  int SetValP0(PortValue port_value);
  int SetPulseP0(uint8_t port_config);

};


#endif /* LVPS_MANAGER */
