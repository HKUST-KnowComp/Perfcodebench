from urllib.parse import parse_qs

def run(rows: list[str]) -> int:
    h = 1469598103934665603
    for row in rows:
        # Find indices of 'u', 'score', and 'flag' parameters in the query string
        u_idx = row.find('u=')
        score_idx = row.find('score=')
        flag_idx = row.find('flag=')

        # Extract values directly from the query string
        u_end_idx = row.find('&', u_idx)
        if u_end_idx == -1:
            u_end_idx = len(row)
        u = int(row[u_idx + 2:u_end_idx])

        score_end_idx = row.find('&', score_idx)
        if score_end_idx == -1:
            score_end_idx = len(row)
        s = int(row[score_idx + 6:score_end_idx])

        flag_end_idx = row.find('&', flag_idx)
        if flag_end_idx == -1:
            flag_end_idx = len(row)
        f = int(row[flag_idx + 5:flag_end_idx])

        v = (u & 2047) + s + f * 19
        h ^= v + 1
        h *= 1099511628211
        h &= (1 << 64) - 1
    return h