import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      
      int b = 0;
      int idx = c2 + 1;
      boolean neg = false;
      if (row.charAt(idx) == '-') {
        neg = true;
        idx++;
      }
      for (; idx < c3; idx++) {
        b = b * 10 + (row.charAt(idx) - '0');
      }
      if (neg) b = -b;
      
      int c = 0;
      idx = c3 + 1;
      neg = false;
      if (row.charAt(idx) == '-') {
        neg = true;
        idx++;
      }
      for (; idx < c4; idx++) {
        c = c * 10 + (row.charAt(idx) - '0');
      }
      if (neg) c = -c;
      
      int hotStart = c4 + 1;
      boolean hot = false;
      if (hotStart + 3 <= row.length() &&
          row.charAt(hotStart) == 'h' &&
          row.charAt(hotStart + 1) == 'o' &&
          row.charAt(hotStart + 2) == 't') {
        if (hotStart + 3 == row.length() || row.charAt(hotStart + 3) == ',') {
          hot = true;
        }
      }
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }
    return hash;
  }
}