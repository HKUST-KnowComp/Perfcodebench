import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int uStart = 2;
      int uEnd = row.indexOf('&', uStart);
      int u = parseInt(row, uStart, uEnd);
      
      int sStart = uEnd + 6;
      int sEnd = row.indexOf('&', sStart);
      int s = parseInt(row, sStart, sEnd);
      
      int fStart = sEnd + 6;
      int f = parseInt(row, fStart, row.length());
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInt(String s, int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++) {
      char c = s.charAt(i);
      val = val * 10 + (c - '0');
    }
    return val;
  }
}