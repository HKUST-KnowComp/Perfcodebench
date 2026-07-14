import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET;
    final int size = rows.size();
    for (int r = 0; r < size; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 2; // skip first part prefix length

      // Parse first integer (u)
      int u;
      boolean neg = false;
      char c = row.charAt(i);
      if (c == '-') { neg = true; i++; }
      int val = 0;
      while (i < len) {
        c = row.charAt(i);
        if (c == '&') { break; }
        val = val * 10 + (c - '0');
        i++;
      }
      u = neg ? -val : val;

      // Move to second part start
      i++;      // skip '&'
      i += 6;   // skip second part prefix length

      // Parse second integer (s)
      int sVal;
      neg = false;
      c = row.charAt(i);
      if (c == '-') { neg = true; i++; }
      val = 0;
      while (i < len) {
        c = row.charAt(i);
        if (c == '&') { break; }
        val = val * 10 + (c - '0');
        i++;
      }
      sVal = neg ? -val : val;

      // Move to third part start
      i++;     // skip '&'
      i += 5;  // skip third part prefix length

      // Parse third integer (f)
      int f;
      neg = false;
      c = row.charAt(i);
      if (c == '-') { neg = true; i++; }
      val = 0;
      while (i < len) {
        c = row.charAt(i);
        if (c == '&') { break; }
        val = val * 10 + (c - '0');
        i++;
      }
      f = neg ? -val : val;

      long v = (u & 2047) + sVal + f * 19L;
      hash ^= (v + 1);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}