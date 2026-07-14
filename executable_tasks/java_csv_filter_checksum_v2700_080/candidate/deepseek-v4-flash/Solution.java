import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int commaCount = 0;
      int startB = -1, endB = -1;
      int startC = -1, endC = -1;
      int startHot = -1, endHot = -1;
      for (int i = 0; i < len; i++) {
        if (row.charAt(i) == ',') {
          commaCount++;
          if (commaCount == 2) {
            startB = i + 1;
          } else if (commaCount == 3) {
            endB = i;
            startC = i + 1;
          } else if (commaCount == 4) {
            endC = i;
            startHot = i + 1;
          } else if (commaCount == 5) {
            endHot = i;
            break;
          }
        }
      }
      if (endHot == -1) {
        endHot = len;
      }
      int b = parseInt(row, startB, endB);
      int c = parseInt(row, startC, endC);
      boolean hot = (endHot - startHot == 3) &&
                    row.charAt(startHot) == 'h' &&
                    row.charAt(startHot + 1) == 'o' &&
                    row.charAt(startHot + 2) == 't';
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int result = 0;
    int i = start;
    if (i < end && s.charAt(i) == '-') {
      // negative numbers not expected, but handle for correctness
      i++;
      for (; i < end; i++) {
        result = result * 10 + (s.charAt(i) - '0');
      }
      return -result;
    }
    for (; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return result;
  }
}