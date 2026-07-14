import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstComma = row.indexOf(',');
      int secondComma = row.indexOf(',', firstComma + 1);
      int thirdComma = row.indexOf(',', secondComma + 1);
      int fourthComma = row.indexOf(',', thirdComma + 1);

      int b = parseInteger(row, secondComma + 1, thirdComma);
      int c = parseInteger(row, thirdComma + 1, fourthComma);
      boolean hot = isHot(row, fourthComma + 1);

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInteger(String s, int start, int end) {
    int num = 0;
    boolean negative = false;
    int i = start;
    if (s.charAt(i) == '-') {
      negative = true;
      i++;
    }
    while (i < end) {
      char c = s.charAt(i);
      if (c < '0' || c > '9') break;
      num = num * 10 + (c - '0');
      i++;
    }
    return negative ? -num : num;
  }

  private static boolean isHot(String s, int start) {
    int len = s.length() - start;
    if (len != 3) return false;
    return s.charAt(start) == 'h' &&
           s.charAt(start + 1) == 'o' &&
           s.charAt(start + 2) == 't';
  }
}