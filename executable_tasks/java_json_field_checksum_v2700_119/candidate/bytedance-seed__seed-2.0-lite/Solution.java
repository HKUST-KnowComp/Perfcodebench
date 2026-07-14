import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            // Parse u field after fixed "{\"u\":" prefix
            int u = 0;
            int i = 5;
            char c;
            while ((c = row.charAt(i)) != ',') {
                u = u * 10 + (c - '0');
                i++;
            }
            // Skip past ",\"s\":" sequence to first digit of s
            i += 5;
            // Parse s field
            int s = 0;
            while ((c = row.charAt(i)) != ',') {
                s = s * 10 + (c - '0');
                i++;
            }
            // Skip past ",\"f\":" sequence to f's value
            i += 5;
            // f is true if first character is 't' per fixed input schema
            boolean f = row.charAt(i) == 't';
            // Exact original hash logic to preserve checksum correctness
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}