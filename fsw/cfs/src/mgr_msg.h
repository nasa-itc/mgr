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

/*
** Ground Command Codes
** TODO: Add additional commands required by the specific component
*/
#define MGR_NOOP_CC           0
#define MGR_RESET_COUNTERS_CC 1
#define MGR_SET_MODE_CC       2
#define MGR_BOOT_SUCCESS_CC   3
#define MGR_REBOOT_PREP_CC    4

/*
** Telemetry Request Command Codes
*/
#define MGR_REQ_HK_TLM   0

/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} MGR_NoArgs_cmd_t;

/*
** MGR housekeeping type definition
*/
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8                     CommandErrorCount;
    uint8                     CommandCount;
    uint8                     SpacecraftMode;
    uint32                    BootCounter;
    uint32                    AnomRebootCtr;
    int64                     TicsMET;

} __attribute__((packed)) MGR_Hk_tlm_t;
#define MGR_HK_TLM_LNGTH sizeof(MGR_Hk_tlm_t)

#endif /* _MGR_MSG_H_ */
