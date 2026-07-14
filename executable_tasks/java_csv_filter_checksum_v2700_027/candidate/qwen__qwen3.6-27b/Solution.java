import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int field = 0;
      int start = 0;
      int bStart = 0, bEnd = 0;
      int cStart = 0, cEnd = 0;
      int f4Start = 0, f4End = 0;

      for (int i = 0; i < len; i++) {
        if (row.charAt(i) == ',') {
          if (field == 2) { bStart = start; bEnd = i; }
          else if (field == 3) { cStart = start; cEnd = i; }
          else if (field == 4) { f4Start = start; f4End = i; }
          start = i + 1;
          field++;
        }
      }
      if (field == 4) { f4Start = start; f4End = len; }

      int b = 0;
      int i = bStart;
      if (row.charAt(i) == '-') { i++; while (i < bEnd) { b = b * 10 - (row.charAt(i++) - '0'); } }
      else { while (i < bEnd) { b = b * 10 + (row.charAt(i++) - '0'); } }

      int c = 0;
      i = cStart;
      if (row.charAt(i) == '-') { i++; while (i < cEnd) { c = c * 10 - (row.charAt(i++) - '0'); } }
      else { while (i < cEnd) { c = c * 10 + (row.charAt(i++) - '0'); } }

      boolean hot = (f4End - f4Start == 3) &&
                    row.charAt(f4Start) == 'h' &&
                    row.charAt(f4Start + 1) == 'o' &&
                    row.charAt(f4Start + 2) == 't';

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}