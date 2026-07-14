import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstComma = row.indexOf(',');
      if (firstComma == -1) throw new RuntimeException("Invalid row");
      int secondComma = row.indexOf(',', firstComma + 1);
      if (secondComma == -1) throw new RuntimeException("Invalid row");
      int thirdComma = row.indexOf(',', secondComma + 1);
      if (thirdComma == -1) throw new RuntimeException("Invalid row");
      int fourthComma = row.indexOf(',', thirdComma + 1);
      if (fourthComma == -1) throw new RuntimeException("Invalid row");

      int b = 0;
      int startB = secondComma + 1;
      int endB = thirdComma;
      boolean negativeB = false;
      if (row.charAt(startB) == '-') {
        negativeB = true;
        startB++;
      }
      for (int i = startB; i < endB; i++) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') throw new NumberFormatException("Invalid integer");
        b = b * 10 + (c - '0');
      }
      if (negativeB) b = -b;

      int c = 0;
      int startC = thirdComma + 1;
      int endC = fourthComma;
      boolean negativeC = false;
      if (row.charAt(startC) == '-') {
        negativeC = true;
        startC++;
      }
      for (int i = startC; i < endC; i++) {
        char cChar = row.charAt(i);
        if (cChar < '0' || cChar > '9') throw new NumberFormatException("Invalid integer");
        c = c * 10 + (cChar - '0');
      }
      if (negativeC) c = -c;

      boolean hot = false;
      int startHot = fourthComma + 1;
      int endHot = row.length();
      if (endHot - startHot == 3 &&
          row.charAt(startHot) == 'h' &&
          row.charAt(startHot + 1) == 'o' &&
          row.charAt(startHot + 2) == 't') {
        hot = true;
      }

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}