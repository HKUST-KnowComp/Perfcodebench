import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstAmp = row.indexOf('&');
      int secondAmp = row.indexOf('&', firstAmp + 1);

      // u: parts[0].substring(2)
      int u = parseInt(row, 2, firstAmp);
      // s: parts[1].substring(6)
      int s = parseInt(row, firstAmp + 7, secondAmp);
      // f: parts[2].substring(5)
      int f = parseInt(row, secondAmp + 6, row.length());

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int num = 0;
    for (int i = start; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num;
  }
}