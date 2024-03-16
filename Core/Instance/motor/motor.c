#include "motor.h"
#include "pid.h"
#include "IMU.h"
#include "string.h"
#include "config.h"
#include "CanInst.h"
#include "can.h"
#include "ErrorHandel.h"
#include "malloc.h"

extern IMU_TypeDef *imu_list[IMU_NUM];
Motor_TypeDef *motor_list[MOTOR_NUM];

void MotorRegist(Motor_InitTypedef st);
/*
 * @brief  	计算PID并限幅
 * @param	电机结构体指针数组
 * @retval 	无，结果在结构体中
 * @note：运行频率为1000Hz
 */
void MotorCalc(void)
{
    for(int i=0;i<MOTOR_NUM;i++)
    {
        if(motor_list[i]->info.enable == 0)
        {
            motor_list[i]->output = 0;
            continue;
        }
        PID_Type_e type = motor_list[i]->info.pid_type;
        float val_now = MotorGetVal(motor_list[i]);
        float *tar_now;
        tar_now = &motor_list[i]->target;
        
        if(type == SPEED)
        {
            motor_list[i]->output = PID_Incr(&(motor_list[i]->pid[SPEED]), val_now, *tar_now);
        }
        else if(type == ANGLE)
        {
            //对Dji电机角度多圈转单圈
            if(motor_list[i]->info.motor_type != OTHER)
            {
                if(*tar_now - val_now > 4096) *tar_now -= 8192;
                if(val_now - *tar_now > 4096) *tar_now += 8192;
            }
            motor_list[i]->output = PID_Origin(&(motor_list[i]->pid[ANGLE]), val_now, *tar_now);
        }
        else if(type == DUAL_LOOP)
        {
            //对Dji电机角度多圈转单圈
            if(motor_list[i]->info.motor_type != OTHER)
            {
                if(*tar_now - val_now > 4096) *tar_now -= 8192;
                if(val_now - *tar_now > 4096) *tar_now += 8192;
            }
            motor_list[i]->output = PID_Origin(&(motor_list[i]->pid[ANGLE]), val_now, *tar_now);
            motor_list[i]->output = PID_Incr(&(motor_list[i]->pid[SPEED]), motor_list[i]->data.speed, motor_list[i]->output);
        }
        else if(type == IMU)
        {
            //多圈转单圈 && 编码转弧度

            if(motor_list[i]->info.motor_type != OTHER)
            {
                if(*tar_now - val_now > PI) *tar_now -= 2*PI;
                if(val_now - *tar_now > PI) *tar_now += 2*PI;
            }
            motor_list[i]->output = PID_Origin(&(motor_list[i]->pid[ANGLE]), val_now, *tar_now);
            motor_list[i]->output = PID_Incr(&(motor_list[i]->pid[SPEED]), motor_list[i]->data.speed, (motor_list[i]->output + motor_list[i]->feed_fwd));
            
        }
        else
        	sErrorHandel(OUT_OF_ENUM);

    //    MotorOptLmt(motor_list[i], motor_list[i]->info.output_max);

    }
    return;
}

/*
 * @brief  	更新PID类型参考值来源
 * @param	电机结构体地址
 * @param   PID类型枚举值
 * @param	参考值来源地址，float
 * @retval 	无
 * @note    '*source' can transmit as NULL if 'newtype' != 'IMU'
 */
//TODO:增加看门狗 && 整合、废弃
void MotorStatusUpdate(Motor_TypeDef *motor, PID_Type_e newtype, float *source)
{
    if(newtype==motor->info.pid_type || source==NULL)
        return;
    
    PID_Clear(&motor->pid[INNER]);
    PID_Clear(&motor->pid[OUTER]);
    motor->info.pid_type = newtype;

    if(motor->info.pid_type == ANGLE)
        motor->value = &(motor->target);  //avoid null ptr access
    else if(motor->info.pid_type == SPEED)
        motor->value = &(motor->target);
    else if(motor->info.pid_type == DUAL_LOOP)
        motor->value = &(motor->target);
    else if(motor->info.pid_type == IMU)
        motor->value = source;
    else
        sErrorHandel(OUT_OF_ENUM);
    return;
}

