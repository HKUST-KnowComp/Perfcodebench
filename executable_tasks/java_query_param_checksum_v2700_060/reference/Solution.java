import java.util.*;

public class Solution {
  static int parseIntAt(String s, int start) {
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
      int up = row.indexOf("u=") + 2;
      int sp = row.indexOf("&score=") + 7;
      int fp = row.indexOf("&flag=") + 6;
      int u = parseIntAt(row, up);
      int s = parseIntAt(row, sp);
      int f = parseIntAt(row, fp);
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
