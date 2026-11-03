# FSBL 链接顺序

重建 FSBL 时，使用 `source/link_order.json` 指定的对象顺序、配套 BSP 和构建脚本中的库顺序。按字母重新排序对象会生成不同的 ELF，影响与 `board/firmware/fsbl.elf` 的逐字节一致性。

在仓库根目录通过 `python -B build/scripts/build.py fsbl` 查看构建计划；实际执行参数见根 README。链接顺序与产物比较记录位于 `build/reports/organization/`。
