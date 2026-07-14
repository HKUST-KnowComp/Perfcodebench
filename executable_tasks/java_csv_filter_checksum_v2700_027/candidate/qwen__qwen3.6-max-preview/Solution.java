import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int pos = 0;
            
            // Skip fields 0 and 1
            pos = row.indexOf(',', pos) + 1;
            pos = row.indexOf(',', pos) + 1;
            
            // Field 2 (b)
            int start2 = pos;
            pos = row.indexOf(',', pos) + 1;
            int end2 = pos - 1;
            
            // Field 3 (c)
            int start3 = pos;
            pos = row.indexOf(',', pos) + 1;
            int end3 = pos - 1;
            
            // Field 4 (hot)
            int start4 = pos;
            int end4 = row.indexOf(',', pos);
            if (end4 == -1) end4 = len;
            
            // Parse b manually
            int b = 0;
            boolean negB = false;
            if (start2 < end2 && row.charAt(start2) == '-') {
                negB = true;
                start2++;
            }
            for (int i = start2; i < end2; i++) {
                b = b * 10 + (row.charAt(i) - '0');
            }
            if (negB) b = -b;
            
            // Parse c manually
            int c = 0;
            boolean negC = false;
            if (start3 < end3 && row.charAt(start3) == '-') {
                negC = true;
                start3++;
            }
            for (int i = start3; i < end3; i++) {
                c = c * 10 + (row.charAt(i) - '0');
            }
            if (negC) c = -c;
            
            // Check "hot" exactly
            boolean hot = (end4 - start4 == 3) && 
                          row.charAt(start4) == 'h' && 
                          row.charAt(start4 + 1) == 'o' && 
                          row.charAt(start4 + 2) == 't';
                          
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}