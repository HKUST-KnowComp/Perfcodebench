import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      char[] chars = row.toCharArray();
      int u = 0, s = 0, f = 0, idx = 0;
      // Parse u
      idx += 2; // Skip 'u='
      while (chars[idx] != '&') {
        u = u * 10 + (chars[idx++] - '0');
      }
      idx += 6; // Skip '&s='
      // Parse s
      while (chars[idx] != '&') {
        s = s * 10 + (chars[idx++] - '0');
      }
      idx += 5; // Skip '&f='
      // Parse f
      while (idx < chars.length) {
        f = f * 10 + (chars[idx++] - '0');
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}