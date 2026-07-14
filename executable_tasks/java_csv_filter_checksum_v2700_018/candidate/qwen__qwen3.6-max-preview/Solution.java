import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = 0;
      
      // Skip field 0
      idx = row.indexOf(',', idx) + 1;
      // Skip field 1
      idx = row.indexOf(',', idx) + 1;
      
      // Parse b (field 2)
      int bStart = idx;
      idx = row.indexOf(',', idx);
      int b = 0;
      int p = bStart;
      if (row.charAt(p) == '-') p++;
      for (; p < idx; p++) b = b * 10 + (row.charAt(p) - '0');
      if (row.charAt(bStart) == '-') b = -b;
      
      // Parse c (field 3)
      int cStart = idx + 1;
      idx = row.indexOf(',', cStart);
      int c = 0;
      p = cStart;
      if (row.charAt(p) == '-') p++;
      for (; p < idx; p++) c = c * 10 + (row.charAt(p) - '0');
      if (row.charAt(cStart) == '-') c = -c;
      
      // Check hot (field 4)
      int hotStart = idx + 1;
      boolean hot = row.charAt(hotStart) == 'h' && 
                    row.charAt(hotStart + 1) == 'o' && 
                    row.charAt(hotStart + 2) == 't' &&
                    (hotStart + 3 == row.length() || row.charAt(hotStart + 3) == ',');
                    
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}