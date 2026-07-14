import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    for (String row : rows) {
      int len = row.length();
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      
      int b = 0;
      int start = c2 + 1;
      boolean bNeg = row.charAt(start) == '-';
      if (bNeg) start++;
      for (int j = start; j < c3; j++) {
        b = b * 10 + (row.charAt(j) - '0');
      }
      if (bNeg) b = -b;
      
      int c = 0;
      start = c3 + 1;
      boolean cNeg = row.charAt(start) == '-';
      if (cNeg) start++;
      for (int j = start; j < c4; j++) {
        c = c * 10 + (row.charAt(j) - '0');
      }
      if (cNeg) c = -c;
      
      boolean hot = false;
      int hStart = c4 + 1;
      if (hStart + 3 <= len && 
          row.charAt(hStart) == 'h' && 
          row.charAt(hStart+1) == 'o' && 
          row.charAt(hStart+2) == 't') {
          int end = hStart + 3;
          if (end == len || row.charAt(end) == ',') {
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