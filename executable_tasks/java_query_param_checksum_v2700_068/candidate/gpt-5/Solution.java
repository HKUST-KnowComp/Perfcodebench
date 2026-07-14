import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int idx = 0;

      int u = 0, s = 0, f = 0;
      int numCount = 0;

      while (numCount < 3 && idx < len) {
        // Find '=' for the current parameter
        while (idx < len && row.charAt(idx) != '=') {
          idx++;
        }
        idx++; // move past '='

        // Parse optional sign
        boolean neg = false;
        if (idx < len) {
          char ch = row.charAt(idx);
          if (ch == '-') {
            neg = true;
            idx++;
          } else if (ch == '+') {
            idx++;
          }
        }

        // Parse integer digits
        int val = 0;
        while (idx < len) {
          char ch = row.charAt(idx);
          int d = ch - '0';
          if (d < 0 || d > 9) break;
          val = val * 10 + d;
          idx++;
        }
        if (neg) val = -val;

        if (numCount == 0) {
          u = val;
        } else if (numCount == 1) {
          s = val;
        } else {
          f = val;
        }
        numCount++;

        // Skip to next '&' if present
        while (idx < len && row.charAt(idx) != '&') {
          idx++;
        }
        if (idx < len && row.charAt(idx) == '&') {
          idx++; // move past '&'
        }
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}