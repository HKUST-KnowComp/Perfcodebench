def run(deltas: list[int], iters: int) -> int:
    """
    Optimized delta decoding and prefix sum calculation.
    The original implementation performed two passes and allocated a list per iteration.
    This version fuses the accumulation and the constant offset addition into a single pass.
    """
    n = len(deltas)
    offset = 0x9E3779B1
    total = 0
    
    for _ in range(iters):
        current = 0
        iteration_sum = 0
        # Fusing the prefix sum calculation and the constant offset addition
        # avoids the need for an intermediate 'values' list.
        for d in deltas:
            current += d
            iteration_sum += current + offset
        total = iteration_sum
        
    return total