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
      
      String part2 = row.substring(secondComma + 1, thirdComma);
      String part3 = row.substring(thirdComma + 1, fourthComma);
      String part4 = (fifthComma == -1) ? row.substring(fourthComma + 1) : row.substring(fourthComma + 1, fifthComma);
      
      int b = Integer.parseInt(part2);
      int c = Integer.parseInt(part3);
      boolean hot = part4.length() == 3 && part4.charAt(0) == 'h' && part4.charAt(1) == 'o' && part4.charAt(2) == 't';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}