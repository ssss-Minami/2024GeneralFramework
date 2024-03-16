#include "topic.h"
#include "string.h"
void TopicPush(Topic_TypeDef *topic, uint8_t *data)
{
    memcpy(topic->data_p, data, topic->len);
    return;
}

void TopicPull(Topic_TypeDef *topic, uint8_t *addr)
{
    memcpy(addr, topic->data_p, topic->len);
    return;
}

void TopicFillMsg(Can_InfoTypedef *st, uint8_t *txdata, uint8_t list_id)
{
    if(topic_list[list_id]->len <=8)
        memcpy(txdata, topic_list[list_id]->data_p, topic_list[list_id]->len);
    else
    {
        uint8_t static bias;
        uint8_t tmp_len = (topic_list[list_id]->len -bias)>7 ? 7 : (topic_list[list_id]->len -bias);
        memcpy(txdata, &bias, 1);
        memcpy(txdata+1, topic_list[list_id]->data_p +bias, tmp_len);
        bias +=tmp_len;
        bias = bias==topic_list[list_id]->len ? bias : 0;
    }
    return;
}

void TopicDoNothing(Can_InfoTypedef *st, uint8_t *txdata, uint8_t list_id)
{
    return;
}

//void TopicDoNothing(Can_InfoTypedef *st, uint8_t *rxdata, uint8_t list_id)
//{
//    return;
//}

void TopicCbk(Can_InfoTypedef *st, uint8_t *rxdata, uint8_t list_id)
{
    if(topic_list[list_id]->len <=8)
        memcpy(topic_list[list_id]->data_p, rxdata, topic_list[list_id]->len);
    else
    {
        uint8_t static bias;
        if(rxdata[0] != bias && rxdata[0]!=bias-7)  
                goto error;
        if(topic_list[list_id]->len - bias >7)
        {
            memcpy(topic_list[list_id]->buff_p +bias, rxdata +1, 7);
            bias +=7;
        }
        else
        {
            memcpy(topic_list[list_id]->buff_p +bias, rxdata +1, topic_list[list_id]->len - bias);
            bias = 0;
            memcpy(topic_list[list_id]->data_p, topic_list[list_id]->buff_p, topic_list[list_id]->len);
        }
    }
    error:
    Message[0] = 255;
    return;
}

void TopicInit()
{
    Topic_TypeDef topic_chassic;
    topic_list[0] = &topic_chassic;
    
}
