/****************************************************************
                   Source file of public.h
****************************************************************/
#ifndef _public_H
	#define _public_H
	#include "stm32f10x.h"
	#include "LED.h"
	#include "USART.H"

	/* 全局任务打断标志：为 1 时表示请求中断当前长任务 */
	extern volatile uint8_t g_task_abort;
#endif
