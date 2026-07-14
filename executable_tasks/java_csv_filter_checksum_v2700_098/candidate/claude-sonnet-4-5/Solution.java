import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx1 = row.indexOf(',');
      int idx2 = row.indexOf(',', idx1 + 1);
      int idx3 = row.indexOf(',', idx2 + 1);
      int idx4 = row.indexOf(',', idx3 + 1);
      
      int b = parseInt(row, idx2 + 1, idx3);
      int c = parseInt(row, idx3 + 1, idx4);
      boolean hot = row.charAt(idx4 + 1) == 'h';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInt(String s, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return result;
  }
}