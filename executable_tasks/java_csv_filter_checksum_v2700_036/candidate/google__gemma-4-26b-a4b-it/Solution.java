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

            for (int i = 0; i <= len; i++) {
                if (i == len || row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 3) {
                        // Field 2 (index 2)
                        b = parseFast(row, start, i);
                    } else if (commaCount == 4) {
                        // Field 3 (index 3)
                        c = parseFast(row, start, i);
                    } else if (commaCount == 5) {
                        // Field 4 (index 4) - check for "hot"
                        // We check if the substring is exactly "hot"
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

    private static int parseFast(String s, int start, int end) {
        int res = 0;
        for (int i = start; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return res;
    }
}