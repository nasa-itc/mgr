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
** CCSDS V1 Command Message IDs (MID)
*/
#define MGR_CMD_MID 0x1917

/*
** This MID is for commands telling the app to publish its telemetry message
*/
#define MGR_REQ_HK_MID 0x1918

/*
** CCSDS V1 Telemetry Message IDs
*/
#define MGR_HK_TLM_MID     0x0917

#endif /* _MGR_MSGIDS_H_ */
