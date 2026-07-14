import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstComma = row.indexOf(',');
      int secondComma = row.indexOf(',', firstComma + 1);
      int thirdComma = row.indexOf(',', secondComma + 1);
      int fourthComma = row.indexOf(',', thirdComma + 1);
      int fifthComma = row.indexOf(',', fourthComma + 1);
      
      int start2 = secondComma + 1;
      int end2 = thirdComma;
      int start3 = thirdComma + 1;
      int end3 = fourthComma;
      int start4 = fourthComma + 1;
      int end4 = (fifthComma == -1) ? row.length() : fifthComma;
      
      int b = parseIntSubstring(row, start2, end2);
      int c = parseIntSubstring(row, start3, end3);
      boolean hot = equalsHot(row, start4, end4);
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseIntSubstring(String s, int start, int end) {
    int result = 0;
    boolean negative = false;
    int i = start;
    if (i < end && s.charAt(i) == '-') {
      negative = true;
      i++;
    }
    for (; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return negative ? -result : result;
  }
  
  private static boolean equalsHot(String s, int start, int end) {
    int len = end - start;
    if (len != 3) return false;
    return s.charAt(start) == 'h' && s.charAt(start+1) == 'o' && s.charAt(start+2) == 't';
  }
}