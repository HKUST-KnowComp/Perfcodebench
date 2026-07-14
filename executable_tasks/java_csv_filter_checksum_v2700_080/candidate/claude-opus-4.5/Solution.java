import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      
      // Skip first two fields (find 2nd comma)
      int idx = 0;
      int commaCount = 0;
      while (commaCount < 2) {
        if (row.charAt(idx++) == ',') commaCount++;
      }
      
      // Parse field b (3rd field)
      int b = 0;
      char ch = row.charAt(idx);
      boolean neg = ch == '-';
      if (neg) idx++;
      while (idx < len && (ch = row.charAt(idx)) != ',') {
        b = b * 10 + (ch - '0');
        idx++;
      }
      if (neg) b = -b;
      idx++; // skip comma
      
      // Parse field c (4th field)
      int c = 0;
      ch = row.charAt(idx);
      neg = ch == '-';
      if (neg) idx++;
      while (idx < len && (ch = row.charAt(idx)) != ',') {
        c = c * 10 + (ch - '0');
        idx++;
      }
      idx++; // skip comma
      
      // Check if field 5 is "hot" (length 3, starts with 'h')
      int fieldStart = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      boolean hot = (idx - fieldStart == 3) && row.charAt(fieldStart) == 'h';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}