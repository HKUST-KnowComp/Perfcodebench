import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      // parse u
      int i = 2; // skip "u="
      int u = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        i++;
      }
      // skip "&score="
      i += 7;
      int s = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') break;
        s = s * 10 + (c - '0');
        i++;
      }
      // skip "&flag="
      i += 6;
      int f = 0;
      while (i < len) {
        char c = row.charAt(i);
        f = f * 10 + (c - '0');
        i++;
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
