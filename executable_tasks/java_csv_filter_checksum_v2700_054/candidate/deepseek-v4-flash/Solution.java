import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int fieldCount = 0;
      int start = 0;
      int b = 0, c = 0;
      boolean hot = false;
      for (int i = 0; i <= len; i++) {
        if (i == len || row.charAt(i) == ',') {
          if (fieldCount == 2) {
            b = parseInt(row, start, i);
          } else if (fieldCount == 3) {
            c = parseInt(row, start, i);
          } else if (fieldCount == 4) {
            int flen = i - start;
            hot = (flen == 3 && row.charAt(start) == 'h' && row.charAt(start+1) == 'o' && row.charAt(start+2) == 't');
            break;
          }
          start = i + 1;
          fieldCount++;
        }
      }
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int value = 0;
    boolean negative = false;
    if (start < end && s.charAt(start) == '-') {
      negative = true;
      start++;
    }
    for (int i = start; i < end; i++) {
      char c = s.charAt(i);
      if (c >= '0' && c <= '9') {
        value = value * 10 + (c - '0');
      } else {
        throw new NumberFormatException("For input string: \"" + s.substring(start, end) + "\"");
      }
    }
    return negative ? -value : value;
  }
}