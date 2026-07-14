import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstComma = row.indexOf(',');
      int secondComma = row.indexOf(',', firstComma + 1);
      int thirdComma = row.indexOf(',', secondComma + 1);
      int fourthComma = row.indexOf(',', thirdComma + 1);

      int b = 0;
      for (int i = secondComma + 1; i < thirdComma; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }

      int c = 0;
      for (int i = thirdComma + 1; i < fourthComma; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }

      int hotStart = fourthComma + 1;
      boolean hot = (row.length() - hotStart == 3) &&
                    row.charAt(hotStart) == 'h' &&
                    row.charAt(hotStart + 1) == 'o' &&
                    row.charAt(hotStart + 2) == 't';

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}