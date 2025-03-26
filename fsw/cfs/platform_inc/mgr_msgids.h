/************************************************************************
** File:
**   $Id: mgr_msgids.h  $
**
** Purpose:
**  Define MGR Message IDs
**
*************************************************************************/
#ifndef _MGR_MSGIDS_H_
#define _MGR_MSGIDS_H_

/*
** CCSDS V1 Command Message IDs (MID) must be 0x18xx
*/
#define MGR_CMD_MID 0x1917

/*
** This MID is for commands telling the app to publish its telemetry message
*/
#define MGR_REQ_HK_MID 0x1918

/*
** CCSDS V1 Telemetry Message IDs must be 0x08xx
*/
#define MGR_HK_TLM_MID     0x0917
#define MGR_DEVICE_TLM_MID 0x0918

#endif /* _MGR_MSGIDS_H_ */
