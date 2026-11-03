# 仿真与验证

`run_host.py` 编译生产八路 CNN、独立原始参考及 test_exact.cpp，验证 25 组合法形状（含完整层）与 1 组捕获输入，共 26 组逐位比较，并检查 7 组非法尺寸与输出守卫。主机数值回归不代表 RTL 时序或实板收益。

`cnn/tb.cpp` 与 `linear/tb.cpp` 是 HLS testbench；通过 `build/scripts/build.py hls-cnn|hls-linear` 使用。卷积回放必需 `data/cnn_replay/` 三个二进制。主机 hls_stream 替身只在 host 测试中使用。

PW 的 testbench 和 30 层数据位于 `src/pl/hls/pw/`，通过 `build/scripts/build_pw.py` 独立运行。命令、数值容差及 C/RTL 覆盖范围见该目录 [README](../src/pl/hls/pw/README.md)。

- `results/lane8/`：历史 host/HLS/布局布线后仿真证据，以及功耗限制和资格记录。
- `results/cosim/cnn/`：历史 HLS co-sim 报告和事务延迟；其结论不覆盖整系统或实际布线后的数值验收。
- `results/organization/`：仓库路径下执行的主机回归日志与验证记录。
- `data/golden/lane8/post_evidence.zip`（仓库根目录下）：黄金输出与 RTL 输出归档，用于按端口比较完整事务。

历史 receipt 使用 AX7020_Project 路径，可通过 `build/organization-sources.json` 查找对应文件。记录中未收录的实验脚本及 `build/scripts/archive`、`board/scripts/archive` 内含旧路径的脚本仅供追溯。修改 RTL、重新布局布线或更换硬件后，需重新运行相应测试并保存独立结果。
