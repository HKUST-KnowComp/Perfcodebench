import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int start = 0;
            
            int b = 0;
            int c = 0;
            boolean hot = false;

            // Manual scan to find indices of commas and parse values in one pass
            for (int i = 0; i <= len; i++) {
                if (i == len || row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 3) {
                        // Field index 2 (b)
                        b = parseFastInt(row, start, i);
                    } else if (commaCount == 4) {
                        // Field index 3 (c)
                        c = parseFastInt(row, start, i);
                    } else if (commaCount == 5) {
                        // Field index 4 (hot)
                        // Check if the substring is "hot"
                        int fieldLen = i - start;
                        if (fieldLen == 3 && 
                            row.charAt(start) == 'h' && 
                            row.charAt(start + 1) == 'o' && 
                            row.charAt(start + 2) == 't') {
                            hot = true;
                        }
                    }
                    start = i + 1;
                }
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseFastInt(String s, int start, int end) {
        int res = 0;
        for (int i = start; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return res;
    }
}