/*
 * @brief  	获取电机当前值
 * @param	电机结构体地址
 * @retval 	电机当前值
 */
float MotorGetVal(Motor_TypeDef *motor)
{
    float val;
    if(motor->info.pid_type == ANGLE)
        val = motor->data.angle;
    else if(motor->info.pid_type == SPEED)
        val = motor->data.speed;
    else if(motor->info.pid_type == DUAL_LOOP)
        val = motor->data.angle;
    else if(motor->info.pid_type == IMU)
        val = *motor->value;
    else
        sErrorHandel(OUT_OF_ENUM);
    
    return val;
}

/*
 * @brief  	电机输出限幅
 * @param	电机结构体地址
 * @param   限幅值
 * @retval 	无
 */
void MotorOptLmt(Motor_TypeDef *motor, float lmt)
{
    float limit=0;
    limit = motor->info.output_max>lmt ? lmt : motor->info.output_max;
    if(motor->output >= limit)
        motor->output = limit;
    else if(motor->output <= -limit)
        motor->output = -limit;
    return;
}

void MotorCanCbk(Can_InfoTypedef *st, uint8_t *rxdata, uint8_t list_id)
{
	motor_list[list_id]->data.angle = (uint16_t)(rxdata[0]<<8) + rxdata[1];
	motor_list[list_id]->data.speed = (int16_t)(rxdata[2]<<8) + rxdata[3];
	motor_list[list_id]->data.current = (int16_t)(rxdata[4]<<8) + rxdata[5];
	motor_list[list_id]->data.temp = rxdata[6];
    return;
}

void MotorFillMsg(Can_InfoTypedef *st, uint8_t *txdata, uint8_t list_id)
{
    uint8_t tmp_id = st->CAN_id>4 ? (st->CAN_id)-4 : st->CAN_id;
    txdata[2 * (tmp_id-1)] = (int16_t)motor_list[list_id]->output >>8;
    txdata[(2*tmp_id) - 1] = (int16_t)motor_list[list_id]->output;
    return;
}

/*
 * @brief  	设置电机目标值
 * @param	电机结构体指针
 * @param	目标值
 * @param   ABS->absolute target; 
 *          INCR->add from perious target
 *          FEED->change feedforward
 * @retval 	无
 */
void MotorSetTar(Motor_TypeDef *motor, float val, uint8_t style)
{
    if(style == ABS)
        motor->target = val;
    else if(style == INCR)
        motor->target += val;
    else if(style == FEED)
        motor->feed_fwd = val;
    else
        sErrorHandel(OUT_OF_ENUM);
}

/*
 * @brief  	停止指定或全部的电机输出
 * @param	电机结构体指针
 * @retval 	无
 * @note    传入空指针时，视为停止所有电机
 */
void MotorStop(Motor_TypeDef *motor)
{
    if(motor==NULL)
    {
        for(int i=0;i<MOTOR_NUM;i++)
        {
            motor_list[i]->info.enable = 0;
            motor_list[i]->output = 0;
            PID_Clear(&motor_list[i]->pid[INNER]);
            PID_Clear(&motor_list[i]->pid[OUTER]);
        }
    }
    else
    {
        motor->info.enable = 0;
        motor->output = 0;
        PID_Clear(&motor->pid[INNER]);
        PID_Clear(&motor->pid[OUTER]);
    }
}

void MotorRestart(Motor_TypeDef *motor)
{
    Motor_TypeDef *m;
    for(int i=0;i<MOTOR_NUM;i++)
    {
        if(motor!=NULL)
            m = motor_list[i];
        else
            m = motor;
        PID_Clear(&m->pid[INNER]);
        PID_Clear(&m->pid[OUTER]);
        if(m->info.pid_type==DUAL_LOOP)
        {
            MotorGetZeroPoint(m);
            MotorSetTar(m, m->info.zero_point, ABS);
        }
        else if(m->info.pid_type==IMU)
            MotorSetTar(m, MotorGetVal(m), ABS);
        m->info.enable = 1;
        if(motor==NULL)
            return;
    }
    
}


