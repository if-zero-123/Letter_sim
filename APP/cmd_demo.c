
#include "shell.h"      // Shell 核心头文件，提供 SHELL_EXPORT_CMD 等宏
#include "led.h"        // LED 控制接口，提供 LEDdisplay()
#include "public.h"     // 工程公共头，可根据需要使用全局变量
#include <stdio.h>       // printf 等标准输出函数
#include <stdint.h>      // 标准整数类型定义

/*
 * demo 命令：无参数
 * 在命令行输入：demo
 * Shell 会调用本函数，串口打印 "demo ok"。
 */
static void demo(void)
{
    printf("demo ok\r\n");
}

SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)| SHELL_CMD_DISABLE_RETURN,
    demo, demo, demo command);


static int led(size_t x)
	{	
    LEDdisplay((uint8_t)x);
    return 0;
}

/*
 * 注册 led 命令
 *  - 命令名：led
 *  - 函数：led(size_t x)
 *  - 描述："led test"（在 help 列表中显示）
 */
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)| SHELL_CMD_DISABLE_RETURN,
    led, led, led test);


static int echo(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\r\n");
    return 0;
}


SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
    echo, echo, echo input and output);
