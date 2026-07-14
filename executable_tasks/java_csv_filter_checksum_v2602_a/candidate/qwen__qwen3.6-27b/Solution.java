import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int idx = 0;
      int commas = 0;
      while (commas < 2 && idx < len) {
        if (row.charAt(idx++) == ',') commas++;
      }
      if (idx >= len) continue;

      int bStart = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      int b = parse(row, bStart, idx);
      if (idx < len) idx++;

      int cStart = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      int c = parse(row, cStart, idx);
      if (idx < len) idx++;

      int hStart = idx;
      while (idx < len && row.charAt(idx) != ',') idx++;
      boolean hot = (idx - hStart == 3) && (row.charAt(hStart) == 'h') && (row.charAt(hStart + 1) == 'o') && (row.charAt(hStart + 2) == 't');

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parse(String s, int start, int end) {
    int val = 0;
    int i = start;
    boolean neg = false;
    if (i < end && s.charAt(i) == '-') {
      neg = true;
      i++;
    }
    for (; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return neg ? -val : val;
  }
}