/*
 * @brief  	获取电机上电零点
 * @param	电机结构体指针
 * @retval 	ret=0: 获取失败    ret=1: 获取成功 
 */
uint8_t MotorGetZeroPoint(Motor_TypeDef *motor)
{
    if(motor->data.angle != 0)
    {
        motor->info.zero_point = motor->data.angle;
        return 1;
    }
    else
        return 0;
}


void MotorInit()
{
    /* Yaw init */
    PID_TypeDef pid_outer = {
        .Kp = 15,
        .Ki = 0.65,
        .Kd = 0,
        .Output_Max = 5000,
        .Err_sum_Max = 5000
    };
    PID_TypeDef pid_inner = {
        .Kp = 0.125,
        .Ki = 1.8,
        .Kd = 0.05,
        .Output_Max = 8000,
        .Err_sum_Max = 8000
    };
    Motor_InitTypedef init = {
    .can_id = 1,
    .hcan = &hcan1,
    .list_id = MOTOR_YAW,
    .motor_type = DJI,
    .pid_type = DUAL_LOOP,
    .source = &imu_list[IMU_YAW]->angle[0],
    .txheader_id = 0x1FF,
	.rxheader_id = 0x204,
    .pid_outer = pid_outer,
    .pid_inner = pid_inner,
    .opt_max = 8000
   };
    MotorRegist(init);   

    /* Pitch init */
    init.can_id = 2;
    init.list_id = MOTOR_PITCH;
    init.pid_type = DUAL_LOOP;
    init.source = NULL;
    MotorRegist(init);

    /* Chassis init */
    init.can_id = 1;
    init.list_id = MOTOR_CHS_1;
    init.pid_type = SPEED;
    init.opt_max = 5000;
    init.txheader_id = 0x200;
    init.rxheader_id = 0x200 + init.can_id;
    MotorRegist(init);
    init.can_id = 2;
    init.list_id = MOTOR_CHS_2;
    init.rxheader_id = 0x200 + init.can_id;
    MotorRegist(init);
    init.can_id = 3;
    init.list_id = MOTOR_CHS_3;
    init.rxheader_id = 0x200 + init.can_id;
    MotorRegist(init);
    init.can_id = 4;
    init.list_id = MOTOR_CHS_4;
    init.rxheader_id = 0x200 + init.can_id;
    MotorRegist(init);
}

//TODO: 添加对其他电机的支持
void MotorRegist(Motor_InitTypedef st)
{
    Motor_TypeDef static motor;
    CAN_TxHeaderTypeDef txheader = {
        .ExtId =   0x0, 
        .IDE = CAN_ID_STD,
		.RTR = CAN_RTR_DATA,
		.DLC = 8,
        .StdId = st.txheader_id
        };
    motor.can.txheader = txheader;
    motor.can.rxheader.StdId = st.rxheader_id + st.can_id;
    motor.can.rxheader.DLC = 8;
    motor.can.CAN_id = st.can_id;
    motor.can.FillTxMsg = MotorFillMsg;
    motor.can.CbkHandel = MotorCanCbk;
    motor.can.hcan_x = st.hcan;

    Motor_InfoTypeDef info = {
        .enable = 1,
        .motor_type = st.motor_type,
        .output_max = st.opt_max,
        .pid_type = st.pid_type,
        .zero_point = 0
    };
    motor.info = info;
    
    motor.pid[OUTER] = st.pid_outer;
    motor.pid[INNER] = st.pid_inner;
    motor.feed_fwd = 0;
    if(st.pid_type == SPEED)
        motor.value = &motor_list[st.list_id]->target;   //avoid null ptr access
    else if(st.pid_type==ANGLE || st.pid_type==DUAL_LOOP)
        motor.value = &motor_list[st.list_id]->target;
    else if(st.pid_type == IMU)
    {
        if(st.source == NULL)
            sErrorHandel(NULL_POINTER);
        else
            motor.value = st.source;
    }
    else
        sErrorHandel(OUT_OF_ENUM);
    motor_list[st.list_id] = malloc(sizeof(Motor_TypeDef));
    memcpy(motor_list[st.list_id], &motor, sizeof(Motor_TypeDef));
    return;
}
