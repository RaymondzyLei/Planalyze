# Planalyze - Lightweight Scheduling & Task Management Project
![](/readmepic/en.png)
[中文版本](README-zh.md)

## Project Introduction

Planalyze is a powerful schedule management and task planning system developed as a major programming course project. The system combines the efficient data processing capabilities of the C++ backend with the user-friendly interactive experience of modern web frontend, providing intuitive schedule arrangement and task management functions.

## ✨✨ Key Features

### Core Features
- 📅📅 **Smart Calendar View** - Modern calendar interface based on FullCalendar
- 📝📝 **Task Management** - Support for task CRUD operations
- ⚡⚡⚡ **Fast Command Line Processing** - Efficient data processing powered by C++ backend
- 🎨🎨🎨 **Responsive Design** - Bootstrap interface adaptable to various device sizes
- 🔔🔔 **Reminder Functionality** - Timely notifications for important schedules and tasks

### Technical Highlights
- Frontend-backend separation architecture
- Real-time data synchronization
- Modular component design
- Cross-platform compatibility

## 🛠🛠🛠 Tech Stack

### Backend Technologies
- **C++17** - Core business logic processing
- Fast command-line interface response
- Efficient memory management
- File system operations

### Frontend Technologies
- **HTML5** - Page structure
- **CSS3** - Styling design
- **JavaScript ES6+** - Interactive logic
- **Bootstrap** - UI framework
- **FullCalendar** - Calendar component

## 🚀🚀🚀 Quick Start

### Environment Requirements
- C++ compiler (GCC 9.0+ or Clang 10.0+)
- Modern web browser (Edge 90+, Chrome 90+, Firefox 88+, Safari 14+)
- Python 3 or VSCode extension [LiveServer](https://marketplace.visualstudio.com/items?itemName=ritwickdey.LiveServer) for building server environment
- No other requirements needed - pretty convenient, right?

### Installation Steps

1. **Clone the Project**
```bash
git clone https://github.com/RaymondzyLei/Planalyze.git
cd Planalyze
```
   Or download the source code directly

2. **Compile Backend Program**

   Compile planalyze.cpp to get planalyze.exe

3. **Start Frontend Server**
```bash
python -m http.server 8000
```
   Or use VSCode extension [LiveServer](https://marketplace.visualstudio.com/items?itemName=ritwickdey.LiveServer)

4. **Access Application**
   Open browser and visit `http://localhost:8000/index-en.html` if using python

## 📁📁 Project Structure

```
planalyze/
├── planalyze.cpp            # Main program source code
├── json.hpp                 # JSON library header file
├── data/                    # Data storage directory
│   ├── bg.jpg               # Background image
├── data.json                # Task data storage file
├── docs/ (todo)             # Documentation directory
├── index.html               # Chinese interface
├── index-en.html            # English interface
├── .gitignore               # Git ignore file configuration
├── LICENSE                  # Open source license
└── README.md                # Project documentation
```

## 💡💡 Usage Guide

### Basic Operations

1. **Add/Delete Tasks**
   - Quick addition via command line
   - View task IDs via command line for modification and deletion

2. **View Schedules**
   - Display all arrangements in calendar view
   - Show tasks sorted by time in list view

3. **Manage Tasks**
   - Mark task completion status
   - Set task priorities

### Command Line Interface Examples

```bash
# View help
./planalyze.exe

# Add task
./planalyze.exe -a

# List tasks
./planalyze.exe -l

# Remove task
./planalyze.exe -r
```

## 🔧🔧 Development Guide

### Frontend Development
Frontend is built on Bootstrap and FullCalendar, supporting custom themes.

### Backend Development
Using [JSON for Modern C++](https://github.com/nlohmann/json) to store schedule data entered by users via command line in data.json

## 🤝🤝 Contribution Guide

We welcome community contributions! Please follow these steps:

1. Fork the project
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄📄 License

This project and the open source projects used are all under MIT License - see the LICENSE file for details.

## 🙏🙏 Acknowledgments

Thanks to the following open source projects for support:

- https://getbootstrap.com/ - Modern responsive frontend framework
- https://fullcalendar.io/ - Feature-rich JavaScript calendar library
- https://github.com/nlohmann/json - JSON for Modern C++
- Programming course for providing learning opportunities and guidance

## 📞📞 Contact

For questions or suggestions, please contact us through:

- Project homepage: https://github.com/RaymondzyLei/Planalyze
- Email: raymond.zy.lei@outlook.com
- Issues: https://github.com/RaymondzyLei/Planalyze/issues

---

This is my first project. If you notice any errors or shortcomings, please let me know. If you like this project, feel free to give it a Star!

**Planalyze** - Making schedule management simpler and more efficient! 📅📅✨