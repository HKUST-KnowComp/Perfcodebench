import java.util.*;

public class Solution {
  private static int parseNum(String s, int start, int end) {
    int res = 0;
    for (int i = start; i < end; i++) {
      res = res * 10 + (s.charAt(i) - '0');
    }
    return res;
  }

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstAmp = row.indexOf('&');
      int secondAmp = row.indexOf('&', firstAmp + 1);
      int u = parseNum(row, 2, firstAmp);
      int s = parseNum(row, firstAmp + 7, secondAmp);
      int f = parseNum(row, secondAmp + 6, row.length());
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}