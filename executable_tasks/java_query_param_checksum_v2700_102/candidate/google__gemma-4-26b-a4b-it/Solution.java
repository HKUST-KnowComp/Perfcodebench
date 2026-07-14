import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      
      // Part 0: u (starts at index 2, ends at first '&')
      int firstAmp = row.indexOf('&');
      int u = parseFastInt(row, 2, firstAmp);
      
      // Part 1: s (starts at index firstAmp + 7, ends at second '&')
      int secondAmp = row.indexOf('&', firstAmp + 1);
      int s = parseFastInt(row, firstAmp + 7, secondAmp);
      
      // Part 2: f (starts at index secondAmp + 6, ends at end of string)
      int f = parseFastInt(row, secondAmp + 6, len);
      
      long v = (u & 2047) + s + (long) f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseFastInt(String s, int start, int end) {
    int res = 0;
    for (int i = start; i < end; i++) {
      res = res * 10 + (s.charAt(i) - '0');
    }
    return res;
  }
}