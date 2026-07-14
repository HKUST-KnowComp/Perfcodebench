import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    for (int r = 0, rn = rows.size(); r < rn; r++) {
      String s = rows.get(r);
      int len = s.length();
      int i = 0;

      // Skip field 0
      while (i < len && s.charAt(i) != ',') i++;
      if (i < len) i++; // skip comma
      // Skip field 1
      while (i < len && s.charAt(i) != ',') i++;
      if (i < len) i++; // now at field 2 start

      // Parse field 2: integer b
      int b = 0;
      boolean neg = false;
      if (i < len) {
        char ch = s.charAt(i);
        if (ch == '-') { neg = true; i++; }
        int val = 0;
        while (i < len) {
          ch = s.charAt(i);
          if (ch == ',') break;
          val = val * 10 + (ch - '0');
          i++;
        }
        b = neg ? -val : val;
      }
      if (i < len && s.charAt(i) == ',') i++; // move to field 3

      // Parse field 3: integer c
      int c = 0;
      neg = false;
      if (i < len) {
        char ch = s.charAt(i);
        if (ch == '-') { neg = true; i++; }
        int val = 0;
        while (i < len) {
          ch = s.charAt(i);
          if (ch == ',') break;
          val = val * 10 + (ch - '0');
          i++;
        }
        c = neg ? -val : val;
      }
      if (i < len && s.charAt(i) == ',') i++; // move to field 4

      // Parse field 4: check equals "hot"
      int start = i;
      while (i < len && s.charAt(i) != ',') i++;
      int end = i;
      boolean hot = (end - start == 3
          && start + 2 < len
          && s.charAt(start) == 'h'
          && s.charAt(start + 1) == 'o'
          && s.charAt(start + 2) == 't');

      long v = ((b > 300) ? (long) (b + c) : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
