import java.util.*;

public class Solution {

    public static long run(List<String> rows) {
        long hash = 146959810394665603L; // Corrected FNV offset basis for 64-bit
        // Note: The original code used 1469598103934665603L. 
        // To ensure exact correctness with the baseline, I must use the exact constant provided.
        hash = 1469598103934665603L;

        for (String row : rows) {
            int len = row.length();
            
            // Manual parsing to avoid Regex overhead
            // Format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
            
            // 1. src
            // Skip 'src=S'
            int idx = 4;
            long src = 0;
            while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
                src = src * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            
            // 2. lat
            // Skip ' lat='
            idx += 5;
            long lat = 0;
            while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
                lat = lat * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            
            // 3. code
            // Skip ' code='
            idx += 7;
            long code = 0;
            while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
                code = code * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            
            // 4. ok
            // Skip ' ok='
            idx += 5;
            boolean ok = false;
            if (idx < len && row.charAt(idx) == 'y') {
                // Check for 'yes'
                if (idx + 2 < len && row.charAt(idx+1) == 'e' && row.charAt(idx+2) == 's') {
                    ok = true;
                }
            }

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}