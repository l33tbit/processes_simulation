#!/usr/bin/env python3
"""
CPU Scheduler Simulator UI
Parses output files in your exact format
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import subprocess
import os
import re
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib
matplotlib.use('Agg')

class SchedulerUI:
    def __init__(self, root):
        self.root = root
        self.root.title("CPU Scheduler Simulator")
        self.root.geometry("1400x900")

        self.c_executable = "/home/zeus/projects/processus_simulation/src/unit_testing/unit_tester"

        # Store chart objects
        self.fig = None
        self.canvas = None
        
        # Setup UI
        self.setup_ui()
        
        # Check if C executable exists
        self.check_c_executable()
    
    def setup_ui(self):
        """Create the user interface"""
        # Configure style
        style = ttk.Style()
        style.configure("Run.TButton", font=('Arial', 11, 'bold'), foreground='white', background='#0078D7')
        
        # Main container
        main_container = ttk.Frame(self.root, padding="10")
        main_container.pack(fill=tk.BOTH, expand=True)
        
        # Left panel - Controls
        control_frame = ttk.LabelFrame(main_container, text="Simulation Controls", padding="20", width=300)
        control_frame.pack(side=tk.LEFT, fill=tk.Y, padx=(0, 10))
        control_frame.pack_propagate(False)
        
        # Title
        ttk.Label(control_frame, text="CPU Scheduler", font=('Arial', 16, 'bold')).pack(pady=(0, 20))
        
        # Algorithm selection
        ttk.Label(control_frame, text="Select Algorithm:", font=('Arial', 11)).pack(anchor=tk.W, pady=(0, 5))
        self.algo_var = tk.StringVar(value="RR")
        algorithms = ["FCFS", "SJF", "RR", "SRTF", "PPP", "PPN"]
        algo_combo = ttk.Combobox(control_frame, textvariable=self.algo_var, 
                                 values=algorithms, state="readonly", width=15)
        algo_combo.pack(fill=tk.X, pady=(0, 15))
        
        # Quantum input (only for RR)
        self.quantum_frame = ttk.Frame(control_frame)
        ttk.Label(self.quantum_frame, text="Quantum (RR only):").pack(side=tk.LEFT)
        self.quantum_var = tk.StringVar(value="2")
        quantum_entry = ttk.Entry(self.quantum_frame, textvariable=self.quantum_var, width=8)
        quantum_entry.pack(side=tk.RIGHT, padx=(10, 0))
        
        # Initially hide quantum
        self.algo_var.trace('w', self.toggle_quantum)
        self.toggle_quantum()
        
        # Input file selection
        ttk.Label(control_frame, text="Input CSV:", font=('Arial', 11)).pack(anchor=tk.W, pady=(20, 5))
        self.file_var = tk.StringVar(value="/home/zeus/projects/processus_simulation/src/unit_testing/data_testing.csv")
        ttk.Entry(control_frame, textvariable=self.file_var).pack(fill=tk.X, pady=(0, 15))
        
        # Run button
        run_btn = ttk.Button(control_frame, text="▶ RUN SIMULATION", 
                           command=self.run_simulation, style="Run.TButton")
        run_btn.pack(pady=30, fill=tk.X)
        
        # Status
        self.status_var = tk.StringVar(value="Ready")
        status_label = ttk.Label(control_frame, textvariable=self.status_var, 
                               relief=tk.SUNKEN, padding=5)
        status_label.pack(side=tk.BOTTOM, fill=tk.X)
        
        # Right panel - Results
        results_frame = ttk.Frame(main_container)
        results_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
        
        # Create notebook for tabs
        self.notebook = ttk.Notebook(results_frame)
        self.notebook.pack(fill=tk.BOTH, expand=True)
        
        # Tab 1: Gantt Chart
        self.gantt_frame = ttk.Frame(self.notebook)
        self.notebook.add(self.gantt_frame, text="📊 Gantt Chart")
        
        # Tab 2: Performance Summary
        perf_frame = ttk.Frame(self.notebook)
        self.notebook.add(perf_frame, text="📈 Performance Summary")
        
        # Performance text
        self.perf_text = scrolledtext.ScrolledText(perf_frame, wrap=tk.WORD, 
                                                  font=('Courier', 10))
        self.perf_text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Tab 3: Process Details
        details_frame = ttk.Frame(self.notebook)
        self.notebook.add(details_frame, text="📋 Process Details")
        
        # Details text
        self.details_text = scrolledtext.ScrolledText(details_frame, wrap=tk.WORD,
                                                     font=('Courier', 9))
        self.details_text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
    
    def toggle_quantum(self, *args):
        """Show/hide quantum input based on algorithm"""
        if self.algo_var.get() == "RR":
            self.quantum_frame.pack(fill=tk.X, pady=(0, 15))
        else:
            self.quantum_frame.pack_forget()
    
    def check_c_executable(self):
        """Check if C executable exists"""
        if not os.path.exists(self.c_executable):
            self.status_var.set("⚠️ C executable not found")
    
    def run_simulation(self):
        """Run the C simulation and parse output"""
        self.status_var.set("Running simulation...")
        self.root.update()
        
        try:
            # Map algorithm name to C input number
            algo_map = {
                "RR": "0", "SRTF": "1", "PPP": "2", 
                "PPN": "3", "FCFS": "4", "SJF": "5"
            }
            algo_num = algo_map.get(self.algo_var.get(), "4")
            
            # Prepare arguments for C program
            args = [self.c_executable, algo_num, self.quantum_var.get(), self.file_var.get()]
            
            # Run C program
            result = subprocess.run(
                args,
                timeout=60
            )
            
            if result.returncode != 0:
                messagebox.showerror("Error", "Simulation failed")
                self.status_var.set("❌ Simulation failed")
                return
            
            # Parse and display results
            self.display_results()
            
            self.status_var.set("✅ Simulation completed")
            
        except subprocess.TimeoutExpired:
            messagebox.showerror("Error", "Simulation timed out after 60 seconds")
            self.status_var.set("❌ Timeout")
        except FileNotFoundError:
            messagebox.showerror("Error", 
                "C executable not found.\nPlease build it first:\n\n"
                "cd src/unit_testing && gcc -o unit_tester *.c ../implementation/*.c ../implementation/helpers/*.c")
            self.status_var.set("❌ Executable not found")
        except Exception as e:
            messagebox.showerror("Error", f"Simulation failed: {str(e)}")
            self.status_var.set("❌ Simulation failed")
    
    def save_output_files(self, output):
        """Parse C output and save to separate files"""
        lines = output.split('\n')
        
        gantt_lines = []
        perf_lines = []
        detail_lines = []
        current_section = None
        
        # Parse the output into sections
        for line in lines:
            line = line.strip()
            
            # Detect section changes
            if line.startswith("Time ") and ("[" in line or "P" in line):
                current_section = "gantt"
                gantt_lines.append(line)
            elif line.startswith("Total Simulation Time:") or line.startswith("CPU Utilization:"):
                current_section = "perf"
                perf_lines.append(line)
            elif line.startswith("PID ") or ("Arrival=" in line and "Burst=" in line):
                current_section = "detail"
                detail_lines.append(line)
            elif current_section == "gantt" and line:
                gantt_lines.append(line)
            elif current_section == "perf" and line:
                perf_lines.append(line)
            elif current_section == "detail" and line and not line.startswith("==="):
                detail_lines.append(line)
        
        # Ensure output directory exists
        os.makedirs("outputs", exist_ok=True)
        
        # Save to files
        with open("outputs/gantt_chart.txt", "w") as f:
            f.write("\n".join(gantt_lines))
        
        with open("outputs/performance_summary.txt", "w") as f:
            f.write("\n".join(perf_lines))
        
        with open("outputs/process_details.txt", "w") as f:
            f.write("\n".join(detail_lines))
    
    def display_results(self):
        """Display all results in UI"""
        try:
            # Display Gantt chart
            self.display_gantt_chart()
            
            # Display Performance Summary
            self.display_performance_summary()
            
            # Display Process Details
            self.display_process_details()
            
            # Switch to Gantt chart tab
            self.notebook.select(0)
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to display results: {str(e)}")
    
    def display_gantt_chart(self):
        """Display Gantt chart from output file"""
        try:
            # Clear previous chart
            if self.canvas:
                self.canvas.get_tk_widget().destroy()
            
            # Read Gantt data
            gantt_data = []
            with open("outputs/gantt_chart.txt", "r") as f:
                for line in f:
                    line = line.strip()
                    if not line:
                        continue
                    
                    # Parse lines like: "Time 0.00-3.00: [P1] (completed)"
                    match = re.search(r'Time\s+([\d.]+)-([\d.]+):\s+\[(P?\d+)\]', line)
                    if match:
                        start, end, process = match.groups()
                        if not process.startswith('P'):
                            process = f"P{process}"
                        gantt_data.append({
                            'process': process,
                            'start_time': float(start),
                            'end_time': float(end),
                            'duration': float(end) - float(start)
                        })
            
            if not gantt_data:
                # Create placeholder if no data
                label = ttk.Label(self.gantt_frame, text="No Gantt chart data available", 
                                font=('Arial', 12))
                label.pack(expand=True)
                return
            
            # Create figure
            self.fig, ax = plt.subplots(figsize=(14, 8))
            
            # Get unique processes and sort them
            processes = sorted(set(entry['process'] for entry in gantt_data),
                             key=lambda x: int(re.search(r'\d+', x).group()) if re.search(r'\d+', x) else x)
            
            # Create color map
            colors = plt.cm.tab20c(range(len(processes)))
            color_map = {proc: colors[i] for i, proc in enumerate(processes)}
            
            # Create y positions
            y_positions = {proc: i for i, proc in enumerate(processes)}
            
            # Plot each bar
            for entry in gantt_data:
                process = entry['process']
                start = entry['start_time']
                duration = entry['duration']
                
                ax.barh(y=y_positions[process],
                        width=duration,
                        left=start,
                        color=color_map[process],
                        edgecolor='black',
                        height=0.7,
                        alpha=0.8)
                
                # Add process label in middle of bar
                mid_point = start + duration / 2
                ax.text(mid_point, y_positions[process], process,
                       ha='center', va='center',
                       color='white', fontweight='bold', fontsize=9)
            
            # Customize chart
            ax.set_xlabel('Time', fontsize=12, fontweight='bold')
            ax.set_ylabel('Processes', fontsize=12, fontweight='bold')
            ax.set_title(f'CPU Scheduling Gantt Chart - {self.algo_var.get()}',
                        fontsize=14, fontweight='bold', pad=20)
            
            # Set y ticks
            ax.set_yticks(range(len(processes)))
            ax.set_yticklabels(processes)
            
            # Add grid
            ax.grid(True, axis='x', alpha=0.3, linestyle='--')
            ax.set_axisbelow(True)
            
            # Set x limits
            max_time = max(entry['end_time'] for entry in gantt_data)
            ax.set_xlim(0, max_time * 1.05)
            
            # Add timeline markers
            time_interval = max(1, int(max_time / 20))
            ax.set_xticks(range(0, int(max_time) + 1, time_interval))
            
            plt.tight_layout()
            
            # Embed in Tkinter
            self.canvas = FigureCanvasTkAgg(self.fig, master=self.gantt_frame)
            self.canvas.draw()
            self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            
            # Add legend
            legend_elements = [plt.Rectangle((0, 0), 1, 1, color=color_map[proc], label=proc)
                             for proc in processes[:10]]  # Show first 10 processes
            if len(processes) > 10:
                legend_elements.append(plt.Rectangle((0, 0), 1, 1, color='gray', 
                                                    label=f'+ {len(processes)-10} more'))
            ax.legend(handles=legend_elements, loc='upper right', bbox_to_anchor=(1.15, 1))
            
        except Exception as e:
            print(f"Error displaying Gantt chart: {e}")
            label = ttk.Label(self.gantt_frame, 
                            text=f"Error loading Gantt chart:\n{str(e)}",
                            font=('Arial', 10), foreground='red')
            label.pack(expand=True)
    
    def display_performance_summary(self):
        """Display performance summary"""
        try:
            with open("outputs/performance_summary.txt", "r") as f:
                content = f.read()
            
            # Format with box drawing characters
            formatted = "╔══════════════════════════════════════════════╗\n"
            formatted += "║          PERFORMANCE SUMMARY                 ║\n"
            formatted += "╠══════════════════════════════════════════════╣\n\n"
            
            # Parse and format each metric
            lines = content.split('\n')
            for line in lines:
                if ':' in line:
                    # Clean up the line
                    line = line.replace('//', '').strip()
                    if line:
                        formatted += f"• {line}\n"
            
            formatted += "\n╚══════════════════════════════════════════════╝"
            
            self.perf_text.delete(1.0, tk.END)
            self.perf_text.insert(1.0, formatted)
            
            # Highlight important metrics
            self.perf_text.tag_configure("highlight", foreground="#0078D7", font=('Courier', 10, 'bold'))
            
            # Apply highlights
            for line in lines:
                if 'Total Simulation Time:' in line or 'CPU Utilization:' in line or 'Average Turnaround Time:' in line:
                    start_idx = self.perf_text.search(line, 1.0, tk.END)
                    if start_idx:
                        end_idx = f"{start_idx}+{len(line)}c"
                        self.perf_text.tag_add("highlight", start_idx, end_idx)
                        
        except Exception as e:
            self.perf_text.delete(1.0, tk.END)
            self.perf_text.insert(1.0, f"Error loading performance summary:\n{str(e)}")
    
    def display_process_details(self):
        """Display process details in formatted table"""
        try:
            with open("outputs/process_details.txt", "r") as f:
                content = f.read()
            
            # Split into lines
            lines = [line.strip() for line in content.split('\n') if line.strip()]
            
            if not lines:
                self.details_text.delete(1.0, tk.END)
                self.details_text.insert(1.0, "No process details available")
                return
            
            # Create formatted table header
            formatted = "╔═══════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n"
            formatted += "║                                          PROCESS DETAILS                                                   ║\n"
            formatted += "╠══════╦══════════╦══════════╦══════════╦══════════╦══════════╦══════════╦══════════════════════════╣\n"
            formatted += "║ PID  ║ Arrival  ║ Burst    ║ Complete ║ Wait     ║ Turnaround║ Response ║ Status                  ║\n"
            formatted += "╠══════╬══════════╬══════════╬══════════╬══════════╬══════════╬══════════╬══════════════════════════╣\n"
            
            # Parse each process line
            for line in lines:
                if line.startswith("PID"):
                    # Extract values using regex
                    pid = re.search(r'PID\s+(\d+)', line)
                    arrival = re.search(r'Arrival=([\d.]+)', line)
                    burst = re.search(r'Burst=([\d.]+)', line)
                    completion = re.search(r'Completion=([\d.]+)', line)
                    wait = re.search(r'Wait=([\d.]+)', line)
                    turnaround = re.search(r'Turnaround=([\d.]+)', line)
                    
                    if pid and arrival and burst and completion:
                        pid_val = pid.group(1)
                        arrival_val = float(arrival.group(1))
                        burst_val = float(burst.group(1))
                        completion_val = float(completion.group(1))
                        wait_val = float(wait.group(1)) if wait else 0.0
                        turnaround_val = float(turnaround.group(1)) if turnaround else 0.0
                        
                        # Calculate response time (first time CPU gets process)
                        response_val = wait_val  # Simplified
                        
                        # Determine status
                        status = "✅ Completed" if "(completed)" in line or completion_val > 0 else "⏸️ Preempted"
                        
                        # Format row
                        row = f"║ {pid_val:4} ║ {arrival_val:8.2f} ║ {burst_val:8.2f} ║ {completion_val:8.2f} ║ "
                        row += f"{wait_val:8.2f} ║ {turnaround_val:9.2f} ║ {response_val:8.2f} ║ {status:24} ║\n"
                        formatted += row
            
            formatted += "╚══════╩══════════╩══════════╩══════════╩══════════╩══════════╩══════════╩══════════════════════════╝\n\n"
            
            # Add summary
            formatted += f"Total Processes: {len(lines)}\n"
            formatted += f"Algorithm: {self.algo_var.get()}\n"
            
            self.details_text.delete(1.0, tk.END)
            self.details_text.insert(1.0, formatted)
            
            # Color code rows based on wait time
            self.details_text.tag_configure("low_wait", foreground="green")
            self.details_text.tag_configure("high_wait", foreground="red")
            
            # Apply color coding
            for line in lines:
                if 'Wait=' in line:
                    wait_match = re.search(r'Wait=([\d.]+)', line)
                    if wait_match:
                        wait_time = float(wait_match.group(1))
                        if wait_time < 10:
                            # Find and tag this line in the text widget
                            search_line = f"PID {re.search(r'PID\s+(\d+)', line).group(1)}:"
                            start_idx = self.details_text.search(search_line, 1.0, tk.END)
                            if start_idx:
                                end_idx = f"{start_idx} lineend"
                                self.details_text.tag_add("low_wait", start_idx, end_idx)
                        elif wait_time > 100:
                            start_idx = self.details_text.search(search_line, 1.0, tk.END)
                            if start_idx:
                                end_idx = f"{start_idx} lineend"
                                self.details_text.tag_add("high_wait", start_idx, end_idx)
                                
        except Exception as e:
            self.details_text.delete(1.0, tk.END)
            self.details_text.insert(1.0, f"Error loading process details:\n{str(e)}\n\nRaw content:\n{content}")

def main():
    """Main entry point"""
    root = tk.Tk()
    app = SchedulerUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()