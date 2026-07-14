import json

def run(rows: list[str]) -> int:
    # FNV-1a constants
    HASH_INIT = 1469598103934665603
    MULTIPLIER = 1099511628211
    MASK = (1 << 64) - 1

    class _FieldExtractor:
        __slots__ = ('h',)
        def __init__(self):
            self.h = HASH_INIT

        def __call__(self, pairs):
            u = 0
            s = 0
            f = False
            # Process key-value pairs as they appear in the JSON object
            for k, v in pairs:
                if k == 'u':
                    u = v
                elif k == 's':
                    s = v
                elif k == 'f':
                    f = v
            v = (u & 1023) + s + (17 if f else 0)
            h = self.h
            h ^= v + 1
            h *= MULTIPLIER
            h &= MASK
            self.h = h
            return {}  # Return value is ignored

    extractor = _FieldExtractor()
    for row in rows:
        json.loads(row, object_pairs_hook=extractor)
    return extractor.h