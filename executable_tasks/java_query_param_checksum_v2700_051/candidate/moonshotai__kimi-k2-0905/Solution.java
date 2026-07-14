import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;

    for (String row : rows) {
      char[] cs = row.toCharArray();
      int len = cs.length;

      int u = 0, s = 0, f = 0;
      int pos = 0;

      // skip "u="
      pos = 2;
      while (pos < len && cs[pos] != '&') {
        u = u * 10 + (cs[pos] - '0');
        pos++;
      }

      // skip "&s="
      pos += 3;
      while (pos < len && cs[pos] != '&') {
        s = s * 10 + (cs[pos] - '0');
        pos++;
      }

      // skip "&f="
      pos += 3;
      while (pos < len) {
        f = f * 10 + (cs[pos] - '0');
        pos++;
      }

      long v = ((u & 2047) + s + f * 19L) + 1;
      hash ^= v;
      hash *= 1099511628211L;
    }
    return hash;
  }
}