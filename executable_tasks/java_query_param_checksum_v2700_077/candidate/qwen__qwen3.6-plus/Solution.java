import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx1 = row.indexOf('&');
      int idx2 = row.indexOf('&', idx1 + 1);
      
      int u = parseInt(row, 2);
      int s = parseInt(row, idx1 + 7);
      int f = parseInt(row, idx2 + 6);
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInt(String s, int start) {
    int n = 0;
    int len = s.length();
    int i = start;
    boolean neg = false;
    if (i < len && s.charAt(i) == '-') {
      neg = true;
      i++;
    }
    for (; i < len; i++) {
      char c = s.charAt(i);
      if (c < '0' || c > '9') break;
      n = n * 10 + (c - '0');
    }
    return neg ? -n : n;
  }
}