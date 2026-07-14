```java
import java.util.regex.Pattern;
import java.util.List;

public class Solution {
  private static final Pattern COMMA_PATTERN = Pattern.compile(",");
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] parts = COMMA_PATTERN.split(row);
      if (parts.length < 5) continue;
      int b;
      int c;
      try {
        b = Integer.parseInt(parts[2]);
        c = Integer.parseInt(parts[3]);
      } catch (NumberFormatException e) {
        continue;
      }
      boolean hot = parts[4].equals("hot");
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
```