import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int idx = 0;
      
      // Skip field 0
      while (idx < len && row.charAt(idx) != ',') idx++;
      idx++; 
      // Skip field 1
      while (idx < len && row.charAt(idx) != ',') idx++;
      idx++; 
      
      // Field 2 (b)
      int start2 = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      int end2 = idx;
      idx++;
      
      int b = 0;
      int i = start2;
      boolean neg = false;
      if (i < end2 && row.charAt(i) == '-') {
        neg = true;
        i++;
      }
      for (; i < end2; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      if (neg) b = -b;
      
      // Field 3 (c)
      int start3 = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      int end3 = idx;
      idx++;
      
      int c = 0;
      i = start3;
      neg = false;
      if (i < end3 && row.charAt(i) == '-') {
        neg = true;
        i++;
      }
      for (; i < end3; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      if (neg) c = -c;
      
      // Field 4 (hot)
      int start4 = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      int end4 = idx;
      
      boolean hot = false;
      if (end4 - start4 == 3) {
        hot = (row.charAt(start4) == 'h' && row.charAt(start4 + 1) == 'o' && row.charAt(start4 + 2) == 't');
      }
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}