/************************************************************************
** File:
**   $Id: mgr_platform_cfg.h  $
**
** Purpose:
**  Define mgr Platform Configuration Parameters
**
** Notes:
**
*************************************************************************/
#ifndef _MGR_PLATFORM_CFG_H_
#define _MGR_PLATFORM_CFG_H_

/*
** Default MGR Configuration
*/
#ifndef MGR_CFG
/* Notes:
**   NOS3 uart requires matching handle and bus number
*/
#define MGR_CFG_STRING      "usart_16"
#define MGR_CFG_HANDLE      16
#define MGR_CFG_BAUDRATE_HZ 115200
#define MGR_CFG_MS_TIMEOUT  50 /* Max 255 */
/* Note: Debug flag disabled (commented out) by default */
//#define MGR_CFG_DEBUG
#endif

#endif /* _MGR_PLATFORM_CFG_H_ */
