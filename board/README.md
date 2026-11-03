# 上板工程、运行脚本与实测输出

`firmware/` 提供互相匹配的正常应用 ELF、bit、XSA、PS 初始化、标准 FSBL 与 BOOT；`firmware/esp021` 提供配套桥固件。版本哈希见仓库根 README 和 `build/FILE_MANIFEST.json`。

## 上板计划入口

```powershell
python -B board/scripts/deploy_ram.py
```

默认命令按 `candidate.json` 核对加载器与固件哈希，并输出 PLAN。加载器检查目标器件、下载器身份、板卡未配置状态、main 读回和外设 ID/运行状态，不执行热刷、复位或自动采集 START。

执行 RAM 加载前，板卡须冷启动且尚未配置，并确认没有需要保留的 RAM 采集数据。使用自己的下载器编号，加上参数 `--execute --jtag-serial <自己的编号> --fresh-board-records-discarded`。最后一个参数表示确认放弃 RAM 中的现有记录。

Vitis xsdb 可通过 `VITIS_BIN` 环境变量或仓库根 `tools.local.json` 的 `vitis_bin` 配置，也可由 PATH 查找。当前加载入口仅有 PLAN 验证记录，执行模式尚未完成实板验证。

QSPI 启动镜像为 `firmware/BOOT.bin`。烧写流程需针对实际板卡完成 Flash 备份、设备/模式/容量核对、烧写后独立读回和冷启动验收。仓库未提供可移植的自动 Flash/SD 擦写入口。`scripts/archive/` 中的加载、读回和冷启动脚本含历史绝对路径及设备号，仅供查阅记录；使用新板时需按实际硬件配置工具与标识。

## 实测证据

- `results/performance/BOARD_COMPARISON.md`、JSON、两份 CSV：同一个公开 15 秒输入，新旧各 24 次；旧版为历史四路，新版为八路，并非同会话交错 A/B。warm 五次均值：模型 1567.623→1169.855 ms，CNN 硬件等待 765.306→407.384 ms，进入分析至 LCD 发送完成 2733.227→2323.683 ms。保留全部慢样本；概率最大差为 0。不可外推数据集准确率。
- `results/flash/`：原写入/范围审核/独立读回/一次冷启动验收及日志。
- `results/CURRENT_BOARD_STATE.historical.json`：历史实板状态快照；当前板卡状态需现场检查。

电流、功耗与长时间热稳定性尚未验收。原始日志中的旧路径用于追溯相应测试的输入和产物。
