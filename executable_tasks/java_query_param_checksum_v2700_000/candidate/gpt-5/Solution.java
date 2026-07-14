import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 0;

      // Parse first integer value after first '='
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      int u = 0;
      if (i < len) {
        boolean neg = false;
        char c = row.charAt(i);
        if (c == '-') { neg = true; i++; }
        while (i < len) {
          c = row.charAt(i);
          if (c >= '0' && c <= '9') {
            u = u * 10 + (c - '0');
            i++;
          } else {
            break;
          }
        }
        if (neg) u = -u;
      }

      // Parse second integer value after next '='
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      int sVal = 0;
      if (i < len) {
        boolean neg = false;
        char c = row.charAt(i);
        if (c == '-') { neg = true; i++; }
        while (i < len) {
          c = row.charAt(i);
          if (c >= '0' && c <= '9') {
            sVal = sVal * 10 + (c - '0');
            i++;
          } else {
            break;
          }
        }
        if (neg) sVal = -sVal;
      }

      // Parse third integer value after next '='
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      int fVal = 0;
      if (i < len) {
        boolean neg = false;
        char c = row.charAt(i);
        if (c == '-') { neg = true; i++; }
        while (i < len) {
          c = row.charAt(i);
          if (c >= '0' && c <= '9') {
            fVal = fVal * 10 + (c - '0');
            i++;
          } else {
            break;
          }
        }
        if (neg) fVal = -fVal;
      }

      long v = (u & 2047) + sVal + (fVal * 19L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}