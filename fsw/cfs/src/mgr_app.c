/*******************************************************************************
** File: mgr_app.c
**
** Purpose:
**   This file contains the source code for the MGR application.
**
*******************************************************************************/

/*
** Include Files
*/
#include <arpa/inet.h>
#include "mgr_app.h"

/*
** Global Data
*/
MGR_AppData_t MGR_AppData;

/*
** Application entry point and main process loop
*/
void MGR_AppMain(void)
{
    int32 status = OS_SUCCESS;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(MGR_PERF_ID);

    /*
    ** Perform application initialization
    */
    status = MGR_AppInit();
    if (status != CFE_SUCCESS)
    {
        MGR_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** Main loop
    */
    while (CFE_ES_RunLoop(&MGR_AppData.RunStatus) == true)
    {
        /*
        ** Performance log exit stamp
        */
        CFE_ES_PerfLogExit(MGR_PERF_ID);

        /*
        ** Pend on the arrival of the next Software Bus message
        ** Note that this is the standard, but timeouts are available
        */
        status = CFE_SB_ReceiveBuffer((CFE_SB_Buffer_t **)&MGR_AppData.MsgPtr, MGR_AppData.CmdPipe,
                                      CFE_SB_PEND_FOREVER);

        /*
        ** Begin performance metrics on anything after this line. This will help to determine
        ** where we are spending most of the time during this app execution.
        */
        CFE_ES_PerfLogEntry(MGR_PERF_ID);

        /*
        ** If the CFE_SB_ReceiveBuffer was successful, then continue to process the command packet
        ** If not, then exit the application in error.
        ** Note that a SB read error should not always result in an app quitting.
        */
        if (status == CFE_SUCCESS)
        {
            MGR_ProcessCommandPacket();
        }
        else
        {
            CFE_EVS_SendEvent(MGR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "MGR: SB Pipe Read Error = %d",
                              (int)status);
            MGR_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Disable component, which cleans up the interface, upon exit
    */
    MGR_Disable();

    /*
    ** Performance log exit stamp
    */
    CFE_ES_PerfLogExit(MGR_PERF_ID);

    /*
    ** Exit the application
    */
    CFE_ES_ExitApp(MGR_AppData.RunStatus);
}

/*
** Initialize application
*/
int32 MGR_AppInit(void)
{
    int32 status = OS_SUCCESS;

    MGR_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY); /* as default, no filters are used */
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("MGR: Error registering for event services: 0x%08X\n", (unsigned int)status);
        return status;
    }

    /*
    ** Create the Software Bus command pipe
    */
    status = CFE_SB_CreatePipe(&MGR_AppData.CmdPipe, MGR_PIPE_DEPTH, "MGR_CMD_PIPE");
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(MGR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "Error Creating SB Pipe,RC=0x%08X",
                          (unsigned int)status);
        return status;
    }

    /*
    ** Subscribe to ground commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MGR_CMD_MID), MGR_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(MGR_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error Subscribing to HK Gnd Cmds, MID=0x%04X, RC=0x%08X", MGR_CMD_MID,
                          (unsigned int)status);
        return status;
    }

    /*
    ** Subscribe to housekeeping (hk) message requests
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MGR_REQ_HK_MID), MGR_AppData.CmdPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(MGR_SUB_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error Subscribing to HK Request, MID=0x%04X, RC=0x%08X", MGR_REQ_HK_MID,
                          (unsigned int)status);
        return status;
    }

    /*
    ** TODO: Subscribe to any other messages here
    */

    /*
    ** Initialize the published HK message - this HK message will contain the
    ** telemetry that has been defined in the MGR_HkTelemetryPkt for this app.
    */
    CFE_MSG_Init(CFE_MSG_PTR(MGR_AppData.HkTelemetryPkt.TlmHeader), CFE_SB_ValueToMsgId(MGR_HK_TLM_MID),
                 MGR_HK_TLM_LNGTH);

    /*
    ** Initialize the device packet message
    ** This packet is specific to your application
    */
    CFE_MSG_Init(CFE_MSG_PTR(MGR_AppData.DevicePkt.TlmHeader), CFE_SB_ValueToMsgId(MGR_DEVICE_TLM_MID),
                 MGR_DEVICE_TLM_LNGTH);

    /*
    ** TODO: Initialize any other messages that this app will publish
    */

    /*
    ** Always reset all counters during application initialization
    */
    MGR_ResetCounters();

    /*
    ** Initialize application data
    ** Note that counters are excluded as they were reset in the previous code block
    */
    MGR_AppData.HkTelemetryPkt.DeviceEnabled          = MGR_DEVICE_DISABLED;
    MGR_AppData.HkTelemetryPkt.DeviceHK.DeviceCounter = 0;
    MGR_AppData.HkTelemetryPkt.DeviceHK.DeviceConfig  = 0;
    MGR_AppData.HkTelemetryPkt.DeviceHK.DeviceStatus  = 0;

    /*
     ** Send an information event that the app has initialized.
     ** This is useful for debugging the loading of individual applications.
     */
    status = CFE_EVS_SendEvent(MGR_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION,
                               "MGR App Initialized. Version %d.%d.%d.%d", MGR_MAJOR_VERSION,
                               MGR_MINOR_VERSION, MGR_REVISION, MGR_MISSION_REV);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("MGR: Error sending initialization event: 0x%08X\n", (unsigned int)status);
    }
    return status;
}

