import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      byte[] b = row.getBytes();
      int u = 0, s = 0, f = 0;

      // u after "u="
      int i = 2;
      while (b[i] != '&') {
        u = u * 10 + (b[i++] - '0');
      }

      // s after "&s="
      i += 3;
      while (b[i] != '&') {
        s = s * 10 + (b[i++] - '0');
      }

      // f after "&f="
      i += 3;
      while (i < b.length) {
        f = f * 10 + (b[i++] - '0');
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}