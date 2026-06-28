<div align="center">

# 🏎️ GS_Smart_car

**AURIX TC264D 智能循迹小车 · 嵌入式控制工程合集**

摄像头循迹 · 编码器测速 · 陀螺仪积分 · PID 闭环控制

![Platform](https://img.shields.io/badge/Platform-AURIX%20TC264D%20Dual%20Core-blue?style=flat-square)
![Language](https://img.shields.io/badge/Language-C99-orange?style=flat-square)
![Framework](https://img.shields.io/badge/Framework-SEEKFREE%20TC264-purple?style=flat-square)
![Architecture](https://img.shields.io/badge/Architecture-Five%20Layer-teal?style=flat-square)
![License](https://img.shields.io/badge/License-GPL--3.0-red?style=flat-square)

</div>

---

## 📌 项目简介

本仓库是一台基于 **Infineon AURIX TC264D 双核 MCU** 的智能循迹小车完整工程，采用**五层架构**设计，底层依赖逐飞科技（SEEKFREE）TC264 开源库。

> 🔗 **固件代码位于 [`master`](https://github.com/ParacosmYy/GS_Smart_car/tree/master) 分支**，本分支（`main`）为项目展示入口。

### 核心能力

| 能力 | 传感器 | 算法 |
|:---:|:---|:---|
| 📷 视觉循迹 | MT9V03X 灰度摄像头 (188×120 @ 30fps) | OTSU 自适应二值化 → 滤波 → 边线检测 → 加权中线 |
| ⚙️ 速度闭环 | 左右轮方向编码器 | 增量式 PID (10ms 采样周期) |
| 🧭 航向估计 | ICM20602 六轴陀螺仪 | 零漂补偿 + Z 轴角速度积分 |
| 🎯 转向控制 | 舵机 (50Hz PWM) | 位置式 PID (偏差驱动) |
| ⚡ 驱动输出 | H 桥电机驱动 (20kHz PWM) | 双轮差速控制 |

---

## 📐 系统架构

```mermaid
graph TD
    subgraph APP ["🚀 App 层"]
        A1["smartcar_app.c<br/>主循环编排"]
    end
    subgraph SVC ["🧠 Service 层"]
        V["vision.c · 视觉处理"]
        C["control.c · PID 决策"]
        P["pid.c · 通用 PID"]
    end
    subgraph BSP ["⚙️ BSP 层"]
        M["motor.c · 电机"]
        S["servo.c · 舵机"]
        D["display.c · TFT 显示"]
        I["input.c · 按键输入"]
    end
    subgraph SUP ["📦 支撑层"]
        CF["config.h · 参数集中"]
        DA["data.c · 共享数据"]
        IN["init.c · 系统初始化"]
    end
    subgraph VEN ["📚 Vendor"]
        L["libraries/ · 逐飞库 + iLLD"]
    end

    A1 --> V --> C
    A1 --> C
    C --> M & S
    A1 --> D
    M & S --> L

    style APP fill:#e1f5fe,stroke:#0288d1,stroke-width:2px
    style SVC fill:#f3e5f5,stroke:#7b1fa2,stroke-width:2px
    style BSP fill:#e8f5e9,stroke:#388e3c,stroke-width:2px
    style SUP fill:#fff3e0,stroke:#f57c00,stroke-width:2px
    style VEN fill:#fce4ec,stroke:#c62828,stroke-width:2px
```

> **依赖铁律：** `App → Service → BSP → Vendor`，单向向下，严禁反向调用。

---

## 🔄 数据流水线

```mermaid
flowchart LR
    CAM["📷 摄像头"] -->|灰度图像| VIS
    ENC["⚙️ 编码器"] --> ISR
    GYRO["🧭 陀螺仪"] --> ISR
    ISR["⏱️ ISR 10ms"] -->|轮速快照| CTL
    VIS["👁️ 视觉处理"] -->|中线偏差| CTL
    CTL["🎯 PID 控制"] --> ACT
    ACT["⚡ 执行输出"] --> SRV["🔧 舵机"]
    ACT --> MTR["🛞 电机"]
```

每帧执行流程：`Vision_Process() → Control_Update() → Actuator_Apply()`

---

## 📁 工程结构

> 完整代码见 [`master`](https://github.com/ParacosmYy/GS_Smart_car/tree/master) 分支

```
GS_Smart_car/                       (master 分支)
├── code/                           ★ 自研代码 — 五层架构
│   ├── app/                        │  应用层：主循环编排
│   ├── service/                    │  算法层：视觉 + 控制 + PID
│   │   ├── vision/                 │    ├ OTSU 二值化 / 边线检测 / 加权中线
│   │   └── control/                │    └ PID 决策 / 舵机电机输出
│   ├── bsp/                        │  驱动层：电机 / 舵机 / 显示 / 输入 / 蜂鸣器
│   ├── config/                     │  配置层：所有可调参数 (config.h)
│   └── common/                     │  公共层：全局变量 / 初始化 / 工具函数
│
├── libraries/                      逐飞库 + Infineon iLLD (只读)
├── user/                           SDK 入口：CPU0/CPU1 main + ISR
├── tests/                          主机端单元测试 (gcc 编译)
├── ARCHITECTURE.md                 📖 详细架构文档 (329 行)
└── .cproject                       AURIX Development Studio 工程文件
```

---

## ⚙️ 参数配置

所有可调参数集中在 **`code/config/config.h`**，调参只需改一个文件：

| 参数组 | 宏定义 | 说明 |
|:---:|:---|:---|
| 舵机 | `SERVO_CENTER_DUTY` / `SERVO_RANGE` | 中位 PWM / 最大偏转范围 |
| 电机 | `MOTOR_CLAMP_LEFT` / `MOTOR_CLAMP_RIGHT` | 左右轮速度限幅 |
| PID | `SERVO_PID_KP` / `SERVO_PID_KD` | 舵机比例/微分增益 |
| PID | `MOTOR_PID_KP` | 电机比例增益 |
| 视觉 | `LOST_LINE_THRESHOLD` | 丢线停车阈值 |
| 定时 | `PIT_PERIOD_MS` | PIT 中断周期 (ms) |

---

## 🚀 快速开始

<details>
<summary><b>📋 编译 & 烧录</b></summary>

```bash
# 1. 克隆工程 (master 分支包含完整固件)
git clone -b master https://github.com/ParacosmYy/GS_Smart_car.git

# 2. 用 AURIX Development Studio 打开工程目录
#    File → Open Projects → 选择根目录 → Build

# 3. 烧录
#    右键工程 → Flash → Flash Device (DAP MiniWiggler)
```

</details>

<details>
<summary><b>🧪 主机端测试（无硬件）</b></summary>

```bash
gcc -Itests/stubs -Icode/common tests/test_my_abs.c code/common/utils.c -o test.exe
./test.exe
```

</details>

---

## 🧩 模块职责矩阵

| 模块 | 位置 | 输入 | 输出 | 可独立测试 |
|:---:|:---|:---|:---|:---:|
| **视觉** | `service/vision/` | 灰度图像 | 中线偏差 `calculate_error` | ✅ |
| **控制** | `service/control/` | 偏差 + 轮速 | PID 输出 → PWM | ✅ |
| **电机** | `bsp/motor.c` | 速度指令 | H 桥 PWM | ❌ 需硬件 |
| **舵机** | `bsp/servo.c` | PWM 占空比 | 50Hz 舵机信号 | ❌ 需硬件 |
| **显示** | `bsp/display.c` | 边线/中线数据 | TFT180 画面 | ❌ 需硬件 |
| **配置** | `config/config.h` | — | 所有可调参数 | ✅ 纯宏 |

---

## 🎬 实车演示

> 📌 **以下位置可添加实车运行动图**

| 直道循迹 | 弯道过渡 | TFT 调试画面 |
|:---:|:---:|:---:|
| ![](docs/images/demo_straight.gif) | ![](docs/images/demo_curve.gif) | ![](docs/images/demo_tft.gif) |

---

## 🔧 开发指南

<details>
<summary><b>➕ 如何添加新功能（以超声波避障为例）</b></summary>

```
1. code/bsp/ultrasonic.c/h      ← 驱动层：HC-SR04 触发 + 回波计时
2. code/config/config.h          ← 加 ULTRASONIC_THRESHOLD 参数
3. code/app/smartcar_app.c       ← RunOnce() 中插入 Obstacle_Check()
4. .cproject                     ← 如需，加 include path
```

**原则：** 驱动放 `bsp/`，算法放 `service/`，参数放 `config.h`。依赖单向向下。

</details>

<details>
<summary><b>📝 提交规范</b></summary>

```
type(scope): 中文描述

type:  feat | fix | refactor | docs | chore | test
scope: app | vision | control | bsp | config | common | isr | core
```

- ❌ 不提交 `Debug/`、`.o`、`.elf`、`.hex` 等构建产物
- 📦 `libraries/` 改动与应用改动分开提交
- 💬 注释用简单中文，技术术语保留英文（PID / PWM / OTSU / DMA）

</details>

---

## 🗺️ 项目里程碑

- [x] 仓库卫生（.gitignore + 构建产物清理）
- [x] 五层架构重构（App / Service / BSP / Config / Common）
- [x] 死代码清理（-334 行，20+ 符号移除）
- [x] 模块物理分离（vision / control 独立目录）
- [x] 参数集中化（config.h 统一管理）
- [x] GBK → UTF-8 编码迁移
- [x] 全量中文注释 + ARCHITECTURE.md
- [x] 企业级 README
- [ ] ADS 编译验证
- [ ] ISR 算法迁移到主循环
- [ ] CPU1 视觉处理 offload（双核并行）

---

## 📚 相关文档

| 文档 | 位置 | 说明 |
|:---|:---|:---|
| 📖 架构文档 | [`ARCHITECTURE.md`](https://github.com/ParacosmYy/GS_Smart_car/blob/master/ARCHITECTURE.md) | 五层架构详解、数据流、扩展指南 |
| 📋 固件 README | [`master/README.md`](https://github.com/ParacosmYy/GS_Smart_car/blob/master/README.md) | 固件工程详细说明 |

---

<div align="center">

**Made with ⚡ by Paracosm**

MIT License · 基于 [SEEKFREE TC264](https://seekfree.taobao.com/) 开源库 (GPL-3.0)

</div>
