import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            // Locate space separators for fixed-format log line
            int sp1 = row.indexOf(' ');
            int sp2 = row.indexOf(' ', sp1 + 1);
            int sp3 = row.indexOf(' ', sp2 + 1);
            
            // Parse src (after "src=S" prefix, ends at first space)
            int src = 0;
            for (int i = 5; i < sp1; i++) {
                src = src * 10 + (row.charAt(i) - '0');
            }
            
            // Parse lat (after " lat=" prefix, ends at second space)
            int lat = 0;
            for (int i = sp1 + 5; i < sp2; i++) {
                lat = lat * 10 + (row.charAt(i) - '0');
            }
            
            // Parse code (after " code=" prefix, ends at third space)
            int code = 0;
            for (int i = sp2 + 6; i < sp3; i++) {
                code = code * 10 + (row.charAt(i) - '0');
            }
            
            // Check ok flag from first character of value
            boolean ok = row.charAt(sp3 + 4) == 'y';
            
            // Exact same hash calculation as original to preserve correctness
            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}