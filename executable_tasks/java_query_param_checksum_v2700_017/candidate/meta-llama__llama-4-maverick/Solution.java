import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0, s = 0, f = 0;
      int state = 0;
      for (int i = 0; i < row.length(); i++) {
        char c = row.charAt(i);
        if (c == '&') {
          state++;
        } else if (c >= '0' && c <= '9') {
          switch (state) {
            case 0:
              u = u * 10 + (c - '0');
              break;
            case 1:
              s = s * 10 + (c - '0');
              break;
            case 2:
              f = f * 10 + (c - '0');
              break;
          }
        }
      }
      u &= 2047;
      long v = u + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}