import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long mul = 1099511628211L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      // Find indices of the first four commas
      int comma0 = row.indexOf(',');
      int comma1 = row.indexOf(',', comma0 + 1);
      int comma2 = row.indexOf(',', comma1 + 1);
      int comma3 = row.indexOf(',', comma2 + 1);
      // Parse b as int from substring between comma2 and comma3
      int b = parseInt(row, comma2 + 1, comma3);
      // Find comma4 (field end at next comma or end)
      int comma4 = row.indexOf(',', comma3 + 1);
      int end4 = (comma4 == -1) ? len : comma4;
      // Parse c as int from substring between comma3 and comma4
      int c = parseInt(row, comma3 + 1, end4);
      // Check if field4 equals "hot" (substring from comma4+1 to next comma or end)
      boolean hot = isHot(row, comma4 + 1, len);
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= mul;
    }
    return hash;
  }

  // Parse integer from substring [start, end) in string s
  private static int parseInt(String s, int start, int end) {
    if (start >= end) return 0; // safety
    boolean neg = false;
    int i = start;
    char c = s.charAt(i);
    if (c == '-') {
      neg = true;
      i++;
    } else if (c == '+') {
      i++;
    }
    int val = 0;
    while (i < end) {
      val = val * 10 + (s.charAt(i) - '0');
      i++;
    }
    return neg ? -val : val;
  }

  // Check if substring [start, end) equals "hot"
  private static boolean isHot(String s, int start, int end) {
    int len = end - start;
    if (len != 3) return false;
    return s.charAt(start) == 'h' && s.charAt(start + 1) == 'o' && s.charAt(start + 2) == 't';
  }
}