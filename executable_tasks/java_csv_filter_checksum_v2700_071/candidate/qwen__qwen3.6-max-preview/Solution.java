import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int i = 0;
            
            // Skip field 0
            while (i < len && row.charAt(i) != ',') i++;
            i++;
            // Skip field 1
            while (i < len && row.charAt(i) != ',') i++;
            i++;
            
            // Parse field 2 (b)
            int b = 0;
            boolean negB = false;
            if (i < len && row.charAt(i) == '-') { negB = true; i++; }
            while (i < len && row.charAt(i) != ',') {
                b = b * 10 + (row.charAt(i) - '0');
                i++;
            }
            if (negB) b = -b;
            i++;
            
            // Parse field 3 (c)
            int c = 0;
            boolean negC = false;
            if (i < len && row.charAt(i) == '-') { negC = true; i++; }
            while (i < len && row.charAt(i) != ',') {
                c = c * 10 + (row.charAt(i) - '0');
                i++;
            }
            if (negC) c = -c;
            i++;
            
            // Check field 4 for "hot"
            boolean hot = (i + 3 <= len && 
                           row.charAt(i) == 'h' && 
                           row.charAt(i + 1) == 'o' && 
                           row.charAt(i + 2) == 't' && 
                           (i + 3 == len || row.charAt(i + 3) == ','));
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}
