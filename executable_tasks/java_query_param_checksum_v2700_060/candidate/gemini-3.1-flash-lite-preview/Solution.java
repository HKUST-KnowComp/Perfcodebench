import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int state = 0;
      int val = 0;
      
      for (int i = 0; i < len; i++) {
        char c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          val = val * 10 + (c - '0');
        } else if (c == '&') {
          if (state == 0) u = val;
          else if (state == 1) s = val;
          state++;
          val = 0;
        }
      }
      f = val;
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}