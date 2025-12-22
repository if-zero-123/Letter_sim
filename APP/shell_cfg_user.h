#ifndef __SHELL_CFG_USER_H__
#define __SHELL_CFG_USER_H__

/* 不使用内部 while(1) 任务循环，由 main 自己调用 shellTask */
#define SHELL_TASK_WHILE            0

/* 命令导出功能，默认就是 1，显式写一遍也可以 */
#define SHELL_USING_CMD_EXPORT      1

/* 暂不启用伴生对象、文件系统等高级功能 */
#define SHELL_USING_COMPANION       0
#define SHELL_SUPPORT_END_LINE      0

/* 配置回车换行形式，普通串口助手一般只发 CR 或 CRLF */
#define SHELL_ENTER_CR              1
/* 也可以改成 LF 或 CRLF，按你的串口助手来选 */
// #define SHELL_ENTER_LF           1
// #define SHELL_ENTER_CRLF         1

/* 一些小配置参数，可以按需要调整 */
#define SHELL_COMMAND_MAX_LENGTH    64
#define SHELL_PARAMETER_MAX_NUMBER  8
#define SHELL_HISTORY_MAX_NUMBER    8

/* 如果你有 1ms Tick 定时器，后面可以再打开，用于双击 tab 等功能 */
// extern uint32_t GetSysTickMs(void);
// #define SHELL_GET_TICK()         GetSysTickMs()

#endif
