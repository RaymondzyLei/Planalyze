#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Planalyze GUI - User-friendly Schedule Management Tool (English Version)
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
        self.root.title("Planalyze Schedule Management System")
        self.root.geometry("450x400")
        self.root.resizable(False, False)
        
        # Set window icon (if available)
        try:
            self.root.iconbitmap("planalyze.ico")  # If icon file exists
        except:
            pass
        
        # Setup styles
        self.setup_styles()
        
        # Create interface
        self.create_widgets()
        
        # Check required files
        self.check_required_files()
    
    def setup_styles(self):
        """Setup interface styles"""
        style = ttk.Style()
        style.configure("TButton", font=("Arial", 12), padding=10)
        style.configure("Title.TLabel", font=("Arial", 16, "bold"))
        style.configure("Info.TLabel", font=("Arial", 10))
    
    def create_widgets(self):
        """Create interface components"""
        # Title
        title_label = ttk.Label(self.root, text="Planalyze Schedule Management System", 
                               style="Title.TLabel")
        title_label.pack(pady=20)
        
        # Description text
        info_label = ttk.Label(self.root, 
                              text="Welcome to Planalyze! Please select the function you need:",
                              style="Info.TLabel")
        info_label.pack(pady=10)
        
        # Button frame
        button_frame = ttk.Frame(self.root)
        button_frame.pack(pady=30, fill="both", expand=True)
        
        # Button 1: Schedule Operations
        self.btn_schedule = ttk.Button(button_frame, text="1. Schedule Operations",
                                      command=self.open_schedule,
                                      style="TButton")
        self.btn_schedule.pack(pady=10, fill="x", padx=50)
        
        # Button 2: Open Reminders
        self.btn_reminder = ttk.Button(button_frame, text="2. Open Reminders",
                                      command=self.open_reminder,
                                      style="TButton")
        self.btn_reminder.pack(pady=10, fill="x", padx=50)
        
        # Button 3: View Schedule
        self.btn_view = ttk.Button(button_frame, text="3. View Schedule",
                                  command=self.view_schedule,
                                  style="TButton")
        self.btn_view.pack(pady=10, fill="x", padx=50)
        
        # Status bar
        self.status_var = tk.StringVar()
        self.status_var.set("Ready")
        status_bar = ttk.Label(self.root, textvariable=self.status_var,
                              relief="sunken", anchor="w")
        status_bar.pack(side="bottom", fill="x")
    
    def check_required_files(self):
        """Check if required executable files exist"""
        missing_files = []
        
        if not os.path.exists("planalyze.exe"):
            missing_files.append("planalyze.exe")
        
        if not os.path.exists("server.exe"):
            missing_files.append("server.exe")
        
        if missing_files:
            messagebox.showwarning("Missing Files", 
                                  f"The following files are missing:\n{', '.join(missing_files)}\n\n"
                                  "Please ensure these files are in the current directory.")
    
    def update_status(self, message):
        """Update status bar"""
        self.status_var.set(message)
        self.root.update()
    
    def open_schedule(self):
        """Open schedule operations (execute planalyze.exe)"""
        def run_planalyze():
            try:
                self.update_status("Opening schedule operations...")
                
                # Get current working directory
                current_dir = os.getcwd()
                
                # Use start command on Windows to keep cmd window open
                if os.name == 'nt':  # Windows system
                    subprocess.Popen(f'start cmd /k "cd /d "{current_dir}" && planalyze.exe"', 
                                   shell=True)
                else:  # Other systems (Linux/Mac)
                    subprocess.Popen(['gnome-terminal', '--', 'bash', '-c', 
                                    f'cd "{current_dir}" && ./planalyze.exe; exec bash'])
                
                self.update_status("Schedule operations opened")
                
            except Exception as e:
                self.update_status("Error: Unable to open schedule operations")
                messagebox.showerror("Error", f"Unable to execute planalyze.exe: {str(e)}")
        
        # Run in new thread to avoid interface freezing
        threading.Thread(target=run_planalyze, daemon=True).start()
    
    def open_reminder(self):
        """Open reminders (run server.exe)"""
        def run_server():
            try:
                self.update_status("Starting reminder service...")
                
                # Run server.exe
                subprocess.Popen("server.exe", shell=True)
                
                self.update_status("Reminder service started")
                messagebox.showinfo("Success", "Reminder service started!")
                
            except Exception as e:
                self.update_status("Error: Unable to start reminder service")
                messagebox.showerror("Error", f"Unable to execute server.exe: {str(e)}")
        
        threading.Thread(target=run_server, daemon=True).start()
    
    def view_schedule(self):
        """View schedule (start HTTP server and open browser)"""
        def start_http_server():
            try:
                self.update_status("Starting web server...")
                
                # Start HTTP server
                server_thread = threading.Thread(target=self.run_http_server, daemon=True)
                server_thread.start()
                
                # Wait for server to start
                import time
                time.sleep(2)
                
                # Open browser
                webbrowser.open("http://localhost:8000/index-en.html")
                
                self.update_status("Web server started, browser opened")
                messagebox.showinfo("Success", "Web server started!\nBrowser will automatically open http://localhost:8000/index-en.html")
                
            except Exception as e:
                self.update_status("Error: Unable to start web server")
                messagebox.showerror("Error", f"Unable to start web server: {str(e)}")
        
        threading.Thread(target=start_http_server, daemon=True).start()
    
    def run_http_server(self):
        """Run HTTP server"""
        try:
            # Use python -m http.server 8000
            subprocess.run([sys.executable, "-m", "http.server", "8000"], 
                         check=True)
        except subprocess.CalledProcessError:
            self.update_status("HTTP server exited abnormally")
        except Exception as e:
            self.update_status(f"HTTP server error: {str(e)}")
    
    def on_closing(self):
        """Handle program closing"""
        if messagebox.askokcancel("Exit", "Are you sure you want to exit Planalyze?"):
            self.root.destroy()

def main():
    """Main function"""
    # Create main window
    root = tk.Tk()
    
    # Create GUI instance
    app = PlanalyzeGUI(root)
    
    # Set close event
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    
    # Start main loop
    root.mainloop()

if __name__ == "__main__":
    main()