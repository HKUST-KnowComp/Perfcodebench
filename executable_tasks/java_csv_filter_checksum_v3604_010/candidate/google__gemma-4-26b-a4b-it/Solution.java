import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int b = 0;
            int c = 0;
            boolean hot = false;
            
            int i = 0;
            // Skip field 0
            while (i < len && row.charAt(i) != ',') i++;
            i++;
            
            // Skip field 1
            while (i < len && row.charAt(i) != ',') i++;
            i++;
            
            // Parse field 2 (int b)
            while (i < len && row.charAt(i) != ',') {
                b = b * 10 + (row.charAt(i) - '0');
                i++;
            }
            i++;
            
            // Parse field 3 (int c)
            while (i < len && row.charAt(i) != ',') {
                c = c * 10 + (row.charAt(i) - '0');
                i++;
            }
            i++;
            
            // Parse field 4 (boolean hot)
            // We check if the substring starting at i is "hot"
            if (i + 2 < len && row.charAt(i) == 'h' && row.charAt(i+1) == 'o' && row.charAt(i+2) == 't') {
                hot = true;
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}
