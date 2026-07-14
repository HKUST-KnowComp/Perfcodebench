import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      
      int b = parseInteger(row, c2 + 1, c3);
      int c = parseInteger(row, c3 + 1, c4);
      boolean hot = isHot(row, c4 + 1, row.length());
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInteger(String s, int start, int end) {
    int num = 0;
    for (int i = start; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num;
  }
  
  private static boolean isHot(String s, int start, int end) {
    int len = end - start;
    if (len != 3) return false;
    return s.charAt(start) == 'h' &&
           s.charAt(start + 1) == 'o' &&
           s.charAt(start + 2) == 't';
  }
}