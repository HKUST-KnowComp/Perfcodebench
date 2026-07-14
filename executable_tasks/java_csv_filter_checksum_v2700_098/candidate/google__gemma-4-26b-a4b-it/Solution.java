import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 146959810394665603L; // Corrected FNV offset basis for 64-bit if needed, but keeping original logic
        // Re-aligning to original constant provided in baseline to ensure exact checksum match
        hash = 1469598103934665603L;

        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            // We need parts[2], parts[3], parts[4]
            // Indices of commas: 
            // part0,part1,part2,part3,part4...
            // comma0 is after part0
            // comma1 is after part1
            // comma2 is after part2
            // comma3 is after part3
            
            int c0 = -1, c1 = -1, c2 = -1, c3 = -1;
            int count = 0;
            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    count++;
                    if (count == 1) c0 = j;
                    else if (count == 2) c1 = j;
                    else if (count == 3) c2 = j;
                    else if (count == 4) c3 = j;
                }
            }

            // parts[2] is between c1 and c2
            int b = 0;
            for (int j = c1 + 1; j < c2; j++) {
                b = b * 10 + (row.charAt(j) - '0');
            }

            // parts[3] is between c2 and c3
            int c = 0;
            for (int j = c2 + 1; j < c3; j++) {
                c = c * 10 + (row.charAt(j) - '0');
            }

            // parts[4] starts at c3 + 1
            boolean hot = false;
            if (c3 + 1 < len) {
                // Check if parts[4] starts with 'hot'
                if (row.charAt(c3 + 1) == 'h' && 
                    (c3 + 2 >= len || row.charAt(c3 + 2) == 'o') && 
                    (c3 + 3 >= len || row.charAt(c3 + 3) == 't')) {
                    // Verify it's exactly "hot" or starts with it? 
                    // The original used .equals("hot"), so we check bounds/next char
                    if (c3 + 4 == len || row.charAt(c3 + 4) == ',') {
                        hot = true;
                    }
                }
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}