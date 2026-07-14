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
      
      int b = Integer.parseInt(row.substring(secondComma + 1, thirdComma));
      int c = Integer.parseInt(row.substring(thirdComma + 1, fourthComma));
      int end4 = (fifthComma == -1) ? row.length() : fifthComma;
      String hotField = row.substring(fourthComma + 1, end4);
      boolean hot = hotField.equals("hot");
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}