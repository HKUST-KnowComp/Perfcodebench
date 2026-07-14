import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0, s = 0, f = 0;
      int state = 0;
      int num = 0;
      for (int i = 0; i < row.length(); i++) {
        char c = row.charAt(i);
        if (c == '&') {
          if (state == 0) {
            u = num;
          } else if (state == 1) {
            s = num;
          }
          state++;
          num = 0;
        } else if (c >= '0' && c <= '9') {
          num = num * 10 + (c - '0');
        }
      }
      f = num;
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}