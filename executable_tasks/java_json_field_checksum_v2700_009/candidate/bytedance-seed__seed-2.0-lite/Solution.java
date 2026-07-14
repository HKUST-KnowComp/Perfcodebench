import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long FNV_PRIME = 1099511628211L;
        for (String row : rows) {
            int len = row.length();
            int i = 5; // Skip past opening "{\"u\":" sequence to first digit of u
            
            // Parse u field value
            int u = 0;
            while (i < len) {
                char c = row.charAt(i);
                if (c == ',') break;
                u = u * 10 + (c - '0');
                i++;
            }
            
            // Skip past ",\"s\":" sequence to first digit of s
            i += 5;
            int s = 0;
            while (i < len) {
                char c = row.charAt(i);
                if (c == ',') break;
                s = s * 10 + (c - '0');
                i++;
            }
            
            // Skip past ",\"f\":" sequence to boolean value
            i += 5;
            boolean f = row.charAt(i) == 't';
            
            // Exact same checksum calculation as original baseline
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= FNV_PRIME;
        }
        return hash;
    }
}