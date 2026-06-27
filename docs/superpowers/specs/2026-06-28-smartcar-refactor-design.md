# GS_Smart_car 保守工程化重构设计

日期：2026-06-28

## 背景

`main` 分支当前只有 README，完整 TC264 智能车工程在 `origin/master`。工程基于 AURIX TC264、TASKING ADS/Eclipse 和逐飞库，业务代码主要在 `code/` 与 `user/`，底层 SDK、驱动和外设库主要在 `libraries/`。

本次优化选择保守工程化路线：保持 ADS/Eclipse 工程可打开、可编译，不重写逐飞库和 BSP，只整理仓库资产、修复明确 bug、拆开业务层职责，并为后续五层架构迁移留出边界。

## 设计目标

1. 保留现有硬件平台、逐飞库、TASKING ADS 工程配置和主要入口。
2. 清理仓库中的构建产物，避免 `Debug/`、`.o`、`.elf`、`.hex` 等临时文件继续污染版本历史。
3. 修复当前已确认的低风险 bug，例如 `my_abs()` 对负数返回错误。
4. 将自有业务代码逐步整理为 `app` 边界，不把逐飞库/BSP 重新实现一遍。
5. 降低主循环、ISR、图像处理、控制输出之间的耦合，让后续迭代能按模块推进。
6. 逐步向 `App -> Service -> Platform -> Impl -> Vendor` 过渡，但第一阶段只建立边界，不做大规模搬迁。

## 非目标

1. 不重写 `libraries/` 下的逐飞库、Infineon iLLD 或 BSP。
2. 不替换 ADS/Eclipse 工程为 CMake、Makefile 或其他构建系统。
3. 不一次性完成完整五层架构迁移。
4. 不在第一阶段调整 PID 参数、赛道算法策略或硬件引脚分配，除非发现明确 bug。
5. 不修改第三方库源码，除非后续单独记录必要偏差。

## 当前问题

1. 分支资产不一致：`main` 只有 README，`origin/master` 才有完整工程。
2. 构建产物进入版本控制：`Debug/` 中包含 `.o`、`.d`、`.src`、`.elf`、`.hex`、`.map` 等文件。
3. 业务边界不清：`code/image.c` 同时处理图像二值化、压缩、滤波、边界检测、控制输出和电机/舵机动作。
4. 主循环职责过重：`user/cpu0_main.c` 直接混合摄像头完成标志、图像计算、TFT 调试显示和全局状态读取。
5. ISR 职责偏重：`user/isr.c` 中断里包含编码器求平均、陀螺仪读取、浮点计算和角度积分，应逐步收敛为采样与事件标记。
6. 全局变量暴露较多：图像、PID、编码器、状态机变量通过头文件和全局变量交叉访问。
7. 存在明确逻辑 bug：`code/service.c` 的 `my_abs()` 在负数分支返回原值。
8. 头文件依赖可收敛：例如 `code/image.h` 在 include guard 前包含头文件，容易扩大依赖面。

## 目标架构

第一阶段采用轻量分层，不破坏 ADS 工程：

```text
user/
  cpu0_main.c          # 保留入口，只调度 app 层接口
  isr.c                # 保留中断入口，逐步只做采样/置位/清标志

code/
  app/                 # 后续新增：智能车主流程、调试显示、模式切换
  vision/              # 后续新增：图像二值化、滤波、边线/中线计算
  control/             # 后续新增：PID、舵机/电机控制策略
  device/              # 后续新增：对逐飞设备接口的薄封装
  common/              # 后续新增：通用工具、错误码、配置常量
  legacy/              # 过渡期可选：暂存未拆完的旧模块

libraries/             # Vendor/SDK，保持只读式依赖
```

这里的 `device/` 不是重写 BSP，而是给自有代码提供更窄的接口，例如 `SmartcarMotor_SetSpeed()` 内部仍调用逐飞 `pwm_set_duty()`。后续如果进入完整五层架构，`device/` 可以演进成 `Platform/Impl` 边界。

## 分阶段方案

### 阶段 1：仓库卫生和低风险修复

1. 新增 `.gitignore`，忽略 ADS/TASKING 构建产物和临时文件。
2. 从当前工作树移除已跟踪的 `Debug/` 构建产物。
3. 保留 `.project`、`.cproject`、`.settings/`，确保 ADS/Eclipse 工程仍可打开。
4. 修复 `my_abs()` 负数返回错误。
5. 整理 README，说明 `main`/`master` 状态、推荐打开方式、目录边界和不提交构建产物的规则。

### 阶段 2：业务入口收敛

1. 新增 `code/app/smartcar_app.c/.h`，把 `core0_main()` 中的业务循环封装为 `SmartcarApp_Init()` 和 `SmartcarApp_RunOnce()`。
2. 主循环只负责初始化、等待多核就绪和循环调用 app。
3. 调试显示单独封装，减少 `cpu0_main.c` 中散落的 TFT 调用。
4. 现有全局变量暂时保留，但从 app 层集中读取，避免继续扩散。

### 阶段 3：图像与控制解耦

1. 将 `task_calculte()` 拆成三步：视觉处理、控制决策、执行输出。
2. 图像模块只产出结构化结果，例如中线、偏差、丢线状态。
3. 控制模块读取图像结果和速度快照，计算电机/舵机输出。
4. 执行模块负责调用逐飞 PWM/电机/舵机接口。
5. 逐步把全局数组封装为上下文或只读快照，先降低写入点数量。

### 阶段 4：实时路径收敛

1. ISR 中优先保留清中断、采样、计数、置位。
2. 复杂滤波、浮点计算、控制决策逐步移到主循环或周期任务中。
3. ISR 与主循环共享变量统一标记 `volatile`，并明确数据所有权。
4. 对编码器、陀螺仪、摄像头完成标志建立快照接口。

### 阶段 5：向五层架构过渡

1. `libraries/` 固定作为 Vendor。
2. `code/device/` 中的薄封装逐步演进为 Platform/Impl 接口。
3. `code/control/` 和 `code/vision/` 保持平台无关，减少直接包含 `zf_common_headfile.h`。
4. 当业务稳定后，再考虑将目录正式迁移为 `app/ service/ platform/ impl/ vendor/`。

## 数据流

```text
Camera/Encoder/IMU ISR or driver
    -> sample flags and raw buffers
    -> SmartcarApp_RunOnce()
    -> Vision_Process()
    -> Control_Update()
    -> Actuator_Apply()
    -> DebugView_Render()
```

第一阶段不强制改变所有数据流，只先建立 app 调度点和模块边界。

## 错误处理

1. 自有模块新增返回值时使用明确错误码，不使用裸 `int` 表示多种含义。
2. 入口参数必须判空。
3. 不在 ISR 中做复杂错误处理，只记录状态或置位。
4. 对明确硬件失败保持现有逐飞库行为，避免第一阶段引入未知副作用。

## 测试和验证

1. 每个阶段后运行 `git status`，确认没有构建产物重新进入跟踪。
2. 使用 ADS/Eclipse 打开工程并编译，作为嵌入式工程主验证方式。
3. 对纯算法函数优先做主机端可编译检查或小型测试，例如绝对值、PID 限幅、图像中线计算。
4. 对涉及硬件输出的改动，优先保持函数签名和调用顺序，减少一次改动的变量。
5. 每次拆分模块后检查头文件依赖，不让业务头文件无理由包含全量 `zf_common_headfile.h`。

## 验收标准

1. ADS/Eclipse 工程仍保留并可打开。
2. `Debug/` 和二进制构建产物不再被 Git 跟踪。
3. README 能说明项目结构、构建入口和提交规则。
4. `my_abs()` 明确修复并可通过简单验证。
5. `core0_main()` 的业务逻辑开始向 app 层收敛。
6. `libraries/` 不被重写，逐飞库继续作为稳定底座使用。
7. 后续迁移到五层架构时，不需要推翻第一阶段的目录和接口命名。
