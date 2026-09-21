# v2_Experiment

一个跑在 **Windows 控制台**里的横版弹幕射击（STG）小游戏，纯 ASCII / 字符块渲染，单文件 C++ 实现。

这是当年「答辩版本」（最后一次提交名）的快照，保留原样存档。

```
   _____  _  _  _     __          __
  / ____|(_)| || |    \ \        / /
 | (___   _ | || | __  \ \  /\  / /___   _ __  _ __ ___
  \___ \ | || || |/ /   \ \/  \/ // _ \ | '__|| '_ ` _ \
  ____) || || ||   <     \  /\  /| (_) || |   | | | | | |
 |_____/ |_||_||_|\_\     \/  \/  \___/ |_|   |_| |_| |_|
```

---

## 玩法

窗口需要**最大化**后再进入游戏（游戏区固定 179 字符宽 × 41 行）。

| 按键 | 作用 |
|:---|:---|
| `W` / `S` | 上下移动 |
| `A` / `D` | 左右移动 |
| `Space` | 射击（按住连发，射速与 tick 计数绑定） |
| `J` | 释放技能：向上下两侧展开的扇形弹幕，冷却 300 tick |

**道具**（随敌机被击毁后掉落，触碰即拾取）

| 道具 | 外观 | 效果 |
|:---|:---|:---|
| 强化 | `STBB` | `strengthTick += 5`，射击时额外射出上下两发散射弹 |
| 镭射 | `LAAA` | `laserTick += 100`，机身前方拉出两条贯穿激光 |

**难度曲线**：撞上敌机 `difficulty += 1`；被子弹击中 `difficulty += 0.1`；击毁一架敌机 `difficulty *= 1.15`。
难度超过 `3.5` 后进入 **Boss 战**。

**Boss**：`湊阿库娅`，血量 44500，会随机释放 A–F 六套弹幕，并会抛物线跳跃走位。
血量跌破 40050 后弹幕频率明显提升，同时触发新台词。
Boss 战开始时才发放 `PLANE_LIFE = 5` 条命（此前 `planeLife = -1`，按显示逻辑视为不显示剩余生命）。

游戏结束时打印本局造成的总伤害（每次命中 `rand() % 30`）。

---

## 运行环境

- **Windows 10 及以上**（依赖 `windows.h` / `conio.h` / `winmm.lib`，无跨平台支持）
- Visual Studio 2022（工程工具集 `v143`）
- 无第三方依赖，单文件、无资源脚本

> 中文输出依赖控制台代码页。源码是 **UTF-8 with BOM**，中文字面量在编译期按系统 ANSI 代码页转换，
> 因此在**简体中文区域设置**下显示正常；换成其他区域或开启「Beta: 使用 UTF-8 提供全球语言支持」可能出现乱码。

---

## 构建与运行

1. 用 Visual Studio 2022 打开 `Experiment.sln`
2. 选择 `x64` / `Debug`（或 `Release`），`F5` 运行
3. 程序启动后**先把控制台窗口最大化**，再按任意键载入

### ⚠️ 关于 `#include <bits/stdc++.h>`

源码第一行用了 GCC 风格的万能头：

```cpp
#include <bits\stdc++.h>
```

MSVC 曾经附送这个兼容头，但**只在旧版本里**。本机实测：

| 工具集 | `include\bits\stdc++.h` |
|:---|:---|
| `14.37.32822`（VS 17.7） | 存在 |
| `14.44.35207`（VS 17.14+） | **已移除** |

所以用较新的 VS 2022 直接打开会编译失败。二选一：

- 通过 Visual Studio Installer 安装 **v143 生成工具 14.37**，在「项目属性 → 高级 → 平台工具集」里指定；
- 或者把这一行换成显式包含（`<iostream>`、`<string>`、`<thread>`、`<fstream>`、`<cstring>`、`<cstdlib>`、`<ctime>`、`<cmath>` 等）。

---

## 音频素材

三个音频文件是**硬编码文件名**，由 `playBgm()` 通过 `mciSendString` 播放：

| 文件 | 用途 |
|:---|:---|
| `ba.mp3` | 主 BGM（标题与常规战） |
| `th.mp3` | Boss 战 BGM |
| `msg.mp3` | 字幕逐字打印的「滴滴」音效 |

`playBgm()` 的查找顺序是：

```cpp
ifstream ifileRelease(".\\res\\"          + bgmName);   // 优先
ifstream ifileDebug  (".\\x64\\Debug\\res\\" + bgmName); // 其次
```

所以音频文件统一放在仓库的 `res\` 目录下（相对工作目录，默认即工程目录），克隆下来即可直接出声。

> `rr.mp3` 与 `bunker.mp3` 在代码中**没有任何引用**，属于未使用素材，一并放在 `res\` 里备查。

---

## 项目结构

```
Experiment.sln              解决方案
Experiment.vcxproj          VS 工程（v143 / Console 子系统）
Experiment.vcxproj.filters  筛选器
Experiment.cpp              全部游戏逻辑（约 1400 行）
res\                        音频素材（ba.mp3 / th.mp3 / msg.mp3，另有未引用的 rr.mp3、bunker.mp3）
.gitignore                  Visual Studio 标准忽略规则
.gitattributes              换行符归一化
```

### `Experiment.cpp` 内部结构

| 区块 | 内容 |
|:---|:---|
| 全局状态 | `tick` / `difficulty` / `score` / `planeLife` 等计时与统计量 |
| 精灵数据 | `char[][IMG_LENGTH]` 二维字符画：飞机、山体、太阳、地面、Boss 立绘 `trueAqua` |
| 地图 | `friendyMap` / `enemyMap` / `itemMap` 三张 `MAP_Y × MAP_X` 碰撞箱 |
| 渲染 | `render()` 负责普通字符画 + 写碰撞箱；`renderAqua()` 用「两个空格 + 背景色」拼 Boss 立绘 |
| 双缓冲 | `CreateConsoleScreenBuffer` + `ReadConsoleOutputCharacterA` / `WriteConsoleOutputCharacterA` |
| 配色 | 大部分走 `SetConsoleTextAttribute` 16 色；Boss 立绘与血条走 24 位 ANSI 转义 `\x1b[48;2;r;g;bm` |
| 音频 | `playBgm()` → `mciSendString` |
| 并发 | `std::thread` + `detach()`：入场动画、敌机重生、Boss 技能、字幕打字机 |
| 主循环 | `tick` 递增 → 输入 → 子弹 → 敌机 → 渲染 → 缓冲刷新 → 碰撞检测 → `Sleep(sleepTick)` |

---

## 已知问题 / 待办

- [ ] 新工具集缺 `bits/stdc++.h`，见上文
- [ ] 未调用 `SetConsoleMode(ENABLE_VIRTUAL_TERMINAL_PROCESSING)`，24 位 ANSI 配色在部分老控制台上不生效
- [ ] `res\rr.mp3` / `res\bunker.mp3` 未被引用，可考虑清理
- [ ] `killBullet()` / `getBullet()` 里留有「找个时间重写一个」的注释，实现是线性扫描
- [ ] 部分中文提示走窄字符 `cout`，跨区域设置下会乱码

---

## 版权与免责声明

本项目是个人学习 / 课程作品，**非商业用途**。

游戏中的角色「湊阿库娅」及随仓库附带的音频素材，其著作权归各自权利人所有，本仓库仅作存档与学习交流之用。
不使用其中的音乐或角色素材进行任何商业传播；如需复用，请自行替换为可自由使用的素材。
