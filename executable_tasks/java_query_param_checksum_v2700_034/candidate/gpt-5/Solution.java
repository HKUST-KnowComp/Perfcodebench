import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (int r = 0, size = rows.size(); r < size; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 2; // skip "u="

      int u = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') { i++; break; }
        u = u * 10 + (c - '0');
        i++;
      }

      // now at start of "score=" after '&'
      i += 6; // skip "score="
      int s = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') { i++; break; }
        s = s * 10 + (c - '0');
        i++;
      }

      // now at start of "flag=" after '&'
      i += 5; // skip "flag="
      int f = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') { break; }
        f = f * 10 + (c - '0');
        i++;
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= prime;
    }

    return hash;
  }
}
