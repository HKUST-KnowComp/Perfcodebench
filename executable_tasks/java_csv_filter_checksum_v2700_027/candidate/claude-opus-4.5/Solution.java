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
        if (row.charAt(idx) == ',') commaCount++;
        idx++;
      }
      
      // Parse field b (3rd field)
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
      idx++; // skip comma
      
      // Parse field c (4th field)
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
      idx++; // skip comma
      
      // Check if 5th field is "hot" (length 3, chars h-o-t)
      int fieldStart = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      int fieldLen = idx - fieldStart;
      boolean hot = (fieldLen == 3 
          && row.charAt(fieldStart) == 'h' 
          && row.charAt(fieldStart + 1) == 'o' 
          && row.charAt(fieldStart + 2) == 't');
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}