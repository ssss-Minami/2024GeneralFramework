#ifndef _CRC_H_
#define _CRC_H_
//注：此为裁判系统所用CRC，与USB-CDC所用算法不同
extern uint8_t Verify_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
extern uint8_t Get_CRC8_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint8_t ucCRC8);
extern void Append_CRC8_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
extern uint16_t Ref_Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
extern uint32_t Ref_Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
extern void Ref_Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);



#endif
