import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV_OFFSET_BASIS;
    for (String row : rows) {
      final int len = row.length();
      int idx = 5; // after "src=S"

      // parse src digits
      int src = 0;
      char c;
      while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') {
        src = src * 10 + (c - '0');
        idx++;
      }

      // skip space and "lat="
      idx++;       // space
      idx += 4;    // "lat="

      // parse lat digits
      int lat = 0;
      while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') {
        lat = lat * 10 + (c - '0');
        idx++;
      }

      // skip space and "code="
      idx++;       // space
      idx += 6;    // "code="

      // parse code digits
      int code = 0;
      while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') {
        code = code * 10 + (c - '0');
        idx++;
      }

      // skip space and "ok="
      idx++;       // space
      idx += 4;    // "ok="

      // determine ok: only "yes" starts with 'y'
      boolean ok = (row.charAt(idx) == 'y');

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }
    return hash;
  }
}