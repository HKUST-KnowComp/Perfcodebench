import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET;
    final int size = rows.size();
    for (int r = 0; r < size; r++) {
      String s = rows.get(r);
      int len = s.length();
      int i = 2; // first number starts after 2-char prefix

      // Parse first integer (u)
      int sign = 1;
      if (i < len) {
        char c = s.charAt(i);
        if (c == '-') { sign = -1; i++; }
        else if (c == '+') { i++; }
      }
      int u = 0;
      while (i < len) {
        char c = s.charAt(i);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        i++;
      }
      u *= sign;

      // move past '&' to second segment start
      if (i < len && s.charAt(i) == '&') i++;

      // Second segment: skip 6-char prefix, then parse integer (s)
      i += 6; // assumes well-formed input as in baseline
      sign = 1;
      if (i < len) {
        char c = s.charAt(i);
        if (c == '-') { sign = -1; i++; }
        else if (c == '+') { i++; }
      }
      int second = 0;
      while (i < len) {
        char c = s.charAt(i);
        if (c == '&') break;
        second = second * 10 + (c - '0');
        i++;
      }
      second *= sign;

      // move past '&' to third segment start if present
      if (i < len && s.charAt(i) == '&') i++;

      // Third segment: skip 5-char prefix, then parse integer (f)
      i += 5; // assumes well-formed input as in baseline
      sign = 1;
      if (i < len) {
        char c = s.charAt(i);
        if (c == '-') { sign = -1; i++; }
        else if (c == '+') { i++; }
      }
      int f = 0;
      while (i < len) {
        char c = s.charAt(i);
        if (c == '&') break; // only the third segment content
        f = f * 10 + (c - '0');
        i++;
      }
      f *= sign;

      long v = (long)(u & 2047) + (long) second + (long) f * 19L;
      hash ^= (v + 1L);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}
