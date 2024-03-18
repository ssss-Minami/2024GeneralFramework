#ifndef __CANINST_H__
#define __CANINST_H__
#include "can.h"
#include "config.h"

typedef enum
{
    MOTOR,
    BOARD
}Can_MsgType;

typedef struct can_info_s
{
    CAN_TxHeaderTypeDef txheader;
    CAN_RxHeaderTypeDef rxheader;
    uint8_t CAN_id;
    CAN_HandleTypeDef *hcan_x;
    // @param:list_id :调用此can的实体在其相应注册列表中的顺次
    void (*FillTxMsg)(struct can_info_s *st, uint8_t *txdata, uint8_t list_id);
    void (*CbkHandel)(struct can_info_s *st, uint8_t *rxdata, uint8_t list_id);
}Can_InfoTypedef;

extern Can_InfoTypedef *can_list[CAN_INST_NUM];

extern void CanSendMsg(void);
extern void CanInit(void);
#endif
