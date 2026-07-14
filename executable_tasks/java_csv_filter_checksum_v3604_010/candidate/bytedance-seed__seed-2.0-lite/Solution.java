import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long FNV_PRIME = 1099511628211L;
        for (String row : rows) {
            int commaCount = 0;
            int current = 0;
            int b = 0, c = 0;
            boolean isHot = false;
            int len = row.length();
            int i = 0;
            // Skip first two columns to reach the start of column 2 (b's column)
            while (i < len && commaCount < 2) {
                if (row.charAt(i) == ',') {
                    commaCount++;
                }
                i++;
            }
            // Parse integer value of b
            while (i < len) {
                char ch = row.charAt(i);
                if (ch == ',') {
                    commaCount++;
                    break;
                }
                current = current * 10 + (ch - '0');
                i++;
            }
            b = current;
            current = 0;
            // Parse integer value of c
            while (i < len) {
                char ch = row.charAt(i);
                if (ch == ',') {
                    commaCount++;
                    break;
                }
                current = current * 10 + (ch - '0');
                i++;
            }
            c = current;
            // Verify if column 4 is exactly "hot" to match original equals() logic
            if (i < len && row.charAt(i) == 'h') {
                if (i + 2 < len && row.charAt(i+1) == 'o' && row.charAt(i+2) == 't') {
                    if (i + 3 == len || row.charAt(i+3) == ',') {
                        isHot = true;
                    }
                }
            }
            // Exact same value and hash calculation as original implementation
            long v = (b > 300 ? (b + c) : 0) + (isHot ? 29 : 0);
            hash ^= (v + 1);
            hash *= FNV_PRIME;
        }
        return hash;
    }
}