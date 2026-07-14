import json


def run(rows: list[str]) -> int:
    mask = (1 << 64) - 1
    h = 1469598103934665603

    ws = set(" \t\r\n")

    for row in rows:
        s = row
        n = len(s)
        u_ok = s_ok = f_ok = True
        u_val = 0
        s_val = 0
        f_val = False

        # Helper to skip whitespace
        def skip_ws(i: int) -> int:
            while i < n and s[i] in ws:
                i += 1
            return i

        # Parse integer starting at index i. Returns (value, next_index) or (None, i) on failure.
        def parse_int_at(i: int):
            i = skip_ws(i)
            if i >= n:
                return None, i
            neg = False
            ch = s[i]
            if ch == '-':
                neg = True
                i += 1
                if i >= n:
                    return None, i
                ch = s[i]
            # JSON numbers do not allow leading '+'
            if ch < '0' or ch > '9':
                return None, i
            val = 0
            while i < n:
                ch = s[i]
                if '0' <= ch <= '9':
                    val = val * 10 + (ord(ch) - 48)
                    i += 1
                else:
                    break
            if neg:
                val = -val
            return val, i

        # Parse boolean starting at index i. Returns (value, next_index) or (None, i) on failure.
        def parse_bool_at(i: int):
            i = skip_ws(i)
            if i >= n:
                return None, i
            ch = s[i]
            # Expect true or false
            if ch == 't':
                if i + 4 <= n and s[i:i+4] == 'true':
                    return True, i + 4
                return None, i
            elif ch == 'f':
                if i + 5 <= n and s[i:i+5] == 'false':
                    return False, i + 5
                return None, i
            else:
                return None, i

        # Utility to locate the colon after a key occurrence starting at key_end
        def find_colon_after(key_end: int) -> int:
            return s.find(':', key_end)

        # Parse field by key name using last occurrence semantics
        # 'u'
        kp = s.rfind('"u"')
        if kp != -1:
            cp = find_colon_after(kp + 3)
            if cp != -1:
                val, _ = parse_int_at(cp + 1)
                if val is not None:
                    u_val = val
                else:
                    u_ok = False
            else:
                u_ok = False
        else:
            u_ok = False

        # 's'
        kp = s.rfind('"s"')
        if kp != -1:
            cp = find_colon_after(kp + 3)
            if cp != -1:
                val, _ = parse_int_at(cp + 1)
                if val is not None:
                    s_val = val
                else:
                    s_ok = False
            else:
                s_ok = False
        else:
            s_ok = False

        # 'f'
        kp = s.rfind('"f"')
        if kp != -1:
            cp = find_colon_after(kp + 3)
            if cp != -1:
                val, _ = parse_bool_at(cp + 1)
                if val is not None:
                    f_val = val
                else:
                    f_ok = False
            else:
                f_ok = False
        else:
            f_ok = False

        if not (u_ok and s_ok and f_ok):
            # Fallback to full JSON parsing for correctness on edge cases
            obj = json.loads(s)
            u_val = obj["u"]
            s_val = obj["s"]
            f_val = obj["f"]

        v = (u_val & 1023) + s_val + (17 if f_val else 0)
        h ^= (v + 1)
        h = (h * 1099511628211) & mask

    return h
