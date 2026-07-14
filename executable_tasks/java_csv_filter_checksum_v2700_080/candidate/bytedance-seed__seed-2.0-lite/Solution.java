import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            final int len = row.length();
            int commaCount = 0;
            int bStart = 0, bEnd = 0, cStart = 0, cEnd = 0, hotStart = 0;
            
            // Scan only until we have all required field positions, break early
            for (int i = 0; i < len; i++) {
                if (row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 2) {
                        bStart = i + 1;
                    } else if (commaCount == 3) {
                        bEnd = i;
                        cStart = i + 1;
                    } else if (commaCount == 4) {
                        cEnd = i;
                        hotStart = i + 1;
                        break; // No need to process rest of the row
                    }
                }
            }
            
            // Parse integers directly from the original string, no substrings
            int b = parse(row, bStart, bEnd);
            int c = parse(row, cStart, cEnd);
            
            // Check if 5th field is exactly "hot" without string allocation
            boolean hot = false;
            if (hotStart + 2 < len) { // Ensure minimum length for "hot"
                if (row.charAt(hotStart) == 'h' && row.charAt(hotStart + 1) == 'o' && row.charAt(hotStart + 2) == 't') {
                    // Confirm end of field (either end of string or next comma)
                    if (hotStart + 3 >= len || row.charAt(hotStart + 3) == ',') {
                        hot = true;
                    }
                }
            }
            
            // Exact same calculation as baseline to preserve checksum
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
    
    private static int parse(String s, int start, int end) {
        int num = 0;
        boolean negative = s.charAt(start) == '-';
        int i = negative ? start + 1 : start;
        for (; i < end; i++) {
            num = num * 10 + (s.charAt(i) - '0');
        }
        return negative ? -num : num;
    }
}