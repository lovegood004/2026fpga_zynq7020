# PW HLS、测试数据与验证

本目录提供 PW 的 HLS 源码、数值参考测试和 30 层回放数据，可独立执行 C 仿真、HLS 综合及 RTL 协同仿真。

| 路径 | 内容 |
|---|---|
| src/ | pw_shared.cpp、pw_shared.hpp，PW HLS 与边界定义 |
| tests/tb_shared.cpp | 数值参考、边界与非法参数测试 |
| data/ | 30 层，90 份 FP32 二进制和 layers.txt |
| vendor/ | 43 个 BLAS 头文件、说明与许可文件 |
| hls/ | 历史 run_shared.tcl、resume_cosim.tcl、export_ip.tcl 脚本 |

## 可移植验证入口

从仓库根目录运行；安装 Vitis HLS 2021.1，替换本机路径，输出目录必须不存在：

```powershell
python -B build/scripts/build_pw.py
python -B build/scripts/build_pw.py --hls C:/Xilinx/Vitis_HLS/2021.1 --stage csim --output build/generated/pw-csim-001 --execute
```

默认仅输出 PLAN。csim 编译 PW HLS 和 testbench，运行 30 层完整数据、6 组合成形状和 7 种非法参数，共 43 次调用。`--stage synth` 在 C 仿真后追加 HLS 综合，`--stage cosim` 再追加 Verilog co-sim；各阶段运行时使用新的输出目录。现有独立入口验证记录仅覆盖 csim，结果见仓库根目录下的 `build/reports/support-additions/`；synth/cosim 尚待验证。

testbench 容差为 `2e-6 + 2*gamma*sum(abs(w*x))`，其中 `gamma=ci*2^-24/(1-ci*2^-24)`。C 模式完整检查全部 30 层；RTL 模式第 0 层完整，其他层最多检查 65 个位置，另有合成形状与非法参数测试。RTL 测试范围不覆盖全部 30 层的完整长度。

## 与现有整机的关联

整机构建使用 `src/pl/ip/pw` 中的固定 IP。独立入口只生成验证工程，不导出或替换整机 IP。重新生成 IP 后，还需验证其与配套接口、数值结果和整机实现的一致性；C 仿真结果不覆盖这些检查。

历史 `hls/run_shared.tcl` 使用 `shared/solution`，`export_ip.tcl` 打开 `ip_export/solution`，二者对应不同工程目录，不能直接顺序串联。使用本仓库时优先运行上述独立入口；历史脚本可能在源码目录中创建或重置 build 目录。
