import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    int n = rows.size();
    for (int idx = 0; idx < n; idx++) {
      String row = rows.get(idx);
      int len = row.length();

      // Find first and second '&'
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);

      // Parse u from after "u=" (index 2) up to first '&'
      int u = fastParseInt(row, 2, amp1);

      // Parse s from after "score=" (6 chars) following the first '&'
      int sStart = amp1 + 1 + 6; // skip '&' and "score="
      int s = fastParseInt(row, sStart, amp2);

      // Parse f from after "flag=" (5 chars) following the second '&'
      int fStart = amp2 + 1 + 5; // skip '&' and "flag="
      int f = fastParseInt(row, fStart, len);

      long v = (u & 2047) + (long) s + (long) f * 19L;
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }

  private static int fastParseInt(String s, int start, int end) {
    int i = start;
    if (i >= end) return 0;
    boolean neg = false;
    char c = s.charAt(i);
    if (c == '-' || c == '+') {
      neg = (c == '-');
      i++;
    }
    int val = 0;
    while (i < end) {
      val = val * 10 + (s.charAt(i) - '0');
      i++;
    }
    return neg ? -val : val;
  }
}