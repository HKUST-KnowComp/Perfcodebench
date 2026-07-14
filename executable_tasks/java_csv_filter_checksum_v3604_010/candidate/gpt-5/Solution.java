import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();

      int b = 0;
      int c = 0;
      boolean hot = false;

      int field = 0; // 0-based field index
      int start = 0;

      // Scan characters, process fields 2,3,4 and stop early once field 4 is processed
      for (int i = 0; ; i++) {
        boolean endField = (i == len) || (row.charAt(i) == ',');
        if (endField) {
          if (field == 2) {
            // parse int for b from [start, i)
            int idx = start;
            boolean neg = false;
            if (idx < i) {
              char ch = row.charAt(idx);
              if (ch == '-') { neg = true; idx++; }
              else if (ch == '+') { idx++; }
            }
            int val = 0;
            for (; idx < i; idx++) {
              val = val * 10 + (row.charAt(idx) - '0');
            }
            if (neg) val = -val;
            b = val;
          } else if (field == 3) {
            // parse int for c from [start, i)
            int idx = start;
            boolean neg = false;
            if (idx < i) {
              char ch = row.charAt(idx);
              if (ch == '-') { neg = true; idx++; }
              else if (ch == '+') { idx++; }
            }
            int val = 0;
            for (; idx < i; idx++) {
              val = val * 10 + (row.charAt(idx) - '0');
            }
            if (neg) val = -val;
            c = val;
          } else if (field == 4) {
            // check equals "hot" for [start, i)
            int fl = i - start;
            hot = (fl == 3
                   && row.charAt(start) == 'h'
                   && row.charAt(start + 1) == 'o'
                   && row.charAt(start + 2) == 't');
            // We have all required fields; stop scanning the row
            break;
          }
          field++;
          if (i == len) {
            // End of string
            break;
          }
          start = i + 1;
        }
      }

      // Compute value using int semantics first, then widen to long to match baseline
      int vi = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      long v = (long) vi;

      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}