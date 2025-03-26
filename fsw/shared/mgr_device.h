/*******************************************************************************
** File: mgr_device.h
**
** Purpose:
**   This is the header file for the MGR device.
**
*******************************************************************************/
#ifndef _MGR_DEVICE_H_
#define _MGR_DEVICE_H_

/*
** Required header files.
*/
#include "device_cfg.h"
#include "hwlib.h"
#include "mgr_platform_cfg.h"

/*
** Type definitions
** TODO: Make specific to your application
*/
#define MGR_DEVICE_HDR   0xDEAD
#define MGR_DEVICE_HDR_0 0xDE
#define MGR_DEVICE_HDR_1 0xAD

#define MGR_DEVICE_NOOP_CMD     0x00
#define MGR_DEVICE_REQ_HK_CMD   0x01
#define MGR_DEVICE_REQ_DATA_CMD 0x02
#define MGR_DEVICE_CFG_CMD      0x03

#define MGR_DEVICE_TRAILER   0xBEEF
#define MGR_DEVICE_TRAILER_0 0xBE
#define MGR_DEVICE_TRAILER_1 0xEF

#define MGR_DEVICE_HDR_TRL_LEN 4
#define MGR_DEVICE_CMD_SIZE    9

/*
** MGR device housekeeping telemetry definition
*/
typedef struct
{
    uint32_t DeviceCounter;
    uint32_t DeviceConfig;
    uint32_t DeviceStatus;

} __attribute__((packed)) MGR_Device_HK_tlm_t;
#define MGR_DEVICE_HK_LNGTH sizeof(MGR_Device_HK_tlm_t)
#define MGR_DEVICE_HK_SIZE  MGR_DEVICE_HK_LNGTH + MGR_DEVICE_HDR_TRL_LEN

/*
** MGR device data telemetry definition
*/
typedef struct
{
    uint32_t DeviceCounter;
    uint16_t DeviceDataX;
    uint16_t DeviceDataY;
    uint16_t DeviceDataZ;

} __attribute__((packed)) MGR_Device_Data_tlm_t;
#define MGR_DEVICE_DATA_LNGTH sizeof(MGR_Device_Data_tlm_t)
#define MGR_DEVICE_DATA_SIZE  MGR_DEVICE_DATA_LNGTH + MGR_DEVICE_HDR_TRL_LEN

/*
** Prototypes
*/
int32_t MGR_ReadData(uart_info_t *device, uint8_t *read_data, uint8_t data_length);
int32_t MGR_CommandDevice(uart_info_t *device, uint8_t cmd, uint32_t payload);
int32_t MGR_RequestHK(uart_info_t *device, MGR_Device_HK_tlm_t *data);
int32_t MGR_RequestData(uart_info_t *device, MGR_Device_Data_tlm_t *data);

#endif /* _MGR_DEVICE_H_ */
