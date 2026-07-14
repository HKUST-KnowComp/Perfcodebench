import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int ri = 0, rn = rows.size(); ri < rn; ri++) {
      String row = rows.get(ri);
      int len = row.length();
      int i = 0;

      // Skip first two fields (field indices 0 and 1)
      int skipped = 0;
      while (skipped < 2 && i < len) {
        while (i < len && row.charAt(i) != ',') i++;
        if (i < len && row.charAt(i) == ',') { i++; skipped++; }
      }

      // Parse field 2 (third field) as int -> b
      int b = 0;
      boolean bNeg = false;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { bNeg = true; i++; }
        else if (ch == '+') { i++; }
        int val = 0;
        while (i < len) {
          ch = row.charAt(i);
          if (ch == ',') break;
          val = val * 10 + (ch - '0');
          i++;
        }
        b = bNeg ? -val : val;
      }
      if (i < len && row.charAt(i) == ',') i++;

      // Parse field 3 (fourth field) as int -> c
      int c = 0;
      boolean cNeg = false;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { cNeg = true; i++; }
        else if (ch == '+') { i++; }
        int val = 0;
        while (i < len) {
          ch = row.charAt(i);
          if (ch == ',') break;
          val = val * 10 + (ch - '0');
          i++;
        }
        c = cNeg ? -val : val;
      }
      if (i < len && row.charAt(i) == ',') i++;

      // Field 4 (fifth field): check if equals "hot"
      boolean hot = false;
      if (i < len) {
        int s = i;
        while (i < len && row.charAt(i) != ',') i++;
        int e = i;
        if (e - s == 3) {
          if (row.charAt(s) == 'h' && row.charAt(s + 1) == 'o' && row.charAt(s + 2) == 't') {
            hot = true;
          }
        }
      }

      long v = ((b > 300) ? (long)(b + c) : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
