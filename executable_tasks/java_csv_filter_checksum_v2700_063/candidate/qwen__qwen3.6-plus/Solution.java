import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String s : rows) {
      int c1 = s.indexOf(',');
      int c2 = s.indexOf(',', c1 + 1);
      int c3 = s.indexOf(',', c2 + 1);
      int c4 = s.indexOf(',', c3 + 1);
      int c5 = s.indexOf(',', c4 + 1);
      if (c5 == -1) c5 = s.length();
      
      int b = 0;
      for (int i = c2 + 1; i < c3; i++) {
        b = b * 10 + (s.charAt(i) - '0');
      }
      
      int c = 0;
      for (int i = c3 + 1; i < c4; i++) {
        c = c * 10 + (s.charAt(i) - '0');
      }
      
      boolean hot = (c5 - c4 == 4) && s.charAt(c4 + 1) == 'h' && s.charAt(c4 + 2) == 'o' && s.charAt(c4 + 3) == 't';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}