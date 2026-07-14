```python
def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    for _ in range(iters):
        total = sum(bin(a & b).count('1') for a, b in zip(left, right))
    return total
```