# Bomb

在名为「TwT」的幻想世界里，疯狂的 Dr. Strangelove 挑起了一场可怕的战争。为了生存，我们必须模拟原子弹的区域破坏范围，寻找最适合建造掩体的地点。

我们偶然找到了一段来自上个世纪的"人工智障"遗留代码，它能进行模拟，但性能低下、效率惨不忍睹。磨刀不误砍柴工，在全面开展模拟之前，我们要先对这段代码进行深度加速优化。

然而，Dr. Strangelove 的间谍偷走了所有硅矿，我们如今只能依靠一颗单核心CPU！在这样的限制下，我们必须榨干单核处理器的潜能，利用其所有底层结构与特性，把性能压榨到极致！

## 性能分析

本题的核心考察点是 **性能分析 (Profiling)**。优化流程是一个循环：

1. 使用工具找到程序的性能热点
2. 针对瓶颈修改代码
3. 再次分析与验证

### 推荐工具

- **perf** (Linux 性能分析工具)
- **Intel VTune** (强大的性能分析器)
- **toplev** ([andikleen/pmu-tools](https://github.com/andikleen/pmu-tools)) - Top-Down 分析

### 使用方法

```bash
# 在子目录中使用
cd perlin
make perf SIZE=M        # 使用perf分析
make toplev SIZE=M      # 使用toplev分析

cd splat
make perf SIZE=M
make toplev SIZE=M
```

**Hint**: 部分任务本身很简单，是人为构造的性能瓶颈；但有些优化思路实现难度高，往往需要对整块代码做重构。需要你根据工程量和性能进行取舍。

## 模块说明

### 1. Perlin 模块

生成复杂战场地形，基于程序化噪声与物理侵蚀模拟：

- 使用 XorShift 随机种子构建 `256×2` 置换表
- 基于 Perlin Noise 计算多频率噪声
- 再进行基于粒子 (droplet) 的水蚀迭代

### 2. Splat 模块

在生成的地形上模拟成千上万次爆炸冲击（战场最终只剩下一片灰烬）

### 3. Validate 模块

提供结果正确性验证工具：

- `validate.cpp`: 正确性检查程序
- `diff.cpp`: 提供 `visualize` 结果的可视化比较

## 常用命令

### 基础命令

```bash
make all              # 编译所有模块
make test             # 运行完整测试 (S/M/L)
make benchmark        # 性能对比测试
make validate         # 验证输出正确性
make run              # 运行完整pipeline
make visualize        # 生成可视化图像
make clean            # 清理所有生成文件
```

### 参数

```bash
SIZE={S|M|L}          # 设置测试尺寸（默认: S）
SEED=<number>         # 设置随机种子（默认: 42）

# 示例
make dev SIZE=M
make benchmark SIZE=L SEED=123
```

## 文件结构
```
.
├── Makefile                
├── README.md               
├── framework/              
├── perlin/
│   ├── submit_perlin.cpp   # 你需要优化的版本 (ONLY SUBMIT)
│   ├── baseline_perlin.cpp # 固定的基线实现（不要改）
│   ├── harness_perlin.cpp  
│   ├── gen_perlin.py       
│   └── Makefile
├── splat/
│   ├── submit_splat.cpp    # 你需要优化的版本 (ONLY SUBMIT)
│   ├── baseline_splat.cpp  # 固定的基线实现（不要改）
│   ├── harness_splat.cpp   
│   ├── gen_splat.py        
│   └── Makefile
├── validate/
│   ├── validate.cpp        # 正确性检查
│   └── diff.cpp            # 可视化
└── utils/                  
    ├── benchmark_perlin.py
    ├── benchmark_splat.py
    ├── track_performance.py
    └── aliases.sh
```

## 正确性验证


```bash
make validate

make validate COMPONENT=perlin
make validate COMPONENT=splat
```
hash错不一定不能通过, 验证器允许 0.5% 以内的小范围误差。

## 优化建议

1. **先保证正确性**: 使用 `make validate` 确保输出正确
2. **从小到大测试**: S → M → L 逐步测试
3. **使用profiling工具**: 找到真正的热点
4. **迭代优化**: 小步快跑，每次改进后立即测试

## 注意事项

- **不要修改** `baseline_*.cpp` 文件
- **只提交** `submit_*.cpp` 文件
- 所有raw文件保存在 `validate/` 目录
- 使用 `make clean` 清理所有构建产物

## 评测环境和参数

**CPU**: Intel(R) Xeon(R) Platinum 8358 CPU @ 2.60GHz单核

**内核**: 5.14.0-503.21.1.el9_5.x86_64

**g++** : 13.3.0

**编译参数**:  -O2 -march=native -fno-tree-vectorize -fno-tree-slp-vectorize
