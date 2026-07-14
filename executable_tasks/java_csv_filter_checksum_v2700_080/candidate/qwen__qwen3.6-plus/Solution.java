import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    
    for (String row : rows) {
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      int c5 = row.indexOf(',', c4 + 1);
      if (c5 == -1) c5 = row.length();
      
      int b = 0;
      int i = c2 + 1;
      boolean neg = false;
      if (i < c3 && row.charAt(i) == '-') { neg = true; i++; }
      for (; i < c3; i++) b = b * 10 + (row.charAt(i) - '0');
      if (neg) b = -b;
      
      int c = 0;
      i = c3 + 1;
      neg = false;
      if (i < c4 && row.charAt(i) == '-') { neg = true; i++; }
      for (; i < c4; i++) c = c * 10 + (row.charAt(i) - '0');
      if (neg) c = -c;
      
      boolean hot = false;
      if (c5 - c4 == 4) {
        int h = c4 + 1;
        if (row.charAt(h) == 'h' && row.charAt(h+1) == 'o' && row.charAt(h+2) == 't') {
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