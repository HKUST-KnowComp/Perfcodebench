import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int p1 = row.indexOf('&');
      int p2 = row.indexOf('&', p1 + 1);
      
      int u = 0;
      for (int j = 2; j < p1; j++) {
        u = u * 10 + (row.charAt(j) - '0');
      }
      
      int s = 0;
      for (int j = p1 + 7; j < p2; j++) {
        s = s * 10 + (row.charAt(j) - '0');
      }
      
      int f = 0;
      for (int j = p2 + 6; j < len; j++) {
        char c = row.charAt(j);
        if (c < '0' || c > '9') break;
        f = f * 10 + (c - '0');
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}