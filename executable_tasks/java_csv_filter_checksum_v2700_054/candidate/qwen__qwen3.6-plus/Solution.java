import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      int c5 = row.indexOf(',', c4 + 1);
      
      int b = 0;
      int i = c2 + 1;
      if (i < c3 && row.charAt(i) == '-') {
        i++;
        for (; i < c3; i++) b = b * 10 + (row.charAt(i) - '0');
        b = -b;
      } else {
        for (; i < c3; i++) b = b * 10 + (row.charAt(i) - '0');
      }
      
      int c = 0;
      i = c3 + 1;
      if (i < c4 && row.charAt(i) == '-') {
        i++;
        for (; i < c4; i++) c = c * 10 + (row.charAt(i) - '0');
        c = -c;
      } else {
        for (; i < c4; i++) c = c * 10 + (row.charAt(i) - '0');
      }
      
      int hotStart = c4 + 1;
      int hotEnd = (c5 == -1) ? row.length() : c5;
      boolean hot = (hotEnd - hotStart == 3) && 
                    row.charAt(hotStart) == 'h' && 
                    row.charAt(hotStart + 1) == 'o' && 
                    row.charAt(hotStart + 2) == 't';
                    
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}