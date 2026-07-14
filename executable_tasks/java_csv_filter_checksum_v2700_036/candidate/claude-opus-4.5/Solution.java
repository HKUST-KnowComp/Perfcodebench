import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      
      // Skip field 0
      int idx = 0;
      while (idx < len && row.charAt(idx) != ',') idx++;
      idx++;
      
      // Skip field 1
      while (idx < len && row.charAt(idx) != ',') idx++;
      idx++;
      
      // Parse field 2 (b)
      int b = 0;
      boolean bNeg = false;
      if (idx < len && row.charAt(idx) == '-') {
        bNeg = true;
        idx++;
      }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') break;
        b = b * 10 + (ch - '0');
        idx++;
      }
      if (bNeg) b = -b;
      idx++;
      
      // Parse field 3 (c)
      int c = 0;
      boolean cNeg = false;
      if (idx < len && row.charAt(idx) == '-') {
        cNeg = true;
        idx++;
      }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') break;
        c = c * 10 + (ch - '0');
        idx++;
      }
      if (cNeg) c = -c;
      idx++;
      
      // Check field 4 for "hot"
      int remaining = len - idx;
      boolean hot = (remaining == 3 &&
                     row.charAt(idx) == 'h' &&
                     row.charAt(idx + 1) == 'o' &&
                     row.charAt(idx + 2) == 't');
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}