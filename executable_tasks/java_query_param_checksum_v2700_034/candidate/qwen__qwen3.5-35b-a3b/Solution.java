import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int index1 = row.indexOf('&');
      int index2 = row.indexOf('&', index1 + 1);
      int u = parseNumber(row, 2, index1);
      int s = parseNumber(row, index1 + 7, index2);
      int f = parseNumber(row, index2 + 6, row.length());
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseNumber(String s, int start, int end) {
    int num = 0;
    for (int i = start; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num;
  }
}