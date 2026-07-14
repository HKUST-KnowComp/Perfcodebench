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
        if (state == 0 && c == ':') state = 1;
        else if (state == 1 && c >= '0' && c <= '9') {
          u = u * 10 + (c - '0');
        } else if (state == 1 && c == ',') state = 2;
        else if (state == 2 && c == ':') state = 3;
        else if (state == 3 && c >= '0' && c <= '9') {
          s = s * 10 + (c - '0');
        } else if (state == 3 && c == ',') state = 4;
        else if (state == 4 && c == ':') state = 5;
        else if (state == 5 && c == 't') f = true;
        else if (state == 5 && c == 'f') f = false;
      }
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}