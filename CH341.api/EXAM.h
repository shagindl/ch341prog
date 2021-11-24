#pragma once

#include	<windows.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    BOOL WINAPI IIC_IssueStart(ULONG iIndex);
    BOOL WINAPI	IIC_IssueStop(ULONG	iIndex);
    BOOL WINAPI	IIC_OutBlockSkipAck(ULONG iIndex, ULONG iOutLength, PVOID iOutBuffer);
    BOOL WINAPI	IIC_OutByteCheckAck(ULONG iIndex, UCHAR iOutByte);
    BOOL WINAPI	IIC_InBlockByAck(ULONG iIndex, ULONG iInLength, PVOID oInBuffer);
    BOOL WINAPI	IIC_InByteNoAck(ULONG iIndex, PUCHAR oInByte);

#ifdef __cplusplus
}
#endif /* __cplusplus */