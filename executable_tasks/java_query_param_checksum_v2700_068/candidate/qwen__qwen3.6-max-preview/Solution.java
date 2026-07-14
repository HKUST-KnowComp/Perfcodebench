import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);
      
      int u = fastParse(row, 2, amp1);
      int s = fastParse(row, amp1 + 7, amp2);
      int f = fastParse(row, amp2 + 6, len);
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int fastParse(String s, int begin, int end) {
    int val = 0;
    int i = begin;
    if (i < end && s.charAt(i) == '-') {
      i++;
      while (i < end) {
        val = val * 10 + (s.charAt(i) - '0');
        i++;
      }
      return -val;
    }
    while (i < end) {
      val = val * 10 + (s.charAt(i) - '0');
      i++;
    }
    return val;
  }
}