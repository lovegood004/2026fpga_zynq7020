# ESP021 辅助桥依赖与构建

目标为 ESP32-S3，构建基准为 ESP-IDF 6.0.2。Arduino/BLE 3.3.10 组件位于 `src/deps/espressif__arduino-esp32/`，仅包含本项目选用的组件子集。

CMake 默认使用该目录中的核心库、BLE、Preferences 和 ESP32-S3 板级头文件。依赖源文件的存在性、版本和配置检查结果见 `build/reports/support-additions/VALIDATION.json`；来源映射见 `build/organization-sources.json`。第三方说明与许可位于组件目录内。

## 构建步骤

先自行安装并导出 ESP-IDF 6.0.2 工具环境，在仓库根目录运行。默认使用随包组件，不自动下载其他库；选用不存在的新构建目录：

```powershell
$repoRoot = (Get-Location).Path
python "$env:IDF_PATH/tools/idf.py" `
  -C "$repoRoot/src/esp32" `
  -B "$repoRoot/build/generated/esp021-001" `
  "-DIDF_TARGET=esp32s3" `
  "-DSDKCONFIG=$repoRoot/build/generated/esp021-001/sdkconfig" `
  "-DSDKCONFIG_DEFAULTS=$repoRoot/src/esp32/sdkconfig.defaults;$repoRoot/src/esp32/sdkconfig.reference_oct80" `
  build
```

需要引用外部组件时，加 CMake 参数 `-DHS_REFERENCE_ARDUINO_COMPONENT=<自己的组件目录>`；仅设置同名系统环境变量不会被当前 CMake 自动读取。共享协议源码沿用 `src/ps/src/ui/raw.cpp`、`raw.hpp`。

## 验证状态与硬件配置

现有检查覆盖依赖版本、配置和选用源文件的存在性，尚无随包组件配置下的完整 ESP 编译或刷写验证。上述命令依据项目 CMake 配置提供，仍需在 ESP-IDF 6.0.2 环境中验证。

配套固件 `board/firmware/esp021/esp021_app.bin` 的 SHA256 前 8 位为 `e8d320d4`。历史离线重编 app 与该文件字节不同，原因尚未确认，因此源码构建与配套固件的一致性仍待验证。构建前还需按实际模块核对 `sdkconfig.reference_oct80` 中的 Flash/PSRAM 配置。
