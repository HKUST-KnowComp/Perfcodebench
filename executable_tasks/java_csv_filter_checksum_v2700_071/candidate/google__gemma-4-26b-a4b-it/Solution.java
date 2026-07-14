import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int startIdx = 0;
            
            int b = 0;
            int c = 0;
            boolean hot = false;

            for (int i = 0; i <= len; i++) {
                if (i == len || row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 3) {
                        // Parse b
                        int val = 0;
                        for (int j = startIdx; j < i; j++) {
                            val = val * 10 + (row.charAt(j) - '0');
                        }
                        b = val;
                    } else if (commaCount == 4) {
                        // Parse c
                        int val = 0;
                        for (int j = startIdx; j < i; j++) {
                            val = val * 10 + (row.charAt(j) - '0');
                        }
                        c = val;
                    } else if (commaCount == 5) {
                        // Check hot
                        hot = (i - startIdx == 3 && 
                               row.charAt(startIdx) == 'h' && 
                               row.charAt(startIdx + 1) == 'o' && 
                               row.charAt(startIdx + 2) == 't');
                    }
                    startIdx = i + 1;
                }
            }
            
            // Handle case where last field might not end with comma
            // The loop above handles the last field if i == len

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}