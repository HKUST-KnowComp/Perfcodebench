import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i1 = row.indexOf(',');
      int i2 = row.indexOf(',', i1 + 1);
      int i3 = row.indexOf(',', i2 + 1);
      int i4 = row.indexOf(',', i3 + 1);
      
      int b = parseInteger(row, i1 + 1, i2);
      int c = parseInteger(row, i2 + 1, i3);
      
      boolean hot = (row.length() - (i3 + 1) == 3) && 
                    row.charAt(i3 + 1) == 'h' && 
                    row.charAt(i3 + 2) == 'o' && 
                    row.charAt(i3 + 3) == 't';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInteger(String s, int start, int end) {
    int num = 0;
    int sign = 1;
    int i = start;
    if (s.charAt(i) == '-') {
      sign = -1;
      i++;
    }
    for (; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num * sign;
  }
}