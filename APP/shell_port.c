#include "shell_port.h"
#include "usart.h"      // 这里用到 USART1 的初始化和收发函数：USART1_Init / USART1_SendChar / USART1_GetChar

Shell shell;
static char shellBuffer[512];

/* shell 写接口：通过 USART1 发送数据 */
short userShellWrite(char *data, unsigned short len)
{
    unsigned short i;
    for (i = 0; i < len; i++)
    {
        USART1_SendChar(data[i]);   // 一个字节一个字节地发出去
    }
    return (short)len;
}

/* shell 读接口：从串口接收数据，读到用户缓冲区 */
short userShellRead(char *data, unsigned short len)
{
    unsigned short count = 0;

    while (count < len)
    {
        if (USART1_GetChar(&data[count]))   // 从环形缓冲读 1 个字节，返回 1 表示成功
        {
            count++;
        }
        else
        {
            break;  // 当前没有新数据就跳出循环
        }
    }

    return (short)count;   // 返回本次实际读取的字节数
}

/* 初始化 shell：配置读写接口并绑定缓冲区 */
void userShellInit(void)
{
    shell.write = userShellWrite;
    shell.read  = userShellRead;

    /* 如果在配置中打开了 SHELL_USING_LOCK，这里还需要设置 shell.lock / shell.unlock */

    shellInit(&shell, shellBuffer, sizeof(shellBuffer));
}
