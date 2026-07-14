import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 2; // Skip "u=" prefix
      int u = 0;
      while (i < row.length()) {
        char c = row.charAt(i);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        i++;
      }
      // Move past first &, skip s's 6-character prefix
      i += 7;
      int s = 0;
      while (i < row.length()) {
        char c = row.charAt(i);
        if (c == '&') break;
        s = s * 10 + (c - '0');
        i++;
      }
      // Move past second &, skip f's 5-character prefix
      i += 6;
      int f = 0;
      while (i < row.length()) {
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