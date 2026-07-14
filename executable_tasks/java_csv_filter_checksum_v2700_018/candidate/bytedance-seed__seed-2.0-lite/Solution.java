import java.util.*;

public class Solution {
  private static int parseIntBetween(String s, int start, int end) {
    if (start >= end) throw new NumberFormatException();
    int result = 0;
    int sign = 1;
    int i = start;
    if (s.charAt(i) == '-') {
      sign = -1;
      i++;
      if (i >= end) throw new NumberFormatException();
    }
    while (i < end) {
      char c = s.charAt(i);
      if (c < '0' || c > '9') throw new NumberFormatException();
      result = result * 10 + (c - '0');
      i++;
    }
    return result * sign;
  }

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_MULTIPLIER = 1099511628211L;
    for (String row : rows) {
      int commaCount = 0;
      int pos2 = -1, pos3 = -1, pos4 = -1;
      int len = row.length();
      for (int i = 0; i < len; i++) {
        if (row.charAt(i) == ',') {
          commaCount++;
          if (commaCount == 2) pos2 = i;
          else if (commaCount == 3) pos3 = i;
          else if (commaCount == 4) {
            pos4 = i;
            break;
          }
        }
      }
      int b = parseIntBetween(row, pos2 + 1, pos3);
      int c = parseIntBetween(row, pos3 + 1, pos4);
      boolean hot = false;
      int hotStart = pos4 + 1;
      if (hotStart + 2 < len) {
        if (row.charAt(hotStart) == 'h' && row.charAt(hotStart+1) == 'o' && row.charAt(hotStart+2) == 't') {
          if (hotStart + 3 == len || row.charAt(hotStart + 3) == ',') {
            hot = true;
          }
        }
      }
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= FNV_MULTIPLIER;
    }
    return hash;
  }
}