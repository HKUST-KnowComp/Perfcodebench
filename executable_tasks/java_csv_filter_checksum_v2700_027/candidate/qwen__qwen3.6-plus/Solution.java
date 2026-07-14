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
      if (c5 == -1) c5 = row.length();

      int b = parseNum(row, c2 + 1, c3);
      int c = parseNum(row, c3 + 1, c4);
      
      boolean hot = (c5 - c4 - 1 == 3) &&
                    row.charAt(c4 + 1) == 'h' &&
                    row.charAt(c4 + 2) == 'o' &&
                    row.charAt(c4 + 3) == 't';
                    
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseNum(String s, int start, int end) {
    int res = 0;
    boolean neg = false;
    if (start < end && s.charAt(start) == '-') {
      neg = true;
      start++;
    }
    for (int i = start; i < end; i++) {
      res = res * 10 + (s.charAt(i) - '0');
    }
    return neg ? -res : res;
  }
}