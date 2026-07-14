def run(rows: list[str]) -> int:
    h = 1469598103934665603
    mask64 = (1 << 64) - 1

    for row in rows:
        u = 0
        s = 0
        f = 0
        
        # Manual scan to avoid parse_qs overhead
        # We look for 'u=', 'score=', and 'flag=' patterns
        # Query params are separated by '&' or ';' (though standard is '&')
        # and start after '?'
        
        start_idx = row.find('?')
        if start_idx == -1:
            start_idx = 0
        else:
            start_idx += 1
            
        current_pos = start_idx
        length = len(row)
        
        while current_pos < length:
            # Find end of current pair
            next_sep = row.find('&', current_pos)
            if next_sep == -1:
                pair_end = length
            else:
                pair_end = next_sep
            
            # Split key and value
            sep_idx = row.find('=', current_pos)
            if sep_idx != -1 and sep_idx < pair_end:
                key = row[current_pos:sep_idx]
                val_str = row[sep_idx + 1:pair_end]
                
                if key == 'u':
                    u = int(val_str)
                elif key == 'score':
                    s = int(val_str)
                elif key == 'flag':
                    f = int(val_str)
            
            current_pos = pair_end + 1

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h = (h * 1099511628211) & mask64
        
    return h