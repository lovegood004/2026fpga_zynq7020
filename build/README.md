# 构建与实现报告

可执行入口 `scripts/build.py` 默认 PLAN，所有目标均为离线；参数与命令见根 README。`verify.py` 检查交付清单、XSA 内嵌 bit 和 BOOT 实际载荷。执行命令必须指定新的输出目录；implement 使用之前 project 的同一目录。

`reports/hls` 保存 HLS 综合报告；`reports/routed` 保存 2026-11-03 normal 八路实现的 timing/utilization/CDC/DRC/IO/methodology 报告及审核记录。该实现使用 LUT 43290、FF 45513、BRAM36 131、DSP 214，setup/hold 裕量为 +0.183/+0.040 ns。检查资源预算时应使用本实现对应的策略；技能包演示中的 80% 预算不适用于该实现。

`qor-policy.json` 仅对随包历史报告重读：预算 100% 表示容量检查，不是保留裕量的项目预算；4 个输入/6 个输出延迟例外沿用该报告的限定对象，仍需独立 IO/CDC 审核。命令：

```powershell
python skill/zynq-validation/scripts/zynq_validate.py --report build/generated/qor-001.json qor --timing build/reports/routed/timing.rpt --utilization build/reports/routed/utilization.rpt --policy build/qor-policy.json
```

验证记录按用途存放：

- `reports/reproducibility`：历史交付文件与独立重建检查。
- `reports/organization`：仓库路径下的 ARM/FSBL/BOOT 重建、FSBL 链接顺序检查，以及 Vivado 工程生成与 BD 验证。该组检查未执行全系统综合、布局布线或 HLS co-sim。
- `reports/support-additions`：PW 独立 C 仿真和 ESP 依赖文件检查。PW 综合/RTL co-sim、ESP 完整编译尚未验证。

重建结果的适用范围以相应记录中的工具版本、输入哈希和执行阶段为准；历史报告不能替代修改后的重新验证。

`organization-sources.json` 记录源工程路径、仓库路径及对应哈希，适配过的文件在 kind 中注明；`FILE_MANIFEST.json` 记录交付文件的大小和哈希，不包含清单自身、generated 和 .git。校验仅覆盖清单列出的文件，后来加入的报告等文件需另行纳入清单。

`scripts/archive/lane8` 中的全系统 Tcl 含历史路径，仅供追溯构建记录。实际构建使用 `scripts/build.py` 入口。
