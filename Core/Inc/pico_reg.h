#ifndef __PICO_REG_H
#define __PICO_REG_H

#include <stdint.h>

#define PICO_UUID_SIZE_BYTES 8
#define PICO_REG_COMMON_BUF_SIZE 2048

struct _pico_registers_st {
  union _offset_0 {
    struct {
      uint8_t VER_MAJOR;      // 0-1B
      uint8_t VER_MINOR;      // 1-1B
      uint8_t CPLD_VER_MAJOR; // 2-1B
      uint8_t CPLD_VER_MINOR; // 3-1B
    } _;
    uint32_t raw;
  } version;
  union _offset_4 {
    struct {
      uint16_t pout;
      uint16_t vref;
    } _;
    uint32_t raw;
  } voltage;
  uint8_t UUID[PICO_UUID_SIZE_BYTES];           // 4-8B
  uint8_t PEER_IP[4];                           // 12-4B client ip address
  uint32_t UP_SECOND;                           // 16-4B up time second
  uint32_t POOL_GAP_US;                         // 20-4B 系统主循环扫描平均间隔时间/每秒
  uint8_t SYS_ERR;                              // 24-1B 系统上一次ERR Code
  uint8_t reserv2[3];                           // 25-3B
  uint8_t reserv3[2];                           // 28-2B
  uint8_t MAC[6];                               // 30-6B < Source Mac Address
  uint8_t IP[4];                                // 36-4B < Source IP Address
  uint8_t SN[4];                                // 40-4B < Subnet Mask
  uint8_t GW[4];                                // 44-4B < Gateway IP Address
  uint8_t DNS[4];                               // 48-4B < DNS server IP Address
  uint8_t DHCP;                                 // 52-1B < 1 - Static, 2 - DHCP
  uint8_t REV0;                                 // 53-1B W5100S 0x003C
  uint8_t REV1;                                 // 54-1B
  uint8_t reserv4;                              // 55-1B
  uint8_t reserv5[72];                          // 56-72B
  uint8_t cmd;                                  // 128-1B
  uint8_t status;                               // 129-1B
  uint8_t last_cmd;                             // 130-1B
  uint8_t REV2;                                 // 131-1B
  uint8_t common_buf[PICO_REG_COMMON_BUF_SIZE]; // 132-2048B common tx rx buffer
} __attribute__((packed));
typedef struct _pico_registers_st pico_registers_st;

extern pico_registers_st pico_reg;

int pico_reg_read(uint32_t addr, uint8_t *buf, uint32_t len);
int pico_reg_write(uint32_t addr, const uint8_t *buf, uint32_t len);

#endif
