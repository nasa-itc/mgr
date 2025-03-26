/************************************************************************
** File:
**    mgr_events.h
**
** Purpose:
**  Define MGR application event IDs
**
*************************************************************************/

#ifndef _MGR_EVENTS_H_
#define _MGR_EVENTS_H_

/* Standard app event IDs */
#define MGR_RESERVED_EID        0
#define MGR_STARTUP_INF_EID     1
#define MGR_LEN_ERR_EID         2
#define MGR_PIPE_ERR_EID        3
#define MGR_SUB_CMD_ERR_EID     4
#define MGR_SUB_REQ_HK_ERR_EID  5
#define MGR_PROCESS_CMD_ERR_EID 6

/* Standard command event IDs */
#define MGR_CMD_ERR_EID         10
#define MGR_CMD_NOOP_INF_EID    11
#define MGR_CMD_RESET_INF_EID   12
#define MGR_CMD_ENABLE_INF_EID  13
#define MGR_ENABLE_INF_EID      14
#define MGR_ENABLE_ERR_EID      15
#define MGR_CMD_DISABLE_INF_EID 16
#define MGR_DISABLE_INF_EID     17
#define MGR_DISABLE_ERR_EID     18

/* Device specific command event IDs */
#define MGR_CMD_CONFIG_INF_EID 20

/* Standard telemetry event IDs */
#define MGR_DEVICE_TLM_ERR_EID 30
#define MGR_REQ_HK_ERR_EID     31

/* Device specific telemetry event IDs */
#define MGR_REQ_DATA_ERR_EID 32

/* Hardware protocol event IDs */
#define MGR_UART_INIT_ERR_EID  40
#define MGR_UART_CLOSE_ERR_EID 41

#endif /* _MGR_EVENTS_H_ */
