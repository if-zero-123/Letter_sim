#include "public.h"
#include "shell_port.h"   // 提供 Shell 对象和 userShellInit 声明
#include <string.h>

/* 全局任务打断标志，USART 中断和长任务共用 */
volatile uint8_t g_task_abort = 0;

int main(void)
{
    USART1_Init(115200);
    LEDInit();

    /* 初始化 Shell，绑定串口读写并准备命令表 */
    userShellInit();

    while (1) {
        shellTask(&shell); // 持续处理 Shell 输入和命令
        
    }
}
