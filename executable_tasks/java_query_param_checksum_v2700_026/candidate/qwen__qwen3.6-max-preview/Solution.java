import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int i = 0;
      
      while (row.charAt(i) != '=') i++;
      i++;
      for (;;) {
        char ch = row.charAt(i);
        if (ch == '&') break;
        u = u * 10 + (ch - '0');
        i++;
      }
      i++;
      
      while (row.charAt(i) != '=') i++;
      i++;
      for (;;) {
        char ch = row.charAt(i);
        if (ch == '&') break;
        s = s * 10 + (ch - '0');
        i++;
      }
      i++;
      
      while (row.charAt(i) != '=') i++;
      i++;
      for (; i < len; i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}