# Letter sheel 移植项目说明

本工程基于 STM32F10x 标准外设库，实现了仿真虚拟串口与 LetterShell 命令行的移植，并增加了一个简单的 LED 流水灯示例。通过串口终端可以交互执行命令、控制 LED、通过Keil观察寄存器状态，并支持任务打断。

## 仿真与环境
- 这里使用Keil的串口和

## 工程主要模块

### 1. 串口驱动 `APP/USART.C`

- `USART1_Init(uint32_t baudrate)`
  - 配置 GPIOA9/10 为 USART1 复用功能。
  - 配置 USART1：波特率、数据位、停止位、校验位等。
  - 使能 USART1 及 RXNE 接收中断，并配置 NVIC。
- `USART1_SendChar(char c)`
  - 阻塞方式发送 1 字节数据。
- `USART1_SendString(const char *str)`
  - 发送以 `\0` 结尾的 C 字符串。
- `fputc` 重定向
  - 将 `printf` 输出重定向到 USART1，便于调试打印。
- 环形接收缓冲
  - 使用 `rx_buffer[RX_BUFFER_SIZE]` + `rx_head/rx_tail` 实现非阻塞接收缓冲。
  - `USART1_GetChar(char *c)` 从环形缓冲中取 1 字节（无数据立即返回 0）。
- `USART1_IRQHandler`
  - 从硬件接收寄存器读取字节，写入环形缓冲。
  - 特殊处理 `Ctrl+C` (0x03)：置位全局中断标志 `g_task_abort`，用来打断长任务。

### 2. LED 流水灯 `APP/LED.C`

- `LEDInit()`
  - 打开 GPIOC 时钟，配置 GPIOC 全口线为推挽输出。
- `LEDdisplay(uint8_t times)`
  - 按顺序向 GPIOC 写入 `0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F`，实现 8 位流水灯效果。
  - 每步之间使用 `delay_ms(200)` 延时。
  - 在循环中周期性检查 `g_task_abort`，支持任务被 `Ctrl+C` 打断。

### 3. SysTick 延时 `APP/systick.c`

- 提供 `delay_ms` / `delay_us`，给 LED 流水灯等代码使用。

### 4. LetterShell 移植

#### 核心文件

- `APP/shell.c` / `APP/shell.h`
  - Shell 核心实现：命令解析、执行、帮助列表等。
  - 使用 section 机制（`shellCommand` 段）自动收集通过 `SHELL_EXPORT_CMD` 导出的命令。
- `APP/shell_cfg_user.h`
  - 用户配置：是否使用导出命令、任务模式、命令长度、历史条数、回车类型等。
- `APP/shell_ext.c/.h`
  - Shell 扩展功能，如参数解析、变量访问等。

#### 串口端口层 `APP/shell_port.c`

- `userShellWrite`
  - 将 Shell 输出通过 `USART1_SendChar` 发送到串口。
- `userShellRead`
  - 从 `USART1_GetChar` 读取数据提供给 Shell 输入。
- `userShellInit`
  - 绑定 `shell.write` / `shell.read`。
  - 调用 `shellInit(&shell, shellBuffer, sizeof(shellBuffer));` 初始化 Shell。

#### 自定义命令 `APP/cmd_demo.c`

- `demo` 命令（无参数）
  - 函数：
    - `static void demo(void)`
  - 导出：
    - `SHELL_EXPORT_CMD(..., demo, demo, demo command);`
  - 作用：在终端输入 `demo`，串口打印 `demo ok`。

- `led` 命令（带次数参数）
  - 函数：
    - `static int led(size_t x)`，内部调用 `LEDdisplay((uint8_t)x);`
  - 导出：
    - `SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, led, led, led test);`
  - 用法：
    - `led 3` → 跑 3 轮 LED 流水灯。
  - 可被 `Ctrl+C` 打断（通过 `g_task_abort`）。

- `echo` 命令（回显参数）
  - 函数：
    - `static int echo(int argc, char *argv[])`（main 形式命令）。
  - 导出：
    - `SHELL_EXPORT_CMD(..., echo, echo, echo input and output);`
  - 用法：
    - `echo hello world 123` → 输出 `hello world 123`。

### 5. 主函数 `user/main.c`

- 初始化顺序：
  - `SystemInit();`
  - `LEDInit();`
  - `USART1_Init(115200);`
  - `userShellInit();`
- 主循环：

  ```c
  while (1) {
      shellTask(&shell);   // 持续处理 Shell 输入和命令
  }
  ```

## 已做的移植和自定义内容

- 移植 LetterShell 到 STM32F10x 工程：
  - 配置 `shell_cfg_user.h`（命令导出、任务模式、回车形式等）。
  - 实现 `shell_port.c`，将 Shell 的读写接口绑定到 `USART1`。
- 实现串口环形缓冲与 printf 重定向：
  - 使用中断接收，主循环中无阻塞读取。
  - 将标准 `printf` 输出重定向到串口，方便调试信息输出。
- 增加可打断的 LED 流水灯任务：
  - 新增全局中断标志 `g_task_abort`，在 USART1 中断中接收 `Ctrl+C` 后置位。
  - `LEDdisplay` 在每一步都检查该标志，实现“长任务可中途停止”。
- 编写自定义 Shell 命令：
  - `demo`：打印简单提示，验证 Shell 工作正常。
  - `led`：通过命令行控制 LED 流水灯次数。
  - `echo`：演示 main 形式命令，回显输入参数。
- 自定义 Shell 欢迎字符画：
  - 修改 `shell.c` 中 `SHELL_TEXT_INFO`，显示自定义 ASCII 艺术 Banner + 编译时间与版本号。
---
## 效果演示
### 进入终端和获取命令
![演示1](/docs/img/1.gif)
### LED流水灯任务执行
![演示1](/docs/img/2.gif)
---
## 功能总结

- 串口命令行交互：
  - 通过串口助手（115200 8N1）连接 USART1，即可看到 Shell 提示符并输入命令。
- 运行内置命令：
  - `demo`：测试 Shell 是否正常工作。
  - `led N`：让 LED 流水灯运行 N 轮，并可随时按 `Ctrl+C` 停止。
  - `echo ...`：测试参数传递和字符串输出。
- 扩展更多命令：
  - 参考 `cmd_demo.c` 中的写法，通过 `SHELL_EXPORT_CMD` 很容易再添加新的调试命令或控制命令。

## 使用方法简要

1. 编译并烧录固件到 STM32F10x 板子上。
2. 打开串口助手：
   - 端口：连接到板子 USART1 的 USB 转串口号
   - 波特率：`115200`
   - 数据位：`8`
   - 校验位：`None`
   - 停止位：`1`
3. 复位,启动调试
4. 输入命令测试：
   - `help`查看所有已经注册函数
   - `demo` 打印测试
   - `led 2` led流水灯闪烁
   - `echo STM32 shell test` 回传显示
5. 由于为裸机代码在 LED 流水灯运行过程中会阻塞任务，发送 `Ctrl+C`可以中断当前任务。
