import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET;
    for (int r = 0, rn = rows.size(); r < rn; r++) {
      String s = rows.get(r);
      int len = s.length();
      int i = 0;
      int b = 0;
      int c = 0;
      boolean hot = false;

      for (int field = 0; field <= 4; field++) {
        int start = i;
        while (i < len && s.charAt(i) != ',') {
          i++;
        }
        int end = i;

        if (field == 2) {
          b = parseIntRange(s, start, end);
        } else if (field == 3) {
          c = parseIntRange(s, start, end);
        } else if (field == 4) {
          if (end - start == 3 && s.charAt(start) == 'h' && s.charAt(start + 1) == 'o' && s.charAt(start + 2) == 't') {
            hot = true;
          }
        }

        if (i == len) {
          break;
        }
        i++; // skip comma
      }

      long v = ((b > 300) ? (long) (b + c) : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= FNV64_PRIME;
    }
    return hash;
  }

  private static int parseIntRange(String s, int start, int end) {
    if (start >= end) return 0;
    int idx = start;
    boolean neg = false;
    char ch = s.charAt(idx);
    if (ch == '-') {
      neg = true;
      idx++;
    } else if (ch == '+') {
      idx++;
    }
    int val = 0;
    while (idx < end) {
      ch = s.charAt(idx++);
      val = val * 10 + (ch - '0');
    }
    return neg ? -val : val;
  }
}
