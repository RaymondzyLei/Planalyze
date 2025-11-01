# Planalyze - 轻量级日程管理与任务规划系统
![](/readmepic/zh.png)
## 项目简介

Planalyze 是一个功能强大的日程管理与任务规划系统，作为程序设计课程的大作业项目开发。系统结合了C++后端的高效数据处理能力和现代Web前端的友好交互体验，为用户提供直观的日程安排和任务管理功能。

## ✨ 功能特点

### 核心功能
- 📅 **智能日历视图** - 基于FullCalendar的现代化日历界面
- 📝 **任务管理** - 支持任务的增删改查操作
- ⚡ **快速命令行处理** - C++后端提供高效的数据处理能力
- 🎨 **响应式设计** - 适配各种设备尺寸的Bootstrap界面
- 🔔 **提醒功能** - 重要日程和任务的及时提醒(todo)

### 技术特色
- 前后端分离架构
- 实时数据同步
- 模块化组件设计
- 跨平台兼容性

## 🛠 技术栈

### 后端技术
- **C++17** - 核心业务逻辑处理
- 命令行接口快速响应
- 高效内存管理
- 文件系统操作

### 前端技术
- **HTML5** - 页面结构
- **CSS3** - 样式设计
- **JavaScript ES6+** - 交互逻辑
- **Bootstrap** - UI框架
- **FullCalendar** - 日历组件

## 🚀 快速开始

### 环境要求
- C++编译器 (GCC 9.0+ 或 Clang 10.0+)
- 现代Web浏览器 (Edge 90+, Chrome 90+, Firefox 88+, Safari 14+)
- python 3或vscode插件[Live Sever](https://marketplace.visualstudio.com/items?itemName=ritwickdey.LiveServer),用于构建服务端环境
- 没有其他要求了，很方便不是吗

### 安装步骤

1. **克隆项目**
```bash
git clone https://github.com/RaymondzyLei/Planalyze.git
cd Planalyze
```
或者下载源代码

2. **编译后端程序**

编译planalyze.cpp得到planalyze.exe

3. **启动前端服务**
```bash
python -m http.server 8000
```
或者使用vscode插件[Live Sever](https://marketplace.visualstudio.com/items?itemName=ritwickdey.LiveServer)

4. **访问应用**
如果你是用的是python，打开浏览器访问 `http://localhost:8000`

## 📁 项目结构

```
planalyze/
├── planalyze.cpp            # 主程序源代码
├── json.hpp                 # JSON库头文件
├── data/                    # 数据存储目录
│   ├── bg.jpg               # 背景图片
├── data.json                # 任务数据存储文件
├── docs/(todo)              # 文档目录
├── index.html               # 中文界面
├── index-en.html            # 英文界面
├── .gitignore               # Git忽略文件配置
├── LICENSE                  # 开源许可证
└── README.md                # 项目说明文档
```

## 💡 使用说明

### 基本操作

1. **增删任务**
   - 通过命令行快速添加
   - 通过命令行查看任务ID并进行修改、删除

2. **查看日程**
   - 日历视图显示所有安排
   - 列表视图按时间排序显示任务

3. **管理任务**
   - 标记任务完成状态
   - 设置任务优先级

### 命令行接口示例

```bash
# 查看帮助
./planalyze.exe

# 添加任务
./planalyze.exe -a

# 查看任务
./planalyze.exe -l

# 删除任务
./planalyze.exe -r
```

## 🔧 开发指南

### 前端开发
前端基于Bootstrap和FullCalendar构建，支持自定义主题。

### 后端开发
使用[JSON for Modern C++](https://github.com/nlohmann/json)将用户通过命令行输入的日程数据存放在data.json中

## 🤝 贡献指南

我们欢迎社区贡献！请遵循以下步骤：

1. Fork 本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. Pull Request

## 📄 开源协议

本项目以及使用的开源项目均采用 MIT 协议 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

感谢以下开源项目的支持：

- https://getbootstrap.com/ - 现代化的响应式前端框架
- https://fullcalendar.io/ - 功能丰富的JavaScript日历库
- https://github.com/nlohmann/json - JSON for Modern C++
- 程序设计课程提供的学习机会和指导

## 📞 联系方式

如有问题或建议，请通过以下方式联系：

- 项目主页：https://github.com/RaymondzyLei/Planalyze
- 邮箱：raymond.zy.lei@outlook.com
- Issues：https://github.com/RaymondzyLei/Planalyze/issues

---

这是我第一次做项目，如果有任何错误或者不好的地方，请告诉我，如果你喜欢这个项目，不妨给我一个Star

**Planalyze** - 让日程管理变得更简单高效！ 📅✨