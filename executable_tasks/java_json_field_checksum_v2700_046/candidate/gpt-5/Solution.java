import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int i = 0;
      // Parse u
      while (row.charAt(i) != ':') i++;
      i++; // at first digit of u
      int u = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch >= '0' && ch <= '9') {
          u = u * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }
      // Parse s
      while (row.charAt(i) != ':') i++;
      i++; // at first digit of s
      int s = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch >= '0' && ch <= '9') {
          s = s * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }
      // Parse f
      while (row.charAt(i) != ':') i++;
      i++; // at first character of boolean
      boolean f = (row.charAt(i) == 't');

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= prime;
    }
    return hash;
  }
}
