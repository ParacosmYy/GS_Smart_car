/*
 * data.c
 *
 *  Created on: 2025年10月31日
 *      Author: Paracosm
 *
 *  Legacy global data definition file.
 *  新架构中运行态数据应优先收敛到明确 owner 的 context/handler。
 *  本文件仅保留尚未完成迁移的兼容变量。
 */


#include "data.h"
//-----------define------------------------

//-----------pit中断变量---------------------
volatile int pit_ch0_count  = 0 ;          // CCU60 通道 0 中断计数（兼容保留）

//------------测试变量-----------------------
