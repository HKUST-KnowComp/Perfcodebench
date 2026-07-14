import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Solution {
  private static final Pattern P = Pattern.compile("^\\{\\\"u\\\":(\\d+),\\\"s\\\":(\\d+),\\\"f\\\":(true|false),\\\"tag\\\":\\\"x\\d+\\\"\\}$");

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      if (row == null || !P.matcher(row).matches()) {
        throw new RuntimeException("Invalid input row: " + row);
      }
      int u = getInt(row, 1);
      int s = getInt(row, 2);
      boolean f = getBoolean(row, 3);
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int getInt(String row, int group) {
    int start = row.indexOf('"u":') + 4;
    int end = row.indexOf(',', start);
    if (group == 2) {
      start = row.indexOf('"s":') + 4;
      end = row.indexOf(',', start);
    }
    return Integer.parseInt(row.substring(start, end));
  }

  private static boolean getBoolean(String row, int group) {
    int start = row.indexOf('"f":') + 4;
    int end = row.indexOf(',', start);
    return row.substring(start, end).equals("true");
  }
}