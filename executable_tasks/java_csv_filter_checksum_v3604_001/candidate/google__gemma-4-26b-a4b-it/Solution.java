import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            int commaCount = 0;
            int pos = 0;
            int b = 0;
            int c = 0;
            boolean hot = false;

            // Manual scan to find indices of commas and parse values
            // We need parts[2], parts[3], and parts[4]
            // parts[0], parts[1], parts[2], parts[3], parts[4]
            
            int currentComma = 0;
            int start = 0;
            
            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    currentComma++;
                    if (currentComma == 2) {
                        // Start of parts[2]
                        start = j + 1;
                    } else if (currentComma == 3) {
                        // End of parts[2], start of parts[3]
                        b = parseFastInt(row, start, j);
                        start = j + 1;
                    } else if (currentComma == 4) {
                        // End of parts[3], start of parts[4]
                        c = parseFastInt(row, start, j);
                        start = j + 1;
                    }
                }
            }
            
            // Handle parts[4] (after the 4th comma or if it's the last part)
            // The loop above handles up to the 4th comma. 
            // We need to check if there's a 4th comma to find the start of parts[4]
            int fourthCommaIdx = -1;
            int count = 0;
            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    count++;
                    if (count == 4) {
                        fourthCommaIdx = j;
                        break;
                    }
                }
            }
            
            if (fourthCommaIdx != -1) {
                // Check if parts[4] is "hot"
                // Optimization: instead of equals, check first 3 chars if length allows
                int p4Start = fourthCommaIdx + 1;
                if (row.startsWith("hot", p4Start)) {
                    hot = true;
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
        int i = start;
        boolean neg = false;
        if (i < end && s.charAt(i) == '-') {
            neg = true;
            i++;
        }
        for (; i < end; i++) {
            res = res * 10 + (s.charAt(i) - '0');
        }
        return neg ? -res : res;
    }
}