---
name: zynq-validation
description: 验证 Zynq-7000 的 PS/PL 工程，比较黄金参考、检查 Vivado 资源与时序报告、核验交付文件并做独立目录重建；提供从 AX7020 项目提炼的结构化故障定位清单。适用于离线验证与工程交接。
---

# Zynq 工程验证

使用 Python 3.10+ 标准库脚本 `scripts/zynq_validate.py`。跨平台离线检查无需 Vivado；生成新报告或实际重建需要项目配套工具链。先读目标工程自身 AGENTS 和状态文档，确定器件、工具版本、PS/PL 边界、黄金参考来源与所需验收层级。

本技能从 AX7020_Project 提炼；不继承该项目的旧固件身份、引脚、地址或警告豁免。其他 Zynq 器件先适配报告格式和资源行；当前解析器面向 Vivado 2021.1 Zynq-7000 文本报告，遇到其他格式应明确报错并补充样例。

## 工作流程

1. 确定验证范围：数值、RTL/真实 BD、布局布线、驱动、交付完整性或实际重建。读 [命令与配置](references/usage.md) 选择脚本子命令。
2. 黄金参考必须独立于被测实现。固定输入、模型/权重、量化、布局、端序及参考生成器身份；按接口选择 byte/HLS word exact 或事先定义的数值容差，不因失败放宽容差。HLS 十六进制 word 比较不会将浮点原始位模式转换成浮点。
3. QoR 检查使用同一设计的 routed 报告。默认零缺失约束、零时序违例，资源预算由项目提供。非零例外逐项说明对象范围/原因/数量，不照抄 AX7020 的 LCD 例外。DRC、CDC、IO、电气和功耗仍需独立审核；仅 QoR PASS 不代表这些已过。
4. 冻结输入清单后核验哈希；需要证明可重建时运行两次新目录构建。清单包含所有源文件、IP fileSets、约束、BD/Tcl、BSP、权重、脚本及构建资源。历史 receipt 的 source/report SHA256 必须对应当前输入，不能修改哈希使旧证据通过。
5. 失败时按 [结构化踩坑清单](references/pitfalls.json) 中的触发条件、排查步骤和关闭条件定位。迁移本项目或解释证据时读 [项目提炼依据](references/ax7020-evidence.md)。
6. 输出新 JSON receipt，写明范围、输入/产物哈希、命令、工具版本、通过项及未覆盖项。PLAN 返回码 2，不能当通过；PASS 为 0，失败/数据缺失为 1，CLI 参数错误为 2。

脚本只读验证输入、写新报告；reproduce 默认只列计划，`--execute` 才执行用户提供的构建命令。检查命令只做离线构建，不包含板卡/Flash/SD操作。不会自动生成候选或部署。对工程已有硬件操作限制继续按该工程要求执行。

## 交付与复用

- `assets/qor-policy.strict.json`：无例外的起点；80% 只是演示预算，迁移时按项目要求修改。
- `assets/qor-policy.ax7020-historical.json`：仅用于重读特定历史 acquisition-preview 报告，不能用作新工程通用豁免。
- `assets/manifest-spec.example.json`、`assets/rebuild-spec.example.json`：最小演示，真实项目需补齐输入闭包和构建命令。
- `scripts/test_zynq_validate.py`：脚本故障注入测试；`python scripts/test_zynq_validate.py`。

整个目录是可搬移技能包；安装到 Codex 的 skills 目录时保留目录名 `zynq-validation`。项目内版本可直接按路径调用脚本，不依赖安装位置。
