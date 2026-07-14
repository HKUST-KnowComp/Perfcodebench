<div align="center">

# ⚡ PerfCodeBench

**A large-scale executable benchmark for evaluating whether code-generation models can write code that is both correct and fast.**

[![Tasks](https://img.shields.io/badge/tasks-1%2C800%2B-6f42c1?style=flat-square)](#-benchmark-tasks)
[![Languages](https://img.shields.io/badge/languages-C%20%7C%20C%2B%2B%20%7C%20CUDA%20%7C%20Go%20%7C%20Python-0969da?style=flat-square)](#-benchmark-tasks)
[![Metric](https://img.shields.io/badge/metric-median%20elapsed__ns-2da44e?style=flat-square)](#-how-it-works)

### [🏆 Explore the Live Leaderboard](https://perfcodebench-explorer.huihao.chatgpt.site/)

</div>

PerfCodeBench asks a model to optimize an existing implementation under a fixed interface and dependency set. Every generated candidate is compiled with the same benchmark harness, checked for correctness, and measured against both a baseline and a reference implementation.

> [!TIP]
> New here? Start with one task and a small run count before launching a full model suite.

## ✨ Highlights

- **Executable evaluation** — candidates are compiled and run instead of judged only by text or static tests.
- **Correctness before speed** — benchmark harnesses validate output before reporting performance.
- **Direct comparisons** — each task measures the baseline, reference, and model-generated candidate under the same setup.
- **Multi-language coverage** — tasks span C, C++, CUDA, Go, and Python.
- **Reproducible artifacts** — generated source code, benchmark results, and build outputs are retained for inspection.
- **Batch evaluation** — run a single task, a selected subset, or every model declared in the configuration.
- **Interactive leaderboard** — explore and compare model results in the [PerfCodeBench Explorer](https://perfcodebench-explorer.huihao.chatgpt.site/).

## 🔄 How it works

```text
baseline implementation + task contract
                  │
                  ▼
          code-generation model
                  │
                  ▼
          generated candidate
                  │
                  ▼
       build → correctness check → benchmark
                  │
                  ▼
      baseline / reference / candidate results
```

The primary metric is the median `elapsed_ns` over repeated runs; lower is better. Individual tasks define their own correctness rules in `instance.json`.

## 🚀 Quick start

### 1. Install the Python dependency

```bash
python3 -m pip install openai
```

You also need the toolchain required by the tasks you plan to run—for example `gcc`/`g++`, `nvcc`, or Go.

### 2. Configure model access

Create a `.env` file in the repository root:

```dotenv
OPENAI_API_KEY=your_api_key
OPENAI_API_BASE=https://your-api-endpoint.example/v1
```

Model aliases and their environment-variable mappings live in [`configs.json`](configs.json). If your provider does not require a custom base URL, omit `OPENAI_API_BASE`.

### 3. Run an evaluation

Evaluate one task:

```bash
python3 scripts/run_openai_codegen_eval.py \
  fast_float_parse \
  --model gpt-5.4 \
  --runs 3
```

Run a small subset of the benchmark:

```bash
python3 scripts/run_model_suite.py \
  --model gpt-5.4 \
  --limit 10 \
  --runs 3
```

Run one or more models declared in `configs.json`:

```bash
python3 scripts/run_all_models_eval.py --models gpt-5.4 gpt-5.5
```

Use `python3 <script> --help` to see concurrency, timeout, task-selection, and output options.

## 🧩 Benchmark tasks

Each directory under `executable_tasks/<task_id>/` is self-contained:

```text
executable_tasks/<task_id>/
├── baseline/          # implementation given to the model
├── reference/         # optimized reference implementation
├── candidate/         # generated implementations, grouped by model
├── harness/           # build entrypoint and correctness checks
└── instance.json      # task metadata, build recipe, and run contract
```

The repository currently includes more than 1,800 task directories. Most executable solutions use one of the following extensions:

| Language | Extension | Typical toolchain |
| :-- | :--: | :-- |
| C | `.c` | GCC / Clang |
| C++ | `.cpp` | GCC / Clang |
| CUDA | `.cu` | NVIDIA CUDA Toolkit |
| Go | `.go` | Go toolchain |
| Python | `.py` | Python 3 |

See [`executable_tasks/README.md`](executable_tasks/README.md) for the task-level benchmark contract.

## 📁 Repository layout

```text
PerfCodeBench/
├── executable_tasks/             # benchmark task corpus
├── scripts/
│   ├── executable_benchmark_lib.py
│   ├── run_openai_codegen_eval.py
│   ├── run_model_suite.py
│   └── run_all_models_eval.py
├── configs.json                  # model aliases and API settings
└── README.md
```

## 📊 Outputs

| Artifact | Default location | Description |
| :-- | :-- | :-- |
| Evaluation result | `results/` | JSON containing model metadata and all benchmark outcomes |
| Candidate source | `executable_tasks/<task_id>/candidate/<model>/` | Complete generated replacement implementation |
| Build artifacts | `build/` | Compiled binaries and intermediate files |

A result records separate `baseline`, `reference`, and `candidate` sections. Each section includes a status and, when successful, timing data. Build failures, runtime errors, and timeouts are preserved as explicit outcomes rather than silently discarded.

## 🛠️ Useful options

```bash
# Preview the evaluation flow without calling a model
python3 scripts/run_openai_codegen_eval.py fast_float_parse --dry-run

# Benchmark an already generated candidate again
python3 scripts/run_openai_codegen_eval.py \
  fast_float_parse \
  --model gpt-5.4 \
  --reuse-candidate

# Evaluate only task IDs listed in a file
python3 scripts/run_model_suite.py \
  --model gpt-5.4 \
  --task-list path/to/tasks.txt
```

## ⚠️ Notes

- Performance results are hardware- and environment-dependent; compare variants on the same machine under similar load.
- CUDA tasks require a compatible NVIDIA GPU, driver, and CUDA toolkit.
- Some tasks use third-party headers or sources under `external/`.
- Full-suite evaluation can be resource-intensive. Tune the CPU/CUDA job counts and timeout options for your machine.

---

<div align="center">
  <sub>Correct code is the baseline. Fast, correct code is the benchmark.</sub>
</div>
