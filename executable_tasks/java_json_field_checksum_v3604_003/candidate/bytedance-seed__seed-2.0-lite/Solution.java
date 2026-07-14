import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            // Extract u value after "{\"u\":" (starts at index 5)
            int commaU = row.indexOf(',', 5);
            int u = Integer.parseInt(row.substring(5, commaU));
            // Extract s value after ",\"s\":" (starts at commaU + 5)
            int commaS = row.indexOf(',', commaU + 5);
            int s = Integer.parseInt(row.substring(commaU + 5, commaS));
            // Extract f boolean from first character after ",\"f\":" (starts at commaS +5)
            boolean f = row.charAt(commaS + 5) == 't';
            // Exact same value and hash calculation as original to preserve correctness
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}