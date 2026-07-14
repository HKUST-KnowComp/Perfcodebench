import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Solution {
  private static final Pattern P = Pattern.compile("^\\{\\\"u\\\":(\\d+),\\\"s\\\":(\\d+),\\\"f\\\":(true|false),\\\"tag\\\":\\\"x\\d+\\\"\\}$");

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    Matcher m = P.matcher("")); // Compile matcher outside loop
    for (String row : rows) {
      m.reset(row);
      if (m.matches()) {
        int u = Integer.parseInt(m.group(1));
        int s = Integer.parseInt(m.group(2));
        boolean f = Boolean.parseBoolean(m.group(3)); // Directly parse boolean
        long v = (u & 1023) + s + (f ? 17 : 0);
        hash ^= (v + 1);
        hash *= 1099511628211L;
      } else {
        // Handle non-matching rows, e.g., throw an exception or log a warning
        throw new RuntimeException("Row does not match expected pattern: " + row);
      }
    }
    return hash;
  }
}