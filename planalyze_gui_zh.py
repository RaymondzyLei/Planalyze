#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Planalyze GUI - 小白用户友好的日程管理工具
"""

import tkinter as tk
from tkinter import ttk, messagebox
import subprocess
import os
import webbrowser
import threading
import sys

class PlanalyzeGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Planalyze 日程管理系统")
        self.root.geometry("450x400")
        self.root.resizable(False, False)
        
        # 设置窗口图标（如果有的话）
        try:
            self.root.iconbitmap("planalyze.ico")  # 如果有图标文件
        except:
            pass
        
        # 设置样式
        self.setup_styles()
        
        # 创建界面
        self.create_widgets()
        
        # 检查必要的文件是否存在
        self.check_required_files()
    
    def setup_styles(self):
        """设置界面样式"""
        style = ttk.Style()
        style.configure("TButton", font=("微软雅黑", 12), padding=10)
        style.configure("Title.TLabel", font=("微软雅黑", 16, "bold"))
        style.configure("Info.TLabel", font=("微软雅黑", 10))
    
    def create_widgets(self):
        """创建界面组件"""
        # 标题
        title_label = ttk.Label(self.root, text="Planalyze 日程管理系统", 
                               style="Title.TLabel")
        title_label.pack(pady=20)
        
        # 说明文字
        info_label = ttk.Label(self.root, 
                              text="欢迎使用Planalyze！请选择您需要的功能：",
                              style="Info.TLabel")
        info_label.pack(pady=10)
        
        # 按钮框架
        button_frame = ttk.Frame(self.root)
        button_frame.pack(pady=30, fill="both", expand=True)
        
        # 按钮1：日程操作
        self.btn_schedule = ttk.Button(button_frame, text="1. 日程操作",
                                      command=self.open_schedule,
                                      style="TButton")
        self.btn_schedule.pack(pady=10, fill="x", padx=50)
        
        # 按钮2：打开提醒
        self.btn_reminder = ttk.Button(button_frame, text="2. 打开提醒",
                                      command=self.open_reminder,
                                      style="TButton")
        self.btn_reminder.pack(pady=10, fill="x", padx=50)
        
        # 按钮3：查看日程
        self.btn_view = ttk.Button(button_frame, text="3. 查看日程",
                                  command=self.view_schedule,
                                  style="TButton")
        self.btn_view.pack(pady=10, fill="x", padx=50)
        
        # 状态栏
        self.status_var = tk.StringVar()
        self.status_var.set("就绪")
        status_bar = ttk.Label(self.root, textvariable=self.status_var,
                              relief="sunken", anchor="w")
        status_bar.pack(side="bottom", fill="x")
    
    def check_required_files(self):
        """检查必要的可执行文件是否存在"""
        missing_files = []
        
        if not os.path.exists("planalyze.exe"):
            missing_files.append("planalyze.exe")
        
        if not os.path.exists("server.exe"):
            missing_files.append("server.exe")
        
        if missing_files:
            messagebox.showwarning("文件缺失", 
                                  f"以下文件不存在：\n{'、'.join(missing_files)}\n\n"
                                  "请确保这些文件在当前目录下。")
    
    def update_status(self, message):
        """更新状态栏"""
        self.status_var.set(message)
        self.root.update()
    
    def open_schedule(self):
        """打开日程操作（执行planalyze.exe）"""
        def run_planalyze():
            try:
                self.update_status("正在打开日程操作...")
                
                # 获取当前工作目录
                current_dir = os.getcwd()
                
                # 在Windows上使用start命令来保持cmd窗口打开
                if os.name == 'nt':  # Windows系统
                    subprocess.Popen(f'start cmd /k "cd /d "{current_dir}" && planalyze.exe"', 
                                   shell=True)
                else:  # 其他系统（Linux/Mac）
                    subprocess.Popen(['gnome-terminal', '--', 'bash', '-c', 
                                    f'cd "{current_dir}" && ./planalyze.exe; exec bash'])
                
                self.update_status("日程操作已打开")
                
            except Exception as e:
                self.update_status("错误：无法打开日程操作")
                messagebox.showerror("错误", f"无法执行planalyze.exe：{str(e)}")
        
        # 在新线程中运行，避免界面卡顿
        threading.Thread(target=run_planalyze, daemon=True).start()
    
    def open_reminder(self):
        """打开提醒（运行server.exe）"""
        def run_server():
            try:
                self.update_status("正在启动提醒服务...")
                
                # 运行server.exe
                subprocess.Popen("server.exe", shell=True)
                
                self.update_status("提醒服务已启动")
                messagebox.showinfo("成功", "提醒服务已启动！")
                
            except Exception as e:
                self.update_status("错误：无法启动提醒服务")
                messagebox.showerror("错误", f"无法执行server.exe：{str(e)}")
        
        threading.Thread(target=run_server, daemon=True).start()
    
    def view_schedule(self):
        """查看日程（启动HTTP服务器并打开浏览器）"""
        def start_http_server():
            try:
                self.update_status("正在启动Web服务器...")
                
                # 启动HTTP服务器
                server_thread = threading.Thread(target=self.run_http_server, daemon=True)
                server_thread.start()
                
                # 等待服务器启动
                import time
                time.sleep(2)
                
                # 打开浏览器
                webbrowser.open("http://localhost:8000")
                
                self.update_status("Web服务器已启动，浏览器已打开")
                messagebox.showinfo("成功", "Web服务器已启动！\n浏览器将自动打开http://localhost:8000")
                
            except Exception as e:
                self.update_status("错误：无法启动Web服务器")
                messagebox.showerror("错误", f"无法启动Web服务器：{str(e)}")
        
        threading.Thread(target=start_http_server, daemon=True).start()
    
    def run_http_server(self):
        """运行HTTP服务器"""
        try:
            # 使用python -m http.server 8000
            subprocess.run([sys.executable, "-m", "http.server", "8000"], 
                         check=True)
        except subprocess.CalledProcessError:
            self.update_status("HTTP服务器异常退出")
        except Exception as e:
            self.update_status(f"HTTP服务器错误：{str(e)}")
    
    def on_closing(self):
        """关闭程序时的处理"""
        if messagebox.askokcancel("退出", "确定要退出Planalyze吗？"):
            self.root.destroy()

def main():
    """主函数"""
    # 创建主窗口
    root = tk.Tk()
    
    # 创建GUI实例
    app = PlanalyzeGUI(root)
    
    # 设置关闭事件
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    
    # 启动主循环
    root.mainloop()

if __name__ == "__main__":
    main()