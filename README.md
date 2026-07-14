# PerfCodeBench

A repository for evaluating code generation models on performance optimization tasks.

The repository includes:
- executable tasks in `executable_tasks/`
- evaluation scripts in `scripts/`
- model config in `configs.json`
- generated candidates and evaluation results

## Structure

Each `executable_tasks/<task_id>/` directory usually contains:
- `baseline/`: the initial implementation to optimize
- `reference/`: a reference implementation
- `candidate/`: model-generated implementations
- `harness/`: benchmark code used for build and execution
- `instance.json`: task description, build config, and run config

## Requirements

- Python 3
- the `openai` Python package
- task-specific toolchains such as `g++`, `nvcc`, `java`, or `go`
- API keys configured in `.env`

Minimal install:

```bash
pip install openai
```

## Usage

Run a single task:

```bash
python3 scripts/run_openai_codegen_eval.py fast_float_parse --model gpt-5.4 --runs 3
```

Run a subset of tasks:

```bash
python3 scripts/run_model_suite.py --model gpt-5.4 --limit 10 --runs 3
```

Run models defined in `configs.json`:

```bash
python3 scripts/run_all_models_eval.py --models gpt-5.4
```

## Outputs

- evaluation results are written to `results/`
- candidate code is written to each task's `candidate/<model>/`
- build artifacts are written to `build/`

## Notes

- tasks use different languages and toolchains
- some tasks depend on third-party code under `external/`
- for a quick sanity check, start with a single task
