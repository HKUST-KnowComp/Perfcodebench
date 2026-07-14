import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0, s = 0, f = 0;
      int i = 2; // skip "u="
      // parse u until '&'
      while (row.charAt(i) != '&') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 3; // skip "&s="
      while (row.charAt(i) != '&') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      i += 3; // skip "&f="
      while (i < row.length()) {
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