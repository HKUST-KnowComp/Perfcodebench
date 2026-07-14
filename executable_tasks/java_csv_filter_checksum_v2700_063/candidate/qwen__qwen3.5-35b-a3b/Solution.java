import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i1 = row.indexOf(',');
      int i2 = row.indexOf(',', i1 + 1);
      int i3 = row.indexOf(',', i2 + 1);
      int i4 = row.indexOf(',', i3 + 1);
      
      int b = parseInteger(row, i2 + 1, i3);
      int c = parseInteger(row, i3 + 1, i4);
      boolean hot = isHot(row, i4 + 1);
      
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
  
  private static boolean isHot(String s, int start) {
    int len = s.length() - start;
    if (len != 3) return false;
    return s.charAt(start) == 'h' &&
           s.charAt(start + 1) == 'o' &&
           s.charAt(start + 2) == 't';
  }
}