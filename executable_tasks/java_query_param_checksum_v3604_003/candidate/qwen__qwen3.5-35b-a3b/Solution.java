import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = row.indexOf('&');
      int j = row.indexOf('&', i + 1);
      int u = parseNumber(row, 2, i);
      int s = parseNumber(row, i + 7, j);
      int f = parseNumber(row, j + 6, row.length());
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseNumber(String s, int start, int end) {
    int num = 0;
    int sign = 1;
    int i = start;
    if (i < end && s.charAt(i) == '-') {
      sign = -1;
      i++;
    }
    for (; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num * sign;
  }
}