/*
** Process packets received on the MGR command pipe
*/
void MGR_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_GetMsgId(MGR_AppData.MsgPtr, &MsgId);
    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        /*
        ** Ground Commands with command codes fall under the MGR_CMD_MID (Message ID)
        */
        case MGR_CMD_MID:
            MGR_ProcessGroundCommand();
            break;

        /*
        ** All other messages, other than ground commands, add to this case statement.
        */
        case MGR_REQ_HK_MID:
            MGR_ProcessTelemetryRequest();
            break;

        /*
        ** All other invalid messages that this app doesn't recognize,
        ** increment the command error counter and log as an error event.
        */
        default:
            MGR_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(MGR_PROCESS_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MGR: Invalid command packet, MID = 0x%x", CFE_SB_MsgIdToValue(MsgId));
            break;
    }
    return;
}

/*
** Process ground commands
** TODO: Add additional commands required by the specific component
*/
void MGR_ProcessGroundCommand(void)
{
    int32             status      = OS_SUCCESS;
    CFE_SB_MsgId_t    MsgId       = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /*
    ** MsgId is only needed if the command code is not recognized. See default case
    */
    CFE_MSG_GetMsgId(MGR_AppData.MsgPtr, &MsgId);

    /*
    ** Ground Commands, by definition, have a command code (_CC) associated with them
    ** Pull this command code from the message and then process
    */
    CFE_MSG_GetFcnCode(MGR_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        /*
        ** NOOP Command
        */
        case MGR_NOOP_CC:
            /*
            ** First, verify the command length immediately after CC identification
            ** Note that VerifyCmdLength handles the command and command error counters
            */
            if (MGR_VerifyCmdLength(MGR_AppData.MsgPtr, sizeof(MGR_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                /* Second, send EVS event on successful receipt ground commands*/
                CFE_EVS_SendEvent(MGR_CMD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "MGR: NOOP command received");
                /* Third, do the desired command action if applicable, in the case of NOOP it is no operation */
            }
            break;

        /*
        ** Reset Counters Command
        */
        case MGR_RESET_COUNTERS_CC:
            if (MGR_VerifyCmdLength(MGR_AppData.MsgPtr, sizeof(MGR_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(MGR_CMD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "MGR: RESET counters command received");
                MGR_ResetCounters();
            }
            break;

        /*
        ** Enable Command
        */
        case MGR_ENABLE_CC:
            if (MGR_VerifyCmdLength(MGR_AppData.MsgPtr, sizeof(MGR_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(MGR_CMD_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "MGR: Enable command received");
                MGR_Enable();
            }
            break;

        /*
        ** Disable Command
        */
        case MGR_DISABLE_CC:
            if (MGR_VerifyCmdLength(MGR_AppData.MsgPtr, sizeof(MGR_NoArgs_cmd_t)) == OS_SUCCESS)
            {
                CFE_EVS_SendEvent(MGR_CMD_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "MGR: Disable command received");
                MGR_Disable();
            }
            break;

        /*
        ** TODO: Edit and add more command codes as appropriate for the application
        ** Set Configuration Command
        ** Note that this is an example of a command that has additional arguments
        */
        case MGR_CONFIG_CC:
            if (MGR_VerifyCmdLength(MGR_AppData.MsgPtr, sizeof(MGR_Config_cmd_t)) == OS_SUCCESS)
            {
                uint32_t config = ntohl(
                    ((MGR_Config_cmd_t *)MGR_AppData.MsgPtr)
                        ->DeviceCfg); // command is defined as big-endian... need to convert to host representation
                CFE_EVS_SendEvent(MGR_CMD_CONFIG_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "MGR: Configuration command received: %u", config);
                /* Command device to send HK */
                status = MGR_CommandDevice(&MGR_AppData.MgrUart, MGR_DEVICE_CFG_CMD, config);
                if (status == OS_SUCCESS)
                {
                    MGR_AppData.HkTelemetryPkt.DeviceCount++;
                }
                else
                {
                    MGR_AppData.HkTelemetryPkt.DeviceErrorCount++;
                }
            }
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            MGR_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(MGR_CMD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MGR: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x",
                              CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}

/*
** Process Telemetry Request - Triggered in response to a telemetery request
** TODO: Add additional telemetry required by the specific component
*/
void MGR_ProcessTelemetryRequest(void)
{
    CFE_SB_MsgId_t    MsgId       = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    /* MsgId is only needed if the command code is not recognized. See default case */
    CFE_MSG_GetMsgId(MGR_AppData.MsgPtr, &MsgId);

    /* Pull this command code from the message and then process */
    CFE_MSG_GetFcnCode(MGR_AppData.MsgPtr, &CommandCode);
    switch (CommandCode)
    {
        case MGR_REQ_HK_TLM:
            MGR_ReportHousekeeping();
            break;

        case MGR_REQ_DATA_TLM:
            MGR_ReportDeviceTelemetry();
            break;

        /*
        ** Invalid Command Codes
        */
        default:
            /* Increment the error counter upon receipt of an invalid command */
            MGR_AppData.HkTelemetryPkt.CommandErrorCount++;
            CFE_EVS_SendEvent(MGR_DEVICE_TLM_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MGR: Invalid command code for packet, MID = 0x%x, cmdCode = 0x%x",
                              CFE_SB_MsgIdToValue(MsgId), CommandCode);
            break;
    }
    return;
}

/*
** Report Application Housekeeping
*/
void MGR_ReportHousekeeping(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (MGR_AppData.HkTelemetryPkt.DeviceEnabled == MGR_DEVICE_ENABLED)
    {
        status = MGR_RequestHK(&MGR_AppData.MgrUart,
                                  (MGR_Device_HK_tlm_t *)&MGR_AppData.HkTelemetryPkt.DeviceHK);
        if (status == OS_SUCCESS)
        {
            MGR_AppData.HkTelemetryPkt.DeviceCount++;
        }
        else
        {
            MGR_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(MGR_REQ_HK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MGR: Request device HK reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */

    /* Time stamp and publish housekeeping telemetry */
    CFE_SB_TimeStampMsg((CFE_MSG_Message_t *)&MGR_AppData.HkTelemetryPkt);
    CFE_SB_TransmitMsg((CFE_MSG_Message_t *)&MGR_AppData.HkTelemetryPkt, true);
    return;
}

/*
** Collect and Report Device Telemetry
*/
void MGR_ReportDeviceTelemetry(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (MGR_AppData.HkTelemetryPkt.DeviceEnabled == MGR_DEVICE_ENABLED)
    {
        status = MGR_RequestData(&MGR_AppData.MgrUart,
                                    (MGR_Device_Data_tlm_t *)&MGR_AppData.DevicePkt.Mgr);
        if (status == OS_SUCCESS)
        {
            MGR_AppData.HkTelemetryPkt.DeviceCount++;
            /* Time stamp and publish data telemetry */
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *)&MGR_AppData.DevicePkt);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *)&MGR_AppData.DevicePkt, true);
        }
        else
        {
            MGR_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(MGR_REQ_DATA_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MGR: Request device data reported error %d", status);
        }
    }
    /* Intentionally do not report errors if disabled */
    return;
}

/*
** Reset all global counter variables
*/
void MGR_ResetCounters(void)
{
    MGR_AppData.HkTelemetryPkt.CommandErrorCount = 0;
    MGR_AppData.HkTelemetryPkt.CommandCount      = 0;
    MGR_AppData.HkTelemetryPkt.DeviceErrorCount  = 0;
    MGR_AppData.HkTelemetryPkt.DeviceCount       = 0;
    return;
}

/*
** Enable Component
** TODO: Edit for your specific component implementation
*/
void MGR_Enable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is disabled */
    if (MGR_AppData.HkTelemetryPkt.DeviceEnabled == MGR_DEVICE_DISABLED)
    {
        /*
        ** Initialize hardware interface data
        ** TODO: Make specific to your application depending on protocol in use
        ** Note that other components provide examples for the different protocols available
        */
        MGR_AppData.MgrUart.deviceString  = MGR_CFG_STRING;
        MGR_AppData.MgrUart.handle        = MGR_CFG_HANDLE;
        MGR_AppData.MgrUart.isOpen        = PORT_CLOSED;
        MGR_AppData.MgrUart.baud          = MGR_CFG_BAUDRATE_HZ;
        MGR_AppData.MgrUart.access_option = uart_access_flag_RDWR;

        /* Open device specific protocols */
        status = uart_init_port(&MGR_AppData.MgrUart);
        if (status == OS_SUCCESS)
        {
            MGR_AppData.HkTelemetryPkt.DeviceCount++;
            MGR_AppData.HkTelemetryPkt.DeviceEnabled = MGR_DEVICE_ENABLED;
            CFE_EVS_SendEvent(MGR_ENABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "MGR: Device enabled");
        }
        else
        {
            MGR_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(MGR_UART_INIT_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MGR: UART port initialization error %d", status);
        }
    }
    else
    {
        MGR_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(MGR_ENABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "MGR: Device enable failed, already enabled");
    }
    return;
}

/*
** Disable Component
** TODO: Edit for your specific component implementation
*/
void MGR_Disable(void)
{
    int32 status = OS_SUCCESS;

    /* Check that device is enabled */
    if (MGR_AppData.HkTelemetryPkt.DeviceEnabled == MGR_DEVICE_ENABLED)
    {
        /* Open device specific protocols */
        status = uart_close_port(&MGR_AppData.MgrUart);
        if (status == OS_SUCCESS)
        {
            MGR_AppData.HkTelemetryPkt.DeviceCount++;
            MGR_AppData.HkTelemetryPkt.DeviceEnabled = MGR_DEVICE_DISABLED;
            CFE_EVS_SendEvent(MGR_DISABLE_INF_EID, CFE_EVS_EventType_INFORMATION, "MGR: Device disabled");
        }
        else
        {
            MGR_AppData.HkTelemetryPkt.DeviceErrorCount++;
            CFE_EVS_SendEvent(MGR_UART_CLOSE_ERR_EID, CFE_EVS_EventType_ERROR, "MGR: UART port close error %d",
                              status);
        }
    }
    else
    {
        MGR_AppData.HkTelemetryPkt.DeviceErrorCount++;
        CFE_EVS_SendEvent(MGR_DISABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "MGR: Device disable failed, already disabled");
    }
    return;
}

/*
** Verify command packet length matches expected
*/
int32 MGR_VerifyCmdLength(CFE_MSG_Message_t *msg, uint16 expected_length)
{
    int32             status        = OS_SUCCESS;
    CFE_SB_MsgId_t    msg_id        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t cmd_code      = 0;
    size_t            actual_length = 0;

    CFE_MSG_GetSize(msg, &actual_length);
    if (expected_length == actual_length)
    {
        /* Increment the command counter upon receipt of an invalid command */
        MGR_AppData.HkTelemetryPkt.CommandCount++;
    }
    else
    {
        CFE_MSG_GetMsgId(msg, &msg_id);
        CFE_MSG_GetFcnCode(msg, &cmd_code);

        CFE_EVS_SendEvent(MGR_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%X,  CC = %d, Len = %ld, Expected = %d",
                          CFE_SB_MsgIdToValue(msg_id), cmd_code, actual_length, expected_length);

        status = OS_ERROR;

        /* Increment the command error counter upon receipt of an invalid command */
        MGR_AppData.HkTelemetryPkt.CommandErrorCount++;
    }
    return status;
}
