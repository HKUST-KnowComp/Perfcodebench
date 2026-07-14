import java.util.List;

public class Solution {
  private static final long OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = OFFSET_BASIS;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 0;

      // Skip field 0
      while (i < len && row.charAt(i) != ',') i++;
      if (i < len && row.charAt(i) == ',') i++;

      // Skip field 1
      while (i < len && row.charAt(i) != ',') i++;
      if (i < len && row.charAt(i) == ',') i++;

      // Field 2: parse int b
      int start = i;
      while (i < len && row.charAt(i) != ',') i++;
      int b = parseIntRange(row, start, i);

      // Field 3: parse int c
      if (i < len && row.charAt(i) == ',') i++;
      start = i;
      while (i < len && row.charAt(i) != ',') i++;
      int c = parseIntRange(row, start, i);

      // Field 4: check equals "hot"
      if (i < len && row.charAt(i) == ',') i++;
      start = i;
      while (i < len && row.charAt(i) != ',') i++;
      boolean hot = isHot(row, start, i);

      long v = (b > 300 ? (long) (b + c) : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  private static int parseIntRange(String s, int start, int end) {
    int i = start;
    boolean neg = false;
    if (i < end) {
      char ch = s.charAt(i);
      if (ch == '-') { neg = true; i++; }
      else if (ch == '+') { i++; }
    }
    int val = 0;
    for (; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return neg ? -val : val;
  }

  private static boolean isHot(String s, int start, int end) {
    // Check if the field equals exactly "hot"
    return (end - start) == 3
        && s.charAt(start) == 'h'
        && s.charAt(start + 1) == 'o'
        && s.charAt(start + 2) == 't';
  }
}
