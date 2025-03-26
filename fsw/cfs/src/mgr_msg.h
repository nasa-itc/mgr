/*******************************************************************************
** File:
**   mgr_msg.h
**
** Purpose:
**  Define MGR application commands and telemetry messages
**
*******************************************************************************/
#ifndef _MGR_MSG_H_
#define _MGR_MSG_H_

#include "cfe.h"
#include "mgr_device.h"

/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define MGR_NOOP_CC           0
#define MGR_RESET_COUNTERS_CC 1
#define MGR_ENABLE_CC         2
#define MGR_DISABLE_CC        3
#define MGR_CONFIG_CC         4

/*
** Telemetry Request Command Codes
** TODO: Add additional commands required by the specific component
*/
#define MGR_REQ_HK_TLM   0
#define MGR_REQ_DATA_TLM 1

/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} MGR_NoArgs_cmd_t;

/*
** MGR write configuration command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32                  DeviceCfg;

} MGR_Config_cmd_t;

/*
** MGR device telemetry definition
*/
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    MGR_Device_Data_tlm_t  Mgr;

} __attribute__((packed)) MGR_Device_tlm_t;
#define MGR_DEVICE_TLM_LNGTH sizeof(MGR_Device_tlm_t)

/*
** MGR housekeeping type definition
*/
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8                     CommandErrorCount;
    uint8                     CommandCount;
    uint8                     DeviceErrorCount;
    uint8                     DeviceCount;

    /*
    ** TODO: Edit and add specific telemetry values to this struct
    */
    uint8                  DeviceEnabled;
    MGR_Device_HK_tlm_t DeviceHK;

} __attribute__((packed)) MGR_Hk_tlm_t;
#define MGR_HK_TLM_LNGTH sizeof(MGR_Hk_tlm_t)

#endif /* _MGR_MSG_H_ */
