import java.util.*;

public class Solution {

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] chars = row.toCharArray();
            int n = chars.length;
            
            // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
            // We scan manually to avoid Regex overhead
            
            int i = 0;
            // Skip 'src=S'
            i += 5;
            
            // Parse src
            int src = 0;
            while (i < n && chars[i] >= '0' && chars[i] <= '9') {
                src = src * 10 + (chars[i] - '0');
                i++;
            }
            
            // Skip ' lat='
            i += 5;
            
            // Parse lat
            int lat = 0;
            while (i < n && chars[i] >= '0' && chars[i] <= '9') {
                lat = lat * 10 + (chars[i] - '0');
                i++;
            }
            
            // Skip ' code='
            i += 7;
            
            // Parse code
            int code = 0;
            while (i < n && chars[i] >= '0' && chars[i] <= '9') {
                code = code * 10 + (chars[i] - '0');
                i++;
            }
            
            // Skip ' ok='
            i += 5;
            
            // Parse ok (yes/no)
            boolean ok = false;
            if (i < n && chars[i] == 'y') {
                ok = true;
                i += 3; // skip 'yes'
            } else {
                i += 2; // skip 'no'
            }

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}