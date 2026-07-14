import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      
      // Skip first two fields (find 3rd comma)
      int idx = 0;
      int commaCount = 0;
      while (commaCount < 2) {
        if (row.charAt(idx) == ',') commaCount++;
        idx++;
      }
      
      // Parse field 2 (b)
      int b = 0;
      boolean neg = false;
      if (idx < len && row.charAt(idx) == '-') {
        neg = true;
        idx++;
      }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') break;
        b = b * 10 + (ch - '0');
        idx++;
      }
      if (neg) b = -b;
      idx++; // skip comma
      
      // Parse field 3 (c)
      int c = 0;
      neg = false;
      if (idx < len && row.charAt(idx) == '-') {
        neg = true;
        idx++;
      }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') break;
        c = c * 10 + (ch - '0');
        idx++;
      }
      if (neg) c = -c;
      idx++; // skip comma
      
      // Check field 4 for "hot" (3 chars starting with 'h')
      boolean hot = (idx + 2 < len) && 
                    row.charAt(idx) == 'h' && 
                    row.charAt(idx + 1) == 'o' && 
                    row.charAt(idx + 2) == 't';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}