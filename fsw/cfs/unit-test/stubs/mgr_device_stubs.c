#include "utgenstub.h"
#include "mgr_device.h"

int32_t MGR_ReadData(uart_info_t *device, uint8_t *read_data, uint8_t data_length)
{
    UT_GenStub_SetupReturnBuffer(MGR_ReadData, int32_t);

    UT_GenStub_AddParam(MGR_ReadData, uart_info_t *, device);
    UT_GenStub_AddParam(MGR_ReadData, uint8_t *, read_data);
    UT_GenStub_AddParam(MGR_ReadData, uint8_t, data_length);

    UT_GenStub_Execute(MGR_ReadData, Basic, NULL);

    return UT_GenStub_GetReturnValue(MGR_ReadData, int32_t);
}

int32_t MGR_CommandDevice(uart_info_t *device, uint8_t cmd, uint32_t payload)
{
    UT_GenStub_SetupReturnBuffer(MGR_CommandDevice, int32_t);

    UT_GenStub_AddParam(MGR_CommandDevice, uart_info_t *, device);
    UT_GenStub_AddParam(MGR_CommandDevice, uint8_t, cmd);
    UT_GenStub_AddParam(MGR_CommandDevice, uint32_t, payload);

    UT_GenStub_Execute(MGR_CommandDevice, Basic, NULL);

    return UT_GenStub_GetReturnValue(MGR_CommandDevice, int32_t);
}

int32_t MGR_RequestHK(uart_info_t *device, MGR_Device_HK_tlm_t *data)
{
    UT_GenStub_SetupReturnBuffer(MGR_RequestHK, int32_t);

    UT_GenStub_AddParam(MGR_RequestHK, uart_info_t *, device);
    UT_GenStub_AddParam(MGR_RequestHK, MGR_Device_HK_tlm_t *, data);

    UT_GenStub_Execute(MGR_RequestHK, Basic, NULL);

    return UT_GenStub_GetReturnValue(MGR_RequestHK, int32_t);
}

int32_t MGR_RequestData(uart_info_t *device, MGR_Device_Data_tlm_t *data)
{
    UT_GenStub_SetupReturnBuffer(MGR_RequestData, int32_t);

    UT_GenStub_AddParam(MGR_RequestData, uart_info_t *, device);
    UT_GenStub_AddParam(MGR_RequestData, MGR_Device_Data_tlm_t *, data);

    UT_GenStub_Execute(MGR_RequestData, Basic, NULL);

    return UT_GenStub_GetReturnValue(MGR_RequestData, int32_t);
}
