import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 0;
      int u = 0, s = 0, f = 0;
      int val;
      char c;

      // Parse u
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      val = 0;
      while (i < len && (c = row.charAt(i)) >= '0' && c <= '9') {
        val = val * 10 + (c - '0');
        i++;
      }
      u = val;

      // Parse s
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      val = 0;
      while (i < len && (c = row.charAt(i)) >= '0' && c <= '9') {
        val = val * 10 + (c - '0');
        i++;
      }
      s = val;

      // Parse f
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      val = 0;
      while (i < len && (c = row.charAt(i)) >= '0' && c <= '9') {
        val = val * 10 + (c - '0');
        i++;
      }
      f = val;

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}