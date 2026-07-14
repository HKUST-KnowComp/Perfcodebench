import operator

def run(left: list[int], right: list[int], iters: int) -> int:
    total = 0
    _sum = sum
    _map = map
    _bit_count = int.bit_count
    _and = operator.and_
    
    for _ in range(iters):
        total = _sum(_map(_bit_count, _map(_and, left, right)))
        
    return total
