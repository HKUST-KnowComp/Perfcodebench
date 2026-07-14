import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int size = rows.size();
    for (int i = 0; i < size; i++) {
      String row = rows.get(i);

      int len = row.length();
      int p = 2; // skip "u="

      int u = 0;
      while (p < len) {
        char c = row.charAt(p++);
        if (c == '&') break;
        u = u * 10 + (c - '0');
      }

      p += 5; // skip "score="
      int s = 0;
      while (p < len) {
        char c = row.charAt(p++);
        if (c == '&') break;
        s = s * 10 + (c - '0');
      }

      p += 4; // skip "flag="
      int f = 0;
      while (p < len) {
        f = f * 10 + (row.charAt(p++) - '0');
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
