/**
 * @file    scurve.c
 * @brief   多实例位置型 S 形(七段式)速度规划器实现
 *
 * 移植自 SMotor 工程(Motor.c),核心七段式状态机与 jerk/accel 限幅逻辑保留,
 * 改造点:
 *   - 多实例(StepperCtrl 全局变量 → Scurve_Planner *p)
 *   - 纯算法,去掉硬编码 htim3/GPIO(PWM 输出由上层电机驱动负责)
 *   - 支持双向(direction = ±1),速度带符号
 *   - 修正减速启动逻辑(原版 case CONST_SPEED 里直接赋 -MAX_JERK*dt 不连续)
 */
#include "scurve.h"
#include <math.h>

/* ---- 对外接口 ----------------------------------------------------------- */

void Scurve_SetConfig(Scurve_Planner *p, const Scurve_Config *cfg)
{
    if (p == NULL || cfg == NULL) return;
    /* 防止除零 / 负值 */
    p->cfg.max_speed = cfg->max_speed > 0.0f ? cfg->max_speed : 1.0f;
    p->cfg.max_accel = cfg->max_accel > 0.0f ? cfg->max_accel : 1.0f;
    p->cfg.max_jerk  = cfg->max_jerk  > 0.0f ? cfg->max_jerk  : 1.0f;
}

void Scurve_Init(Scurve_Planner *p, const Scurve_Config *cfg)
{
    if (p == NULL) return;
    p->state         = SCURVE_STOP;
    p->current_speed = 0.0f;
    p->current_accel = 0.0f;
    p->target_pos    = 0.0f;
    p->current_pos   = 0.0f;
    p->direction     = 1.0f;
    Scurve_SetConfig(p, cfg);
}

void Scurve_MoveTo(Scurve_Planner *p, float target_pos)
{
    if (p == NULL) return;
    /* 目标位移过小则直接判定完成,避免数值抖动 */
    float target_abs = target_pos < 0.0f ? -target_pos : target_pos;
    if (target_abs < 1e-3f) {
        Scurve_Stop(p);
        return;
    }
    p->target_pos    = target_pos;
    p->current_pos   = 0.0f;
    p->current_speed = 0.0f;
    p->current_accel = 0.0f;
    p->direction     = target_pos >= 0.0f ? 1.0f : -1.0f;
    p->state         = SCURVE_ACCEL_JERK_UP;
}

void Scurve_Stop(Scurve_Planner *p)
{
    if (p == NULL) return;
    p->state         = SCURVE_STOP;
    p->current_speed = 0.0f;
    p->current_accel = 0.0f;
    /* 不清零 current_pos,便于上层查看已完成量 */
}

bool Scurve_IsIdle(const Scurve_Planner *p)
{
    return (p != NULL) && (p->state == SCURVE_STOP);
}

float Scurve_GetPos(const Scurve_Planner *p)
{
    return (p != NULL) ? p->current_pos : 0.0f;
}

float Scurve_GetSpeed(const Scurve_Planner *p)
{
    return (p != NULL) ? p->current_speed : 0.0f;
}

float Scurve_Update(Scurve_Planner *p, float dt)
{
    if (p == NULL || p->state == SCURVE_STOP || dt <= 0.0f) {
        return 0.0f;
    }

    /* remaining 用绝对值比较(位置/速度按 direction 折算成正方向处理) */
    float target_abs   = p->target_pos  < 0.0f ? -p->target_pos  : p->target_pos;
    float current_abs  = p->current_pos < 0.0f ? -p->current_pos : p->current_pos;
    float remaining    = target_abs - current_abs;
    if (remaining < 0.0f) remaining = 0.0f;

    float speed_abs    = p->current_speed < 0.0f ? -p->current_speed : p->current_speed;
    float required_decel_steps = (speed_abs * speed_abs) / (2.0f * p->cfg.max_accel);

    /* ---- 加速阶段 ---- */
    if (speed_abs < p->cfg.max_speed && remaining > required_decel_steps) {
        switch (p->state) {
        case SCURVE_ACCEL_JERK_UP:
            p->current_accel += p->cfg.max_jerk * dt;
            if (p->current_accel >= p->cfg.max_accel) {
                p->current_accel = p->cfg.max_accel;
                p->state = SCURVE_ACCEL_CONST;
            }
            break;
        case SCURVE_ACCEL_CONST:
            /* 预测下一拍是否到 max_speed, 到了就收 jerk */
            if (speed_abs + p->current_accel * dt >= p->cfg.max_speed) {
                p->state = SCURVE_ACCEL_JERK_DOWN;
            }
            break;
        case SCURVE_ACCEL_JERK_DOWN:
            p->current_accel -= p->cfg.max_jerk * dt;
            if (p->current_accel <= 0.0f) {
                p->current_accel = 0.0f;
                /* 速度钳到 max_speed,消除舍入误差 */
                speed_abs = p->cfg.max_speed;
                p->current_speed = speed_abs * p->direction;
                p->state = SCURVE_CONST_SPEED;
            }
            break;
        case SCURVE_CONST_SPEED:
            /* 匀速,等待到达减速点 */
            break;
        /* 减速阶段中,若 remaining 又变大(理论上不会),直接保持减速 */
        default:
            break;
        }
    }
    /* ---- 减速阶段 ---- */
    else if (remaining <= required_decel_steps) {
        switch (p->state) {
        case SCURVE_CONST_SPEED:
        case SCURVE_ACCEL_JERK_DOWN:
            /* 进入减速启动: 从当前(可能非零的)加速度开始线性减小到 -MAX_ACCEL */
            p->state = SCURVE_DECEL_JERK_UP;
            /* 不在此处突变 accel, 让 DECEL_JERK_UP 平滑过渡(修正 SMotor 的不连续) */
            if (p->state == SCURVE_DECEL_JERK_UP) {
                /* fallthrough 到下面的处理 */
            }
            break;
        default:
            break;
        }

        switch (p->state) {
        case SCURVE_DECEL_JERK_UP:
            p->current_accel -= p->cfg.max_jerk * dt;
            if (p->current_accel <= -p->cfg.max_accel) {
                p->current_accel = -p->cfg.max_accel;
                p->state = SCURVE_DECEL_CONST;
            }
            break;
        case SCURVE_DECEL_CONST:
            /* 预测下一拍是否到 0, 到了就收 jerk */
            if (speed_abs + p->current_accel * dt <= 0.0f) {
                p->state = SCURVE_DECEL_JERK_DOWN;
            }
            break;
        case SCURVE_DECEL_JERK_DOWN:
            p->current_accel += p->cfg.max_jerk * dt;
            if (p->current_accel >= 0.0f) {
                p->current_accel = 0.0f;
                p->current_speed = 0.0f;
                /* 把位移钳到目标,消除舍入误差 */
                p->current_pos = p->target_pos;
                p->state = SCURVE_STOP;
                return 0.0f;
            }
            break;
        default:
            break;
        }
    }

    /* ---- 积分更新 ---- */
    p->current_speed += p->current_accel * dt * p->direction;
    /* 速度反向钳位(不允许冲过 0) */
    if (p->direction > 0.0f && p->current_speed < 0.0f) p->current_speed = 0.0f;
    if (p->direction < 0.0f && p->current_speed > 0.0f) p->current_speed = 0.0f;
    /* 速度上限钳位 */
    speed_abs = p->current_speed < 0.0f ? -p->current_speed : p->current_speed;
    if (speed_abs > p->cfg.max_speed) {
        p->current_speed = p->cfg.max_speed * p->direction;
    }

    p->current_pos += p->current_speed * dt;

    /* 到达/越过目标 → 精确停车 */
    float done_abs = p->current_pos < 0.0f ? -p->current_pos : p->current_pos;
    if (done_abs >= target_abs) {
        p->current_pos   = p->target_pos;
        p->current_speed = 0.0f;
        p->current_accel = 0.0f;
        p->state         = SCURVE_STOP;
    }

    return p->current_speed;
}