#ifndef __TOPIC_H__
#define __TOPIC_H__
#include "main.h"
#include "CanInst.h"
#include "config.h"
typedef struct
{
    uint8_t *data_p;
    uint8_t *buff_p;
    Can_InfoTypedef can;
    uint8_t len;    
}Topic_TypeDef;


extern void TopicPush(Topic_TypeDef *topic, uint8_t *data);
extern void TopicPull(Topic_TypeDef *topic, uint8_t *addr);
extern Topic_TypeDef *topic_list[TOPIC_NUM];
#endif