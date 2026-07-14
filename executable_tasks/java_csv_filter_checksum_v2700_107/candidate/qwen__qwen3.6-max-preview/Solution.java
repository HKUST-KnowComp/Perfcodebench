import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      
      int bStart = c2 + 1;
      int b = 0;
      if (row.charAt(bStart) == '-') {
        for (int j = bStart + 1; j < c3; j++) b = b * 10 + (row.charAt(j) - '0');
        b = -b;
      } else {
        for (int j = bStart; j < c3; j++) b = b * 10 + (row.charAt(j) - '0');
      }
      
      int cStart = c3 + 1;
      int c = 0;
      if (row.charAt(cStart) == '-') {
        for (int j = cStart + 1; j < c4; j++) c = c * 10 + (row.charAt(j) - '0');
        c = -c;
      } else {
        for (int j = cStart; j < c4; j++) c = c * 10 + (row.charAt(j) - '0');
      }
      
      int hStart = c4 + 1;
      boolean hot = (hStart + 3 <= len) &&
                    row.charAt(hStart) == 'h' &&
                    row.charAt(hStart + 1) == 'o' &&
                    row.charAt(hStart + 2) == 't' &&
                    (hStart + 3 == len || row.charAt(hStart + 3) == ',');
                    
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}