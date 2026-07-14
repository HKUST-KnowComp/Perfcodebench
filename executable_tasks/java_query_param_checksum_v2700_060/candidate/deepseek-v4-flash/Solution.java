import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstAmp = row.indexOf('&');
      int secondAmp = row.indexOf('&', firstAmp + 1);
      int u = parsePositiveInt(row, 2, firstAmp);
      int s = parsePositiveInt(row, firstAmp + 1 + 6, secondAmp);
      int f = parsePositiveInt(row, secondAmp + 1 + 5, row.length());
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parsePositiveInt(String s, int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return val;
  }
}