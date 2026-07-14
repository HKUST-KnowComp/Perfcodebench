import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0, s = 0, f = 0;
      int idx = 0;
      // Parse u value after first '='
      while (idx < row.length() && row.charAt(idx) != '=') idx++;
      idx++;
      while (idx < row.length()) {
        char c = row.charAt(idx);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        idx++;
      }
      // Parse s value after its '='
      while (idx < row.length() && row.charAt(idx) != '=') idx++;
      idx++;
      while (idx < row.length()) {
        char c = row.charAt(idx);
        if (c == '&') break;
        s = s * 10 + (c - '0');
        idx++;
      }
      // Parse f value after its '='
      while (idx < row.length() && row.charAt(idx) != '=') idx++;
      idx++;
      while (idx < row.length()) {
        char c = row.charAt(idx);
        f = f * 10 + (c - '0');
        idx++;
      }
      // Exact original hash calculation preserved for correctness
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}