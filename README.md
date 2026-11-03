# 基于 Zynq 的心肺音采集与智能分析系统

AX7020（XC7Z020-2CLG400I）裸机 PS + PL 工程，包含采集协议、心肺音预处理、双模型推理和 LCD 界面。PS 承担协议/CRC、预处理、调度和绘图；PL 包含 PW、八路 FP32 CNN、Linear、LCD FIFO/SPI、按键与采集统计/波形分桶。ESP021 是配套 BLE/UART 桥。

仓库采用 2026-11-03 normal 八路版本，提供源码、测试数据、离线构建入口、配套固件和验证记录。设计报告见 [report/design_report.docx](report/design_report.docx)，大模型协作记录见 [report/llm_collaboration_records.zip](report/llm_collaboration_records.zip)。

## 目录

```text
Github/
├── README.md              项目简介与复现步骤
├── src/                   设计源码：RTL / HLS / PS / FSBL / ESP 辅助桥
│   ├── pl/                RTL、HLS、IP、约束、BD/Tcl、布局布线 checkpoint
│   ├── ps/                ARM 源码、权重、资源、配套 BSP 和编译命令
│   ├── boot/              FSBL 源码、链接顺序和配套 BSP
│   ├── esp32/             配套桥源码与依赖说明
│   └── deps/              ESP Arduino/BLE 组件子集
├── sim/                   testbench、可运行主机回归、历史仿真结果
├── build/                 可移植构建脚本、综合/实现报告、来源与完整性索引
├── board/                 匹配上板固件、默认 PLAN 的加载脚本、实测输出
├── data/                  CNN 回放输入与黄金参考输出归档
├── skill/                 可复用 zynq-validation 技能包及使用入口
│   └── README.md
└── report/                设计报告与大模型协作记录
```

## 固件版本

| 文件 | SHA256 前 8 位 | 说明 |
|---|---|---|
| board/firmware/application.elf | a9d959c5 | 正常 ARM 应用，非测时临时应用 |
| board/firmware/system.bit | e5597b4d | 匹配八路 PL |
| board/firmware/hardware.xsa | 3e69b7da | 匹配硬件平台 |
| board/firmware/fsbl.elf | acd724fa | 标准 FSBL |
| board/firmware/BOOT.bin | e82d8ae6 | 三分区 QSPI 自启动镜像 |

2026-11-03 的实板记录包含 Flash 写入、独立读回、一次冷启动正常菜单检查，以及同一个 15 秒公开输入的新旧版本各 24 次性能测试。功耗、电流和长时间热稳定性未验收，性能测试也不覆盖临床准确率。详见 [实板结果](board/README.md)。历史记录中的 CURRENT_RELEASE 对应更早版本，核对本仓库固件时以上表和完整性清单为准。

## 环境

- Python 3.10+，离线校验仅用标准库。
- Windows 上的 Vivado / Vitis / Vitis HLS 2021.1；下列安装路径仅为示例，替换为自己的位置。
- 主机回归需要提供 MinGW g++ 的 bin 路径，保持 `-ffp-contract=off -fno-fast-math`。
- ESP 单独需要 ESP-IDF 6.0.2；Arduino/BLE 3.3.10 组件子集已随包放在 `src/deps/espressif__arduino-esp32`，详见 [辅助桥依赖](src/esp32/DEPENDENCIES.md)。它不参与 Zynq 主线构建。

在仓库根目录运行以下命令。建议使用短英文路径。无需保留原 AX7020_Project 目录。

## 复现步骤

项目实际采用 QSPI Flash 启动；功能复现也可通过 JTAG 加载匹配的 PS 初始化、bit、XSA 和 ELF，无需先生成或写入 BOOT.bin。JTAG 入口与板卡状态要求见 [board/README.md](board/README.md)。

1. 核验交付文件、XSA/bit 配对以及 BOOT 实际分区载荷：

   ```powershell
   python -B build/verify.py
   ```

2. 运行主机精确数值回归（新输出目录，不覆盖旧证据）：

   ```powershell
   python -B sim/run_host.py --host-bin C:/tools/mingw64/bin --output sim/generated/run-001
   ```

3. 从 ARM/FSBL 源码生成 BOOT。第一条只显示计划，第二条执行离线构建：

   ```powershell
   python -B build/scripts/build.py flash
   python -B build/scripts/build.py flash --vitis C:/Xilinx/Vitis/2021.1 --output build/generated/flash-001 --execute
   ```

   输出 receipt 包含 `matches_reference`，表示新 BOOT 是否与交付镜像完全一致。`boot` 目标仅用已交付 ELF 重新组包；`arm`、`fsbl` 目标可分别重建。此处 flash 是“生成 Flash 镜像”，不会连接板卡或写 Flash。

4. 重建 HLS 并做 C/RTL co-sim，或生成 Vivado 工程并实现：

   ```powershell
   python -B build/scripts/build.py hls-cnn --hls C:/Xilinx/Vitis_HLS/2021.1 --output build/generated/hls-cnn-001 --execute
   python -B build/scripts/build.py hls-linear --hls C:/Xilinx/Vitis_HLS/2021.1 --output build/generated/hls-linear-001 --execute
   python -B build/scripts/build.py project --vivado C:/Xilinx/Vivado/2021.1 --output build/generated/hardware-001 --execute
   python -B build/scripts/build.py implement --vivado C:/Xilinx/Vivado/2021.1 --output build/generated/hardware-001 --execute
   ```

   HLS 输出用于单核验证，不自动替换冻结 IP。Vivado 使用随包 IP 生成全系统，最终报告/checkpoint 在指定目录 results。新综合/布局布线可能产生不同实现，需要重新完成数值、时序、CDC、DRC、引脚等验证；不自动替换已上板固件。

5. 资源与时序复核、上板计划入口和实测记录分别见 [build](build/README.md)、[board](board/README.md)、[skill](skill/README.md)。加载脚本默认仅输出计划；执行前按 board 说明配置工具和下载器，并检查板卡状态。

## 验证范围

仓库现有验证记录覆盖以下内容：

- ARM 的 30 个对象、FSBL 的 12 个对象构建及 BOOT 组包，产物与配套固件逐字节一致。工具版本、构建日志与 FSBL 链接顺序检查见 `build/reports/organization/`。
- 26 组主机精确数值比较与 7 组非法尺寸/输出守卫检查，结果见 `sim/results/organization/`。
- Vivado 2021.1 工程生成、BD 验证与生成目标，记录见 `build/reports/organization/`。随包全系统综合、布局布线及 CNN HLS co-sim 报告来自历史实现；上述工程生成验证未重新执行这些阶段。
- PW 独立 C 仿真及 ESP 依赖文件检查，结果见 `build/reports/support-additions/`。PW 独立综合/RTL co-sim 和 ESP 完整编译仍待验证。

`build/FILE_MANIFEST.json` 用于核对当前文件大小和哈希；`build/organization-sources.json` 用于查找源工程路径与仓库路径的对应关系。历史验证记录（receipt）中的 AX7020_Project 路径可结合该映射查阅，记录结论仅适用于其注明的产物和测试条件。

## PW 与 ESP 使用入口

- [PW HLS 与验证说明](src/pl/hls/pw/README.md)：包含 HLS、testbench、30 层输入/权重/参考输出和 BLAS 依赖。独立验证入口 `python -B build/scripts/build_pw.py` 默认输出计划，整机构建使用 `src/pl/ip/pw` 中的固定 IP。
- [ESP 依赖与构建步骤](src/esp32/DEPENDENCIES.md)：使用随包 Arduino/BLE 3.3.10 组件子集；需另行安装 ESP-IDF 6.0.2。
