import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_MULTIPLIER = 1099511628211L;
    
    for (String row : rows) {
      int commaIdx = 0;
      final int len = row.length();
      
      // Skip first two columns to reach parts[2] (b value)
      while (commaIdx < len && row.charAt(commaIdx) != ',') commaIdx++;
      commaIdx++;
      while (commaIdx < len && row.charAt(commaIdx) != ',') commaIdx++;
      
      // Parse b from parts[2]
      int bStart = ++commaIdx;
      while (commaIdx < len && row.charAt(commaIdx) != ',') commaIdx++;
      int b = 0;
      for (int i = bStart; i < commaIdx; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      
      // Parse c from parts[3]
      int cStart = ++commaIdx;
      while (commaIdx < len && row.charAt(commaIdx) != ',') commaIdx++;
      int c = 0;
      for (int i = cStart; i < commaIdx; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      
      // Check if parts[4] equals "hot" via direct char comparison
      boolean hot = false;
      if (commaIdx + 2 < len) {
        if (row.charAt(commaIdx + 1) == 'h' &&
            row.charAt(commaIdx + 2) == 'o' &&
            row.charAt(commaIdx + 3) == 't') {
          hot = true;
        }
      }
      
      // Exact same hash computation as baseline to preserve correctness
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= FNV_MULTIPLIER;
    }
    return hash;
  }
}