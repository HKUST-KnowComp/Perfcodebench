import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx1 = row.indexOf('&');
      int idx2 = row.indexOf('&', idx1 + 1);
      int len = row.length();
      
      int u = 0;
      int i = 2;
      if (row.charAt(i) == '-') i++;
      while (i < idx1) {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      if (row.charAt(2) == '-') u = -u;
      
      int s = 0;
      i = idx1 + 7;
      if (row.charAt(i) == '-') i++;
      while (i < idx2) {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      if (row.charAt(idx1 + 7) == '-') s = -s;
      
      int f = 0;
      i = idx2 + 6;
      if (row.charAt(i) == '-') i++;
      while (i < len) {
        f = f * 10 + (row.charAt(i) - '0');
        i++;
      }
      if (row.charAt(idx2 + 6) == '-') f = -f;
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}