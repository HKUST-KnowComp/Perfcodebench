# Executable Tasks

These are the first real PerfCodeBench-style executable tasks.

Each task contains:
- `baseline/solution.cpp` or `baseline/solution.cu`
- `reference/solution.cpp` or `reference/solution.cu`
- `candidate/solution.cpp` or `candidate/solution.cu`
- `harness/interface.h`
- `harness/benchmark_main.cpp` or `harness/benchmark_main.cu`
- `instance.json`

The benchmark contract is:
- compile each variant against the same harness
- run correctness checks inside the benchmark binary
- report `elapsed_ns`
- compare baseline, reference, and candidate speed directly

Helpers:
- `python3 scripts/run_executable_benchmark.py <task_id> baseline`
- `python3 scripts/run_executable_benchmark.py <task_id> reference`
- `python3 scripts/run_openai_codegen_eval.py <task_id>`
