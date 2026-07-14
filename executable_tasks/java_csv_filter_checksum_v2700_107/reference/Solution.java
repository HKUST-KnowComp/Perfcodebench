import java.util.*;

public class Solution {
  static int parseIntBetween(String s, int start) {
    int v = 0;
    for (int i = start; i < s.length(); i++) {
      char c = s.charAt(i);
      if (c < '0' || c > '9') break;
      v = v * 10 + (c - '0');
    }
    return v;
  }

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      int b = parseIntBetween(row, c2 + 1);
      int c = parseIntBetween(row, c3 + 1);
      boolean hot = row.charAt(c4 + 1) == 'h';
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
