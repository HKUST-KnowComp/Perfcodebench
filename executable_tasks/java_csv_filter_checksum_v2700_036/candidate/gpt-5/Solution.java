import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV_OFFSET_BASIS;
    for (int r = 0, rn = rows.size(); r < rn; r++) {
      String s = rows.get(r);
      int len = s.length();

      int field = 0;
      int start = 0;
      int i = 0;

      int b = 0;
      int c = 0;
      boolean hot = false;

      while (true) {
        // find end of current field
        while (i < len && s.charAt(i) != ',') {
          i++;
        }
        int end = i;

        if (field == 2) {
          b = parseInt(s, start, end);
        } else if (field == 3) {
          c = parseInt(s, start, end);
        } else if (field == 4) {
          hot = isHot(s, start, end);
          // We have processed the needed fields; break early.
          break;
        }

        if (i == len) {
          // Reached end of line before field 4; break.
          break;
        }
        // move to next field
        i++; // skip comma
        start = i;
        field++;
      }

      long v = (b > 300 ? (long) (b + c) : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    if (start >= end) {
      throw new NumberFormatException();
    }
    int i = start;
    boolean neg = false;
    char ch = s.charAt(i);
    if (ch == '-') {
      neg = true;
      i++;
    } else if (ch == '+') {
      i++;
    }
    if (i >= end) {
      throw new NumberFormatException();
    }
    int val = 0;
    while (i < end) {
      int d = s.charAt(i) - '0';
      if ((d | 9 - d) < 0) { // fast range check: d<0 || d>9
        throw new NumberFormatException();
      }
      val = val * 10 + d;
      i++;
    }
    return neg ? -val : val;
  }

  private static boolean isHot(String s, int start, int end) {
    if (end - start != 3) return false;
    return s.charAt(start) == 'h' && s.charAt(start + 1) == 'o' && s.charAt(start + 2) == 't';
  }
}
