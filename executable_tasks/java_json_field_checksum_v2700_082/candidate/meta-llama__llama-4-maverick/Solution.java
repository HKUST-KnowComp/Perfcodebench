import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0, s = 0;
      boolean f = false;
      int state = 0;
      for (int i = 0; i < row.length(); i++) {
        char c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          if (state == 1) {
            u = u * 10 + (c - '0');
          } else if (state == 2) {
            s = s * 10 + (c - '0');
          }
        } else if (c == ',') {
          state++;
        } else if (c == 't') {
          f = true;
        }
      }
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}