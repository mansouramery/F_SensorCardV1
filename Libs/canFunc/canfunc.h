#ifndef CAN_FUNCS_
#define CAN_FUNCS_
#include "can.h"
#include "CAN_CONF.h"
#include "stdbool.h"
#include "stdio.h"
#include <string.h>
#define CAN_BUFFER_SIZE 32

extern CAN_HandleTypeDef hcan;

typedef struct {
    uint32_t id;
    uint8_t data[8];
} CAN_Message;

void CAN_Sub_MNG( volatile CAN_Message* CAN_mesages, volatile uint8_t  CANWriteIndex);
int CAN_Pub(uint16_t MsgId, void*Msg, uint8_t len);

#endif