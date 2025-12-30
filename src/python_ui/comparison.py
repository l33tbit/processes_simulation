#!/usr/bin/env python3
"""
Algorithm Comparison Module
Compares scheduling algorithms based on performance metrics
"""

import os
import re
from typing import Dict, List, Tuple, Optional

class AlgorithmComparison:
    """Compare scheduling algorithms based on performance metrics"""
    
    ALGORITHM_FILES = {
        "RR": "performance_summary_rr.txt",
        "FCFS": "performance_summary_fcfs.txt",
        "SRTF": "performance_summary_srtf.txt",
        "PPP": "performance_summary_ppp.txt",
        "PPN": "performance_summary_ppn.txt",
        "SJF": "performance_summary_sjf.txt"
    }
    
    ALGORITHM_NAMES = {
        "RR": "Round Robin",
        "FCFS": "First Come First Served",
        "SRTF": "Shortest Remaining Time First",
        "PPP": "Priority Preemptive",
        "PPN": "Priority Non-Preemptive",
        "SJF": "Shortest Job First"
    }
    
    def __init__(self, output_dir: str = "outputs"):
        """Initialize with output directory"""
        self.output_dir = output_dir
        self.data = {}
        self.missing_files = []
    
    def check_data_availability(self) -> Tuple[bool, List[str]]:
        """
        Check if all performance summary files exist and contain data
        Returns: (all_present, missing_files)
        """
        missing = []
        
        for algo_name, filename in self.ALGORITHM_FILES.items():
            filepath = os.path.join(self.output_dir, filename)
            
            if not os.path.exists(filepath):
                missing.append(f"{algo_name} ({filename})")
            else:
                # Check if file has content and contains performance data
                try:
                    with open(filepath, 'r') as f:
                        content = f.read().strip()
                        # Check if file has meaningful content (at least contains "Total Simulation Time" or metrics)
                        if not content or len(content) < 20 or "Total Simulation Time" not in content:
                            missing.append(f"{algo_name} ({filename} - empty or incomplete)")
                except Exception as e:
                    missing.append(f"{algo_name} ({filename} - error: {str(e)})")
        
        self.missing_files = missing
        return len(missing) == 0, missing
    
    def parse_performance_file(self, filepath: str) -> Optional[Dict[str, float]]:
        """
        Parse a performance summary file and extract metrics
        Returns dict with: avg_waiting_time, avg_turnaround_time, throughput, cpu_utilization, context_switches
        """
        try:
            with open(filepath, 'r') as f:
                content = f.read()
            
            metrics = {}
            
            # Extract Average Waiting Time
            wait_match = re.search(r'Average Waiting Time:\s*([\d.]+)', content)
            if wait_match:
                metrics['avg_waiting_time'] = float(wait_match.group(1))
            
            # Extract Average Turnaround Time
            turnaround_match = re.search(r'Average Turnaround Time:\s*([\d.]+)', content)
            if turnaround_match:
                metrics['avg_turnaround_time'] = float(turnaround_match.group(1))
            
            # Extract Throughput
            throughput_match = re.search(r'Throughput:\s*([\d.]+)', content)
            if throughput_match:
                metrics['throughput'] = float(throughput_match.group(1))
            
            # Extract CPU Utilization (handle percentage sign)
            cpu_match = re.search(r'CPU Utilization:\s*([\d.]+)', content)
            if cpu_match:
                metrics['cpu_utilization'] = float(cpu_match.group(1))
            
            # Extract Context Switches
            context_match = re.search(r'Context Switches:\s*(\d+)', content)
            if context_match:
                metrics['context_switches'] = int(context_match.group(1))
            
            # Extract Total Simulation Time
            time_match = re.search(r'Total Simulation Time:\s*([\d.]+)', content)
            if time_match:
                metrics['total_time'] = float(time_match.group(1))
            
            return metrics if len(metrics) >= 3 else None  # Need at least 3 metrics
            
        except Exception as e:
            print(f"Error parsing {filepath}: {e}")
            return None
    
    def load_all_data(self) -> bool:
        """
        Load data from all available performance summary files
        Returns: True if all data loaded successfully
        """
        self.data = {}
        
        for algo_name, filename in self.ALGORITHM_FILES.items():
            filepath = os.path.join(self.output_dir, filename)
            
            if os.path.exists(filepath):
                metrics = self.parse_performance_file(filepath)
                if metrics:
                    self.data[algo_name] = metrics
        
        return len(self.data) == 6
    
    def compare_algorithms(self) -> Dict[str, List[Tuple[str, float]]]:
        """
        Compare algorithms and rank them by:
        1. Average Waiting Time (lower is better)
        2. Average Turnaround Time (lower is better)
        3. Throughput (higher is better)
        
        Returns: Dict with rankings for each metric
        """
        if not self.data:
            return {}
        
        rankings = {
            'avg_waiting_time': [],
            'avg_turnaround_time': [],
            'throughput': []
        }
        
        # Rank by Average Waiting Time (ascending - lower is better)
        waiting_times = [(algo, self.data[algo].get('avg_waiting_time', float('inf'))) 
                         for algo in self.data.keys()]
        waiting_times.sort(key=lambda x: x[1])
        rankings['avg_waiting_time'] = waiting_times
        
        # Rank by Average Turnaround Time (ascending - lower is better)
        turnaround_times = [(algo, self.data[algo].get('avg_turnaround_time', float('inf'))) 
                           for algo in self.data.keys()]
        turnaround_times.sort(key=lambda x: x[1])
        rankings['avg_turnaround_time'] = turnaround_times
        
        # Rank by Throughput (descending - higher is better)
        throughputs = [(algo, self.data[algo].get('throughput', 0.0)) 
                      for algo in self.data.keys()]
        throughputs.sort(key=lambda x: x[1], reverse=True)
        rankings['throughput'] = throughputs
        
        return rankings
    
    def get_comparison_report(self) -> str:
        """
        Generate a formatted comparison report
        """
        all_present, missing = self.check_data_availability()
        
        if not all_present:
            report = "╔══════════════════════════════════════════════════════════════╗\n"
            report += "║              ALGORITHM COMPARISON                           ║\n"
            report += "╠══════════════════════════════════════════════════════════════╣\n\n"
            report += "⚠️  DATA MISSING\n\n"
            report += "The following algorithm data files are missing or empty:\n\n"
            
            for i, missing_item in enumerate(missing, 1):
                report += f"  {i}. {missing_item}\n"
            
            report += "\n"
            report += "Please run simulations for all algorithms first.\n"
            report += "\n"
            report += "Required files:\n"
            for algo, filename in self.ALGORITHM_FILES.items():
                status = "✓" if algo not in [m.split()[0] for m in missing] else "✗"
                report += f"  {status} {filename}\n"
            
            report += "\n╚══════════════════════════════════════════════════════════════╝\n"
            return report
        
        # Load data
        if not self.load_all_data():
            return "Error: Could not load all performance data."
        
        # Get rankings
        rankings = self.compare_algorithms()
        
        # Generate report
        report = "╔═══════════════════════════════════════════════════════════════════════════════════╗\n"
        report += "║                        ALGORITHM COMPARISON REPORT                                ║\n"
        report += "╠═══════════════════════════════════════════════════════════════════════════════════╣\n\n"
        
        # Ranking by Average Waiting Time
        report += "📊 RANKING BY AVERAGE WAITING TIME (Lower is Better)\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        report += f"{'Rank':<6} {'Algorithm':<25} {'Waiting Time':<15} {'Full Name'}\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        
        for rank, (algo, value) in enumerate(rankings['avg_waiting_time'], 1):
            full_name = self.ALGORITHM_NAMES.get(algo, algo)
            medal = "🥇" if rank == 1 else "🥈" if rank == 2 else "🥉" if rank == 3 else "  "
            report += f"{medal} {rank:<4} {algo:<25} {value:>12.2f}     {full_name}\n"
        
        report += "\n\n"
        
        # Ranking by Average Turnaround Time
        report += "📊 RANKING BY AVERAGE TURNAROUND TIME (Lower is Better)\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        report += f"{'Rank':<6} {'Algorithm':<25} {'Turnaround Time':<15} {'Full Name'}\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        
        for rank, (algo, value) in enumerate(rankings['avg_turnaround_time'], 1):
            full_name = self.ALGORITHM_NAMES.get(algo, algo)
            medal = "🥇" if rank == 1 else "🥈" if rank == 2 else "🥉" if rank == 3 else "  "
            report += f"{medal} {rank:<4} {algo:<25} {value:>12.2f}     {full_name}\n"
        
        report += "\n\n"
        
        # Ranking by Throughput
        report += "📊 RANKING BY THROUGHPUT (Higher is Better)\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        report += f"{'Rank':<6} {'Algorithm':<25} {'Throughput':<15} {'Full Name'}\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        
        for rank, (algo, value) in enumerate(rankings['throughput'], 1):
            full_name = self.ALGORITHM_NAMES.get(algo, algo)
            medal = "🥇" if rank == 1 else "🥈" if rank == 2 else "🥉" if rank == 3 else "  "
            report += f"{medal} {rank:<4} {algo:<25} {value:>12.4f}     {full_name}\n"
        
        report += "\n\n"
        
        # Summary table
        report += "📋 SUMMARY TABLE\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        report += f"{'Algorithm':<12} {'Waiting':<12} {'Turnaround':<12} {'Throughput':<12} {'CPU Util':<12} {'Context Sw':<12}\n"
        report += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        
        for algo in sorted(self.data.keys()):
            metrics = self.data[algo]
            report += f"{algo:<12} "
            report += f"{metrics.get('avg_waiting_time', 0):>10.2f}   "
            report += f"{metrics.get('avg_turnaround_time', 0):>10.2f}   "
            report += f"{metrics.get('throughput', 0):>10.4f}   "
            report += f"{metrics.get('cpu_utilization', 0):>10.2f}%  "
            report += f"{metrics.get('context_switches', 0):>10}   "
            report += "\n"
        
        report += "\n╚═══════════════════════════════════════════════════════════════════════════════════╝\n"
        
        return report

