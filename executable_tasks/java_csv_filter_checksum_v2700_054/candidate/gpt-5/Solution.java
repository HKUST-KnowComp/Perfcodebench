import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV_OFFSET_BASIS;
    for (String row : rows) {
      int n = row.length();
      int field = 0;
      int start = 0;
      int b = 0;
      int c = 0;
      boolean hot = false;

      for (int i = 0; i <= n; i++) {
        boolean atEnd = (i == n);
        if (atEnd || row.charAt(i) == ',') {
          int end = i;
          if (field == 2) {
            b = parseIntRange(row, start, end);
          } else if (field == 3) {
            c = parseIntRange(row, start, end);
          } else if (field == 4) {
            // Check if field equals "hot"
            int len = end - start;
            hot = (len == 3
                && row.charAt(start) == 'h'
                && row.charAt(start + 1) == 'o'
                && row.charAt(start + 2) == 't');

            long v = ((b > 300) ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= FNV_PRIME;
            break; // Done with this row; later fields are irrelevant
          }
          field++;
          start = i + 1;
        }
      }
    }
    return hash;
  }

  private static int parseIntRange(String s, int start, int end) {
    if (start >= end) {
      throw new NumberFormatException("Empty number");
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
      throw new NumberFormatException("Sign only");
    }
    int val = 0;
    while (i < end) {
      int d = s.charAt(i++) - '0';
      if (d < 0 || d > 9) {
        throw new NumberFormatException("Invalid digit");
      }
      val = val * 10 + d;
    }
    return neg ? -val : val;
  }
}