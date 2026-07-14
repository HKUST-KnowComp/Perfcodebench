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
            // Field 0 & 1 (skip)
            while (i < len && commaCount < 2) {
                if (row.charAt(i) == ',') commaCount++;
                i++;
            }
            
            // Field 2 (int b)
            int start = i;
            while (i < len && row.charAt(i) != ',') i++;
            for (int j = start; j < i; j++) {
                b = b * 10 + (row.charAt(j) - '0');
            }
            if (i < len) i++; // skip comma
            commaCount++;

            // Field 3 (int c)
            start = i;
            while (i < len && row.charAt(i) != ',') i++;
            for (int j = start; j < i; j++) {
                c = c * 10 + (row.charAt(j) - '0');
            }
            if (i < len) i++; // skip comma
            commaCount++;

            // Field 4 (string hot)
            start = i;
            while (i < len) {
                if (row.charAt(i) == ',') break;
                i++;
            }
            // Check if field 4 is "hot"
            if (i - start == 3 && 
                row.charAt(start) == 'h' && 
                row.charAt(start + 1) == 'o' && 
                row.charAt(start + 2) == 't') {
                hot = true;
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}