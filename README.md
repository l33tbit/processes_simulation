# CPU Scheduling Simulator

This project is a comprehensive simulator for various CPU scheduling algorithms. It features a core simulation engine written in C and a user-friendly graphical interface built with Python and Tkinter for running simulations and visualizing results.

## Features

- **Multiple Scheduling Algorithms:** Simulates a variety of common scheduling algorithms.
- **Graphical User Interface:** An intuitive GUI to control simulations and view outputs.
- **Detailed Performance Metrics:** Calculates and displays key performance indicators like average waiting time, turnaround time, CPU utilization, and more.
- **Gantt Chart Visualization:** Generates a clear and colorful Gantt chart for each simulation to visualize process execution over time.
- **Algorithm Comparison:** Provides a side-by-side comparison report to rank algorithms based on their performance for a given workload.

## Algorithms Implemented

The simulator supports the following CPU scheduling algorithms:
- **FCFS (First-Come, First-Served):** Processes are executed in the order they arrive.
- **SJF (Shortest Job First):** A non-preemptive algorithm where the process with the smallest burst time is executed next.
- **SRTF (Shortest Remaining Time First):** The preemptive version of SJF. The process with the least remaining time is executed.
- **RR (Round Robin):** A preemptive algorithm where each process gets a small unit of CPU time (quantum).
- **PPN (Priority Non-Preemptive):** A non-preemptive algorithm where the process with the highest priority is executed next.
- **PPP (Priority Preemptive):** The preemptive version of the priority algorithm.

## Getting Started

Follow these instructions to compile and run the simulator on your local machine.

### Prerequisites

- **GCC Compiler:** Required to compile the C simulation engine.
- **Python 3:** Required to run the GUI.
- **Tkinter:** The standard Python interface to the Tcl/Tk GUI toolkit. (Usually included with Python).
- **Matplotlib:** A Python library for creating static, animated, and interactive visualizations.

You can install Matplotlib using pip:
```sh
pip install matplotlib
```

### 1. Compilation

The core logic is in C and must be compiled into an executable. The Python UI depends on this executable.

Navigate to the `src/unit_testing` directory and run the following `gcc` command. The `-lm` flag is necessary to link the math library.

```sh
cd src/unit_testing
gcc -o unit_tester *.c ../implementation/*.c ../implementation/helpers/*.c -lm
```

This command creates an executable file named `unit_tester` inside the `src/unit_testing` directory. The Python UI is pre-configured to use this specific path.

### 2. Running the Simulator

Once the C code is compiled, you can launch the graphical user interface from the project's root directory.

```sh
python3 src/python_ui/run_simulation.py
```

### 3. Using the UI

1.  **Select Algorithm:** Choose the desired scheduling algorithm from the dropdown menu.
2.  **Set Quantum (for Round Robin):** If you select "RR", an input box will appear to specify the time quantum.
3.  **Verify Input File:** The path to the input data file (`data_testing.csv`) is pre-filled.
4.  **Run Simulation:** Click the **▶ RUN SIMULATION** button to execute the C backend with the selected parameters.
5.  **View Results:** The results will be displayed across several tabs:
    - **📊 Gantt Chart:** A visual timeline of process execution.
    - **📈 Performance Summary:** Key metrics for the simulation run.
    - **📋 Process Details:** A table with detailed statistics for each individual process.
6.  **Compare Algorithms:** After running simulations for multiple algorithms, click the **📊 COMPARE ALGORITHMS** button to see a ranked comparison report.

## Input File Format

The simulator uses a `.csv` file to load process data. Each line in the file represents a single process and must contain 8 comma-separated fields in the following order:

`process_name,user_id,priority,instructions,n_instruction,memoire,burst,temps_arrive`

**Field Descriptions:**

- `process_name` (String, max 20 chars): The name of the process (e.g., `P1`).
- `user_id` (String, max 20 chars): The user ID associated with the process.
- `priority` (Integer, 1-5): The priority of the process (lower number means higher priority).
- `instructions` (String): A string representing resource requests, formatted as `[AAA.BBB.CCC]`.
- `n_instruction` (Integer): The total number of instructions/resource requests. Must match the count in the `instructions` string.
- `memoire` (Integer): The memory required by the process.
- `burst` (Float): The total CPU burst time required for the process.
- `temps_arrive` (Float): The arrival time of the process.

**Example CSV line:**
```csv
Proc1,user1,2,[AAA.BBB],2,1024,8.5,0.0
```

## Project Structure

```
├── src
│   ├── implementation      # Core C implementation of the simulator components
│   │   ├── helpers         # C helpers, including the CSV parser
│   │   └── ...
│   ├── python_ui           # Python source for the Tkinter GUI
│   │   ├── run_simulation.py # Main UI application
│   │   └── comparison.py   # Algorithm comparison logic
│   └── unit_testing        # C entry point and data files
│       ├── unit_tester.c   # Main function for the C executable
│       └── data_testing.csv# Default input data for the simulation
├── lib
│   └── structs             # C header files defining all data structures
└── outputs/                # Directory where simulation results are saved (auto-generated)
```

## Code Overview

### C Backend

The C code forms the heart of the simulator.
- **`process_manager`**: Handles the creation and management of Process Control Blocks (PCBs), including parsing the input CSV and managing the ready and blocked queues.
- **`schedular`**: Implements the logic for each scheduling algorithm, deciding which process to run next.
- **`simulator`**: Orchestrates the entire simulation, managing time, and coordinating between the process manager, resource manager, and scheduler.
- **`unit_tester.c`**: The main entry point that receives command-line arguments (algorithm, quantum, file path) from the Python UI and initiates the simulation.

### Python Frontend

The Python code provides a user-friendly interface to the powerful C backend.
- **`run_simulation.py`**: Uses `tkinter` to create the GUI. It gathers user input and uses the `subprocess` module to call the compiled C executable (`unit_tester`) with the appropriate arguments. It then reads the text files generated by the C program to populate the results tabs.
- **`comparison.py`**: Reads the multiple `performance_summary_*.txt` files from the `outputs` directory, parses the key metrics, and generates a formatted text report that ranks the algorithms.