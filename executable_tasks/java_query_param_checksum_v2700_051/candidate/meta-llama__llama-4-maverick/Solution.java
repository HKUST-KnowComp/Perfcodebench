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
        } else if (state == 0 && i >= 2) {
          u = u * 10 + (c - '0');
        } else if (state == 1 && i >= 8) {
          s = s * 10 + (c - '0');
        } else if (state == 2 && i >= 7) {
          f = f * 10 + (c - '0');
        }
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}