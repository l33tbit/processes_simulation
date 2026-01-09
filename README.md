# CPU Scheduling Simulator

A comprehensive CPU scheduling simulator featuring a C-based core engine and a Python/Tkinter graphical interface. It visualizes process execution and compares the performance of various scheduling algorithms.

## Supported Algorithms
*   **FCFS** (First-Come, First-Served)
*   **SJF** (Shortest Job First)
*   **SRTF** (Shortest Remaining Time First)
*   **RR** (Round Robin)
*   **PPN** (Priority Non-Preemptive)
*   **PPP** (Priority Preemptive)

## Requirements
*   GCC Compiler
*   Python 3
*   `matplotlib` (`pip install matplotlib`)
*   `tkinter` (usually included with Python)

## Quick Start

### 1. Compile the Backend
The core logic is written in C and must be compiled first.

```bash
cd src/unit_testing
gcc -o unit_tester *.c ../implementation/*.c ../implementation/helpers/*.c -lm
cd ../..
```

### 2. Run the Interface
Launch the GUI to start simulations.

```bash
python3 src/python_ui/run_simulation.py
```

## Input Data Format
The simulator accepts CSV files with the following structure:
`Name, UserID, Priority, [Resources], NumInstructions, Memory, BurstTime, ArrivalTime`

**Example:**
`Proc1,user1,2,[AAA.BBB],2,1024,8.5,0.0`