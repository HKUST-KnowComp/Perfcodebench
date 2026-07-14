import java.util.List;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV_OFFSET;
    for (String row : rows) {
      // Parse u
      int uStart = row.indexOf("\"u\":") + 4;
      int uEnd = row.indexOf(',', uStart);
      int u = parseInt(row, uStart, uEnd);

      // Parse s
      int sStart = row.indexOf("\"s\":", uEnd) + 4;
      int sEnd = row.indexOf(',', sStart);
      int s = parseInt(row, sStart, sEnd);

      // Parse f
      int fStart = row.indexOf("\"f\":", sEnd) + 4;
      boolean f = row.charAt(fStart) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return val;
  }
}
