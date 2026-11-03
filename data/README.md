# 测试数据与黄金参考

- `cnn_replay/input_a.bin`、`weight_a.bin`、`bias_a.bin`：CNN 的 FP32 中间层回放张量，不含原始患者录音。按 testbench 的二进制 float 格式读取，测试形状为 ci=128、co=2、h=16、w=59，覆盖前两个输出通道。文件可能包含更大的缓冲内容，读取长度以 testbench 为准。
- `golden/lane8/post_evidence.zip`：captured/legacy 事务数据及仿真产物。归档内的 `summary.json` 与仓库根目录下的 `sim/results/lane8/post_receipt.json` 记录测试范围和哈希。
- PW 的 30 层输入、权重、参考输出和层参数位于 `src/pl/hls/pw/data/`（仓库根目录下），由 PW testbench 读取。
- 24 次实板对比 CSV 与结果位于 `board/results/performance`，便于与实板运行日志一起查阅。

完整事务比较可使用 `skill/zynq-validation` 的 hls 模式，对 mem、ap_return 等输出端口分别比较全部事务。归档哈希用于确认文件身份；修改设计后的数值验证需重新运行仿真。
