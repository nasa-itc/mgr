/*******************************************************************************
** File: mgr_app.h
**
** Purpose:
**   This is the main header file for the MGR application.
**
*******************************************************************************/
#ifndef _MGR_APP_H_
#define _MGR_APP_H_

/*
** Include Files
*/
#include "cfe.h"
#include "device_cfg.h"
#include "mgr_events.h"
#include "mgr_platform_cfg.h"
#include "mgr_perfids.h"
#include "mgr_msg.h"
#include "mgr_msgids.h"
#include "mgr_version.h"

/*
** Specified pipe depth - how many messages will be queued in the pipe
*/
#define MGR_PIPE_DEPTH 32

/* 
** Spacecraft Modes
*/
#define MGR_SAFE_MODE 1
#define MGR_SAFE_REBOOT_MODE 2
#define MGR_SCIENCE_ACTIVE_MODE 3
#define MGR_SCIENCE_IDLE_MODE 4
#define MGR_SCIENCE_REBOOT_MODE 5

/*
** MGR global data structure
** The cFE convention is to put all global app data in a single struct.
** This struct is defined in the `mgr_app.h` file with one global instance
** in the `.c` file.
*/
typedef struct
{
    /*
    ** Housekeeping telemetry packet
    ** Each app defines its own packet which contains its OWN telemetry
    */
    MGR_Hk_tlm_t HkTelemetryPkt; /* MGR Housekeeping Telemetry Packet */

    /*
    ** Operational data  - not reported in housekeeping
    */
    CFE_MSG_Message_t *MsgPtr;    /* Pointer to msg received on software bus */
    CFE_SB_PipeId_t    CmdPipe;   /* Pipe Id for HK command pipe */
    uint32             RunStatus; /* App run status for controlling the application state */

} MGR_AppData_t;

/*
** Exported Data
** Extern the global struct in the header for the Unit Test Framework (UTF).
*/
extern MGR_AppData_t MGR_AppData; /* MGR App Data */

/*
**
** Local function prototypes.
**
** Note: Except for the entry point (MGR_AppMain), these
**       functions are not called from any other source module.
*/
void  MGR_AppMain(void);
int32 MGR_AppInit(void);
void  MGR_ProcessCommandPacket(void);
void  MGR_ProcessGroundCommand(void);
void  MGR_ProcessTelemetryRequest(void);
void  MGR_ReportHousekeeping(void);
void  MGR_ResetCounters(void);
void  MGR_SaveHkFile(MGR_Hk_tlm_t* hk);
void  MGR_RestoreHkFile(MGR_Hk_tlm_t* hk);
int32 MGR_VerifyCmdLength(CFE_MSG_Message_t *msg, uint16 expected_length);

#endif /* _MGR_APP_H_ */
