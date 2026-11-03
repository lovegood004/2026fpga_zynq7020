# Zynq 验证技能包

`zynq-validation/` 用于裸机 Zynq PS/PL 项目的黄金参考比较、资源与时序检查、文件完整性核验及独立重建验证。目录包含使用说明、校验脚本、配置示例、16 条常见问题记录及验证依据。

从仓库根目录执行：

```powershell
python skill/zynq-validation/scripts/zynq_validate.py --help
python -B skill/zynq-validation/scripts/test_zynq_validate.py
```

入口：[SKILL.md](zynq-validation/SKILL.md)，详细命令：[usage.md](zynq-validation/references/usage.md)，踩坑清单：[pitfalls.json](zynq-validation/references/pitfalls.json)。整个 zynq-validation 目录可以单独安装到 Codex skills 目录。

检查本仓库八路实现的历史资源与时序报告时，使用 `build/qor-policy.json`，命令见 [build/README.md](../build/README.md)。技能包 assets 中的 acquisition-preview 配置仅适用于其对应历史报告。新设计需设置自己的预算和约束例外；PYNQ/Linux 的 overlay、device tree 和 DMA 验证需另行适配。
