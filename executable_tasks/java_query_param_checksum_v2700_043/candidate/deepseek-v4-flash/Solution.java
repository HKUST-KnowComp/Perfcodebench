import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 2; // skip "u="
      int u = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 6; // skip "&sess="
      int s = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 6; // skip "&flag="
      int f = 0;
      while (i < len && row.charAt(i) >= '0' && row.charAt(i) <= '9') {
        f = f * 10 + (row.charAt(i) - '0');
        i++;
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}