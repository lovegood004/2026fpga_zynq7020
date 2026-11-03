# 源码索引

- `pl/rtl`：采集统计、LCD SPI/FIFO 与按键 RTL。
- `pl/hls/cnn`、`pl/hls/linear`：八路 FP32 卷积与 int8 Linear HLS 源码。
- `pl/hls/pw`：PW HLS、testbench、30 层数据、第三方依赖及许可；独立验证命令见该目录 [README](pl/hls/pw/README.md)。
- `pl/ip/{pw,cnn,linear}`：整机构建使用的固定 IP，包含 component.xml、RTL、约束及驱动文件。
- `pl/constraints`：AX7020 的 UART/LCD/按键 XDC；更换板卡时需重新核对引脚和电气标准。
- `pl/base_bd.tcl`、`pl/create_project.tcl`：PS DDR/时钟/AXI 配置及全系统组装；入口接受输出目录参数。
- `pl/implement.tcl`、`pl/remap_controls.tcl`：离线综合、布局布线和控制集调整。
- `pl/routed.dcp`：2026-11-03 normal 八路实现的布局布线 checkpoint。
- `ps/src`：ARM UI、分析、模型、权重、显示资源、驱动和性能优化；`ps/bsp` 是匹配硬件平台的 BSP；`ps/commands.json` 为可替换根路径的完整 30 对象编译与链接命令。
- `boot`：标准 FSBL 源码及配套 BSP；链接时遵循 `boot/source/link_order.json` 和 [链接顺序说明](boot/LINK_ORDER.md)。
- `esp32`：ESP021 辅助桥，构建步骤见 [DEPENDENCIES.md](esp32/DEPENDENCIES.md)。
- `deps/espressif__arduino-esp32`：ESP 辅助桥使用的 Arduino/BLE 组件子集。

ARM 构建依赖模型权重、BSP、显示资源和生成的 runtime include，使用时需保留完整源码目录及其相对路径。现有重建记录中的 ARM ELF 与配套固件逐字节一致，记录见 `build/reports/organization/`。
