# AX7020 提炼依据与适用范围

本包提炼自 AX7020_Project 的现有脚本和文本证据，不复制业务模型、权重、固件或板卡标识。输入路径均由调用者传入；技能目录可整体搬移。原项目参考 Python 3.10+、Vivado/Vitis/HLS 2021.1；新项目采用其实际匹配版本。

## 决策对应

| 可复用决策 | 原项目文件 | 本包实现 |
|---|---|---|
| 输出逐 word 比较、事务完整性 | tools/verify_cnn_reference.py | compare --format hls，适配任意同格式单端口 |
| 正/负样本数值验证与范围边界 | build_work/lung_fix/post_receipt.json | 比较失败不可用日志摘要覆盖；多个端口全部检查 |
| 检查 setup/hold/脉宽、check_timing 与资源 | tools/review_acq_implementation.py | qor 加可配置预算和精确例外 |
| 验证历史证据绑定当前源码 | tools/model_evidence.py、tools/acq_evidence.py | freeze/verify，输入列表由目标项目定义 |
| 工具位置解耦 | tools/portable_paths.py、tools/path_alias.py | 不内置本机工具路径；由构建入口或 PATH 提供 |
| 独立交付核验和重建 | VALIDATION.md、PROJECT_STATE.md | reproduce 的两个新副本和产物比较 |
| 真实 BD、缓存、加载失败注入 | tools/test_acq_bd.py、tools/test_model_cache.py、tools/test_lcd_loader.py | 结构化 pitfall 的触发/步骤/关闭条件 |

pitfalls.json 中 evidence_kind 区分“项目实际记录”和“项目测试/保护机制”；不能据测试文件推断每种故障都在实板发生过。迁移其他工程时按 evidence 追溯原决策，替换项目特定对象。

## 发现的身份冲突

原 AGENTS/skill 描述旧 accepted 版本和无已验收 BOOT，而 PROJECT_STATE 的 2026-11-03 新条目记录 normal QSPI 自启动成功，同时功耗/电流仍未合格；CURRENT_RELEASE 又保留另一历史身份。由此只能得出：这些文件承担的角色与新旧程度不同。不能选择一个旧常量作为通用“当前版本”。应结合 CURRENT_BOARD_STATE、具体加载/冷启动 receipt、文件哈希和本次任务确定状态；本包没有访问板卡来确认。

## 本次验证边界

实际重读 acquisition-preview 历史 routed 文本报告，使用专属例外配置检查；不属于新综合、布局布线或新上板结果。黄金参考从原 post_evidence.zip 临时解出 mem/ap_return 两个端口并重比，不是新 RTL 仿真。自测中执行两个新目录的最小确定性构建以验证 runner；未执行耗时的 Vivado 全工程重建。原工程业务源码与固件未改。

`source-provenance.json` 记录所引用原文件的提炼时 SHA256（相对 AX7020_Project），用于追溯，不是新实测证明。交付验证报告在工作区 `outputs/zynq-skill-validation/`，不装入 skill 的运行依赖。
