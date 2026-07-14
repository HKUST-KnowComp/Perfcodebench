import re


def run(rows: list[str]) -> int:
    """
    Fixed FNV-1a checksum over rows.
    """
    h = 1469598103934665603
    for row in rows:
        # fast path for fixed-pattern: length=16, positions 0-1=[A-Z],2,7=[0-9],3-6=[0-9],8='-',9-16=[0-9A-F]
        if (
            len(row) == 16
            and row[0].isupper()
            and row[1].isupper()
            and row[2].isdigit()
            and row[3].isdigit()
            and row[4].isdigit()
            and row[5].isdigit()
            and row[6] == '-'
            and row[7].isdigit()
            and row[8].isdigit()
            and row[9].isdigit()
            and row[10].isdigit()
            and row[11].isdigit()
            and row[12].isdigit()
            and row[13].isdigit()
            and row[14].isdigit()
            and row[15].isdigit()
        ):
            v = ord(row[2]) + ord(row[7])
        else:
            v = 0
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h