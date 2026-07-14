import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int size = rows.size();
    for (int i = 0; i < size; i++) {
      String row = rows.get(i);
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      int c5 = row.indexOf(',', c4 + 1);
      
      int b = 0;
      int idx = c2 + 1;
      boolean negB = false;
      if (idx < c3 && row.charAt(idx) == '-') { negB = true; idx++; }
      for (; idx < c3; idx++) b = b * 10 + (row.charAt(idx) - 48);
      if (negB) b = -b;
      
      int c = 0;
      idx = c3 + 1;
      boolean negC = false;
      if (idx < c4 && row.charAt(idx) == '-') { negC = true; idx++; }
      for (; idx < c4; idx++) c = c * 10 + (row.charAt(idx) - 48);
      if (negC) c = -c;
      
      boolean hot = false;
      int hStart = c4 + 1;
      int hEnd = (c5 == -1) ? row.length() : c5;
      if (hEnd - hStart == 3 && row.charAt(hStart) == 'h' && row.charAt(hStart + 1) == 'o' && row.charAt(hStart + 2) == 't') {
        hot = true;
      }
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}