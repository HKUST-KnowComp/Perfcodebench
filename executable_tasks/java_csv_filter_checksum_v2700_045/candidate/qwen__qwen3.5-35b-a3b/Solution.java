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
      
      boolean hot = (row.length() - fourthComma - 1 == 3) &&
                    row.charAt(fourthComma + 1) == 'h' &&
                    row.charAt(fourthComma + 2) == 'o' &&
                    row.charAt(fourthComma + 3) == 't';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInteger(String s, int start, int end) {
    int num = 0;
    int sign = 1;
    int i = start;
    if (s.charAt(i) == '-') {
      sign = -1;
      i++;
    }
    for (; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num * sign;
  }
}