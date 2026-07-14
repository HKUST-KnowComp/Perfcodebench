import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx1 = row.indexOf('&');
      int idx2 = row.indexOf('&', idx1 + 1);
      
      int u = parseIntFast(row, 2, idx1);
      int s = parseIntFast(row, idx1 + 7, idx2);
      int f = parseIntFast(row, idx2 + 6, row.length());
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseIntFast(String s, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return result;
  }
}
