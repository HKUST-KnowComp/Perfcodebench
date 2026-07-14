import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      // locate commas for fields 2,3,4 (0‑based indices)
      int comma1 = row.indexOf(',');
      int comma2 = row.indexOf(',', comma1 + 1);
      int comma3 = row.indexOf(',', comma2 + 1);
      int comma4 = row.indexOf(',', comma3 + 1);
      int comma5 = row.indexOf(',', comma4 + 1);

      // parse field 2 (b)
      int b = parseInt(row, comma2 + 1, comma3);
      // parse field 3 (c)
      int c = parseInt(row, comma3 + 1, comma4);
      // check field 4 for "hot"
      int end4 = (comma5 == -1) ? len : comma5;
      int start4 = comma4 + 1;
      boolean hot = (end4 - start4 == 3)
          && row.charAt(start4) == 'h'
          && row.charAt(start4 + 1) == 'o'
          && row.charAt(start4 + 2) == 't';

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int val = 0;
    boolean neg = false;
    int i = start;
    if (s.charAt(i) == '-') {
      neg = true;
      i++;
    }
    for (; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return neg ? -val : val;
  }
}