/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "chassis.h"
#include "stepper.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t chassisTaskHandle;
const osThreadAttr_t chassisTask_attributes = {
  .name = "chassisTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartChassisTask(void *argument);
static void chassis_uart_log(const char *fmt, ...);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  chassis_init();
  Stepper_Init();          /* 配置 4 个步进定时器(Prescaler/ARR/占空比), 不立即转 */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  chassisTaskHandle = osThreadNew(StartChassisTask, NULL, &chassisTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /*
   * 指令层(状态机, 非阻塞): 世界坐标系下走一条路径.
   * 设计要点:
   *   - move_to_coordinate / headturn 均为"发起-查询"语义, 反复调用同一目标
   *     仅在目标变化时重新规划, 否则只查询 arrived 状态.
   *   - 本任务为低优先级, 可做阻塞串口打印; 控制环在 chassisTask(1ms) 中独立运行.
   *   - 10ms 节拍查询状态, 100ms 节拍打印一次 pose/轮速.
   */
  (void)argument;

  enum { DS_MOVE1, DS_TURN, DS_MOVE2, DS_DONE };
  uint8_t demo_state = DS_MOVE1;
  uint8_t log_div = 0;
  uint8_t settle = 0;  /* 到达后短暂停留计数 */

  chassis_uart_log("\r\n[chassis demo] start (world frame)\r\n");

  for (;;)
  {
    bool done = false;
    switch (demo_state)
    {
    case DS_MOVE1:
      /* 平移到世界坐标 (1000, 0) mm */
      done = move_to_coordinate(1000.0f, 0.0f);
      if (done) {
        chassis_uart_log("[1] reached (1000,0)\r\n");
        settle = 30;          /* 约 300ms 停顿, 便于观察 */
        demo_state = DS_TURN;
      }
      break;
    case DS_TURN:
      if (settle > 0) { settle--; break; }
      /* 转向到绝对角度 90 度 (有陀螺仪则用陀螺仪, 否则里程计积分) */
      done = headturn(90);
      if (done) {
        chassis_uart_log("[2] reached head=90\r\n");
        settle = 30;
        demo_state = DS_MOVE2;
      }
      break;
    case DS_MOVE2:
      if (settle > 0) { settle--; break; }
      /* 此时车头已转 90°, 仍以世界坐标系走到 (1000, 500) */
      done = move_to_coordinate(1000.0f, 500.0f);
      if (done) {
        chassis_uart_log("[3] reached (1000,500), demo done\r\n");
        demo_state = DS_DONE;
      }
      break;
    case DS_DONE:
    default:
      break;
    }

    /* 每 100ms 打印位姿与 4 轮目标线速度 (上位机观察) */
    if (++log_div >= 10)
    {
      log_div = 0;
      float x, y, th;
      float w[4];
      chassis_get_pose(&x, &y, &th);
      chassis_get_wheel_speed(w);
      chassis_uart_log("pose x=%.1f y=%.1f th=%.1f | w=[%.0f %.0f %.0f %.0f]\r\n",
                       x, y, th, w[0], w[1], w[2], w[3]);
    }

    osDelay(10);   /* 10ms 节拍 */
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* 1ms 控制层: S 曲线 + 麦轮逆解 + 里程计.
 * 注意: 本任务严格 1ms 节拍, 绝不做阻塞操作(如串口打印), 否则破坏控制环精度.
 *       打印/指令编排交给低优先级的 defaultTask. */
void StartChassisTask(void *argument)
{
  (void)argument;
  TickType_t last = xTaskGetTickCount();

  for (;;)
  {
    chassis_tick();                       /* 推进底盘状态机(内部 dt=1ms) */

    /* 把 chassis 算出的 4 轮目标线速度(mm/s) 下发给步进电机层 */
    float w[4];
    chassis_get_wheel_speed(w);
    Stepper_SetWheelSpeedAll(w);

    vTaskDelayUntil(&last, pdMS_TO_TICKS(1));  /* 严格 1ms */
  }
}

/* 轻量串口日志(USART1, 阻塞发送). 注: 若 Keil 使用 MicroLib 且报 vsnprintf 链接错误,
 * 请在 Options->Target 取消勾选 Use MicroLib(改用默认 libc), 即可支持 %f。 */
static void chassis_uart_log(const char *fmt, ...)
{
  static char buf[160];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  if (n > 0)
  {
    if (n > (int)sizeof(buf)) n = (int)sizeof(buf);
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, (uint16_t)n, 50);
  }
}

/* USER CODE END Application */

