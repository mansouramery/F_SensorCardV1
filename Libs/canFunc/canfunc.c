#include "canfunc.h"

volatile CAN_Message CAN_Buffer[CAN_BUFFER_SIZE];
volatile uint8_t CAN_WriteIndex = 0;
volatile uint8_t CAN_ReadIndex = 0;
uint8_t IndexCounterLast = 0;

void Critical_system_Control_handler(volatile CAN_Message *CAN_messages, uint8_t Index);
void NodePub_SensorCard1_ACK_handler(volatile CAN_Message *CAN_messages, uint8_t Index); 
void NodePub_KeepAlive_handler(volatile CAN_Message *CAN_messages, uint8_t Index); 
void NodePub_SensorCard1_MSG_handler(volatile CAN_Message *CAN_messages, uint8_t Index); 





void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan) {

  CAN_RxHeaderTypeDef rxHeader;
  uint8_t rxData[8];

  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, & rxHeader, rxData) != HAL_OK) {
    return; // Handle errors if necessary
  }

  // Buffer CAN message
  CAN_Buffer[CAN_WriteIndex].id = rxHeader.StdId;
  CAN_Buffer[CAN_WriteIndex].data[0] = rxData[0];
  CAN_Buffer[CAN_WriteIndex].data[1] = rxData[1];
  CAN_Buffer[CAN_WriteIndex].data[2] = rxData[2];
  CAN_Buffer[CAN_WriteIndex].data[3] = rxData[3];
  CAN_Buffer[CAN_WriteIndex].data[4] = rxData[4];
  CAN_Buffer[CAN_WriteIndex].data[5] = rxData[5];
  CAN_Buffer[CAN_WriteIndex].data[6] = rxData[6];
  CAN_Buffer[CAN_WriteIndex].data[7] = rxData[7];
  CAN_WriteIndex = (CAN_WriteIndex + 1) % CAN_BUFFER_SIZE;
switch(rxHeader.StdId){
    case NodePub_Main_ACK:
         cycle_sync = true; 
    break;
    case NodePub_SensorCard1_MSG:
       cycle_sync = true; 
    HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin); 
    break;
        
}  

}


void Critical_system_Control_handler(volatile CAN_Message *CAN_messages, uint8_t Index){
    
    
    
}

void NodePub_SensorCard1_ACK_handler(volatile CAN_Message *CAN_messages, uint8_t Index){
    
    
}

void NodePub_KeepAlive_handler(volatile CAN_Message *CAN_messages, uint8_t Index){
    
}

void NodePub_SensorCard1_MSG_handler(volatile CAN_Message *CAN_messages, uint8_t Index){
    
}


void MessageExtract(volatile CAN_Message *CAN_messages, uint8_t Index) {
   
switch (CAN_messages[Index].id){
    case Critical_system_Control:       
      Critical_system_Control_handler(CAN_messages,Index);
    break;
    case NodePub_SensorCard1_ACK:
      //NodeMainSub_RFID_MSG_handler(CAN_messages,Index);   
      NodePub_SensorCard1_ACK_handler(CAN_messages,Index); 
      cycle_sync = true;    
        break;     
    case NodePub_KeepAlive:
      NodePub_KeepAlive_handler(CAN_messages,Index);        
        break;
    case NodePub_SensorCard1_MSG:
      NodePub_SensorCard1_MSG_handler(CAN_messages,Index);  
    cycle_sync = true;
HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);    
        break;

    default:
        break;
        
    }
    
    CAN_messages[Index].id = 0;  // Clear message ID after processing
}



void CAN_Sub_MNG( volatile CAN_Message* CAN_mesages, volatile uint8_t  CANWriteIndex){
    uint8_t IndexCounter = 0;
  uint8_t CAN_Index = CANWriteIndex;
  if (IndexCounterLast != CAN_Index) {
    IndexCounter = IndexCounterLast;
    while (IndexCounter != CAN_Index) {
      //printf("\nCan_Packet1_Index=%d--->%s\n", IndexCounter, CAN_mesages[IndexCounter].data);
      MessageExtract(CAN_mesages, IndexCounter);

      IndexCounter = (IndexCounter + 1) % CAN_BUFFER_SIZE;
    }
    IndexCounterLast = IndexCounter; // equls to CAN_Index
  } else if (CAN_mesages[IndexCounterLast].id != 0) {
    IndexCounter = IndexCounterLast;
    do {
      //printf("\nCan_Packet2_Index=%d--->%s\n", IndexCounter, CAN_mesages[IndexCounter].data);
      MessageExtract(CAN_mesages, IndexCounter);

      IndexCounter = (IndexCounter + 1) % CAN_BUFFER_SIZE;
    }
    while (IndexCounter != CAN_Index);

  }
     
}

int CAN_Pub(uint16_t MsgId, void*Msg, uint8_t len){
    if (Msg == NULL) return -1;
//    if (HAL_CAN_GetState(&hcan) != HAL_CAN_STATE_READY) {
//        return -4;
//    }
    CAN_TxHeaderTypeDef txHeader;
    memset(&txHeader, 0, sizeof(txHeader));
    
    uint32_t txMailbox = 0;    
    if (len > 8) len = 8;  // Limit to CAN frame size

    txHeader.DLC = len;
    txHeader.StdId = MsgId;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.TransmitGlobalTime = DISABLE;

    uint16_t waite = 0;
    while ((HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) && (waite < 20)) {
        waite++;
        HAL_Delay(1);
    }

    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0) {
        return -2; // timeout
    }

    if (HAL_CAN_AddTxMessage(&hcan, &txHeader,(uint8_t *)Msg, &txMailbox) != HAL_OK) {
        return -3; // send failed
    }

    return 0; // success
} 
