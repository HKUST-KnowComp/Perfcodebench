import json

class _Hook:
    __slots__ = ('v',)
    def __call__(self, pairs):
        u_val = None
        s_val = None
        f_val = None
        for k, v in pairs:
            if k == 'u':
                u_val = v
            elif k == 's':
                s_val = v
            elif k == 'f':
                f_val = v
        self.v = (u_val & 1023) + s_val + (17 if f_val else 0)
        return {}

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    MASK = (1 << 64) - 1
    MULT = 1099511628211
    hook = _Hook()
    decoder = json.JSONDecoder(object_pairs_hook=hook)
    for row in rows:
        decoder.decode(row)
        v = hook.v
        h ^= v + 1
        h = (h * MULT) & MASK
    return h