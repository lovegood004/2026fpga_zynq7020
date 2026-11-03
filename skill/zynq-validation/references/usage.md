# 命令、输入契约与证据范围

以下命令从本仓库根目录运行。将技能包独立安装到其他目录后，按实际位置调整 `skill/zynq-validation` 路径。所有 `--report` 必须位于子命令之前，目标必须是不存在的新文件；失败记录需保留。除资源与时序示例外，命令中的输入文件名和项目 spec 均为占位示例，运行前替换为自己的文件。

## 黄金参考

```powershell
python skill/zynq-validation/scripts/zynq_validate.py --report results/word-check.json compare --golden reference.dat --actual rtl.dat --format hls
python skill/zynq-validation/scripts/zynq_validate.py --report results/float-check.json compare --golden reference.txt --actual actual.txt --format numeric --atol 1e-6 --rtol 1e-5
python skill/zynq-validation/scripts/zynq_validate.py compare --golden release.elf --actual rebuild.elf --format bytes
```

- `bytes`：非空黄金文件逐字节相等，适用于已固定格式/头部的固件或任意二进制。
- `hls`：`[[[runtime]]]`、`[[transaction]] ID`、十六进制 words、`[[/transaction]]`、`[[[/runtime]]]`。要求唯一 ID、顺序、事务数、每事务长度、所有 word 一致。拒绝空事务与 X/Z。不隐式掩码超宽 word；接口宽度由测试向量生成器约束。
- `numeric`：仅包含逗号或空白分隔数字的平面文件，无表头；按位置比较，需先验证形状/张量布局。公式 `abs(actual-golden) <= atol + rtol*abs(golden)`。拒绝非有限数与空输入，不默认设容差。
- 单次比较覆盖一个输出端口。多端口（例如 mem、ap_return）逐个运行，全部成功才可汇总通过。不得只比较标签、分数摘要或日志中的 PASS。
- 保存参考生成器、输入向量、模型权重、被测源码、网表/CP 和参数的哈希；compare receipt 只自动记录两个数据文件哈希。

## 资源与时序

```powershell
python skill/zynq-validation/scripts/zynq_validate.py --report build/generated/qor-001.json qor --timing build/reports/routed/timing.rpt --utilization build/reports/routed/utilization.rpt --policy build/qor-policy.json
```

`build/qor-policy.json` 仅适用于随包八路实现的历史报告。100% 预算用于器件容量检查，不表示预留了工程裕量；4 个输入与 6 个输出延迟例外仅适用于该报告限定的按键和 LCD 对象，需另行核验 IO/CDC。新工程使用 strict 配置，并按设计需要设置预算与例外，不能仅凭例外数量相同沿用结论。脚本检查总表 WNS/WHS/WPWS、TNS/THS/TPWS 及失败端点、12 项 check_timing、LUT/FF/BRAM36/DSP 利用率。BRAM36 对应 Block RAM Tile，可有小数；不把 BRAM18 数直接当 BRAM36。缺字段、非 routed 利用率、格式不匹配均失败。

新报告应由同一实现工程/同一 routed checkpoint 导出 `report_timing_summary`（含 check_timing）和 `report_utilization`；另导出 DRC、CDC、IO、methodology。保留工具日志、器件、checkpoint/约束/源码哈希，核对两份报告来自同一设计。脚本无法仅凭文本证明其来源，不能把它用作全部上板门禁。时序正裕量不等于外部接口/功耗合格。

## 完整性与重建

编写项目 spec：`files` 是相对工程根目录的显式文件列表，不支持目录或通配符；`toolchain` 是已测版本字典。可由项目依赖发现工具生成文件列表，但需包含构建脚本、IP metadata/RTL/XDC、BSP、库和权重。相对路径必须留在工程内，不允许清单通过符号链接指向工程外。

```powershell
python skill/zynq-validation/scripts/zynq_validate.py --report results/inputs.json freeze --root my_project --spec my_manifest_spec.json
python skill/zynq-validation/scripts/zynq_validate.py --report results/integrity.json verify --root my_project --manifest results/inputs.json
python skill/zynq-validation/scripts/zynq_validate.py reproduce --root my_project --manifest results/inputs.json --spec my_rebuild_spec.json --work results/rebuild-001
python skill/zynq-validation/scripts/zynq_validate.py --report results/rebuild.json reproduce --root my_project --manifest results/inputs.json --spec my_rebuild_spec.json --work results/rebuild-001 --execute
```

`freeze` 创建基线不证明其正确性；由审核过的交付版本生成并保存，不能失败后重新 freeze 来掩盖变化。`verify` 仅检查清单文件是否存在、大小和 SHA256，未列出的文件不会检查。

重建配置见 assets：commands 是无 shell 的 argv 数组，支持 `{root}`（独立构建副本）、`{python}`；不要在元素中再加命令行引号。Windows `.bat` 工具如需 cmd，显式写 `cmd.exe /d /c ...` 并检查引用；优先调用项目 Python 构建入口。timeout_seconds 是每个命令的上限，按工作量调整，超时保存日志并失败，不声称设计错误。

`reproduce` 先验证原清单，将清单内文件复制到新的 a/b 两个目录，各执行相同命令，比较配置 outputs 的完整字节，最后核验原输入未变。任何命令非零、缺输出、超时或输出不一致均失败。重建失败保留现场，不能在已有 work 上重试。执行前确认构建入口只依赖副本，且不访问硬件或写原工程。

限制：这不是操作系统沙箱，不会阻止构建脚本绝对路径访问外部依赖；工具链、环境变量、许可证和系统库由两次构建共享。版本字典只是声明，应另存 `vivado -version`、编译器/Python 版本和关键环境的脱敏日志。绝对编译路径、时间戳、种子可能导致输出不等；先定位差异，不自动剥离头部或放宽标准。需要功能可复现时，在两次产物上另外运行相同向量/数值/STA 门禁，不用位流不等直接判定功能错误。

## 迁移检查

器件容量取本工程报告，预算可调；替换引脚、IOSTANDARD、时钟、PS DDR 参数、AXI 地址、BSP 与 XSA；接口布局和缓存策略按实际驱动检查。Vivado/HLS 版本变更要重新收集报告并验证解析器，不沿用旧结论。PYNQ/Linux 的 overlay/device tree/DMA 验证需另补适配，本包源自裸机 PS/PL 工程。
