import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            // Manual scan to find indices of commas to avoid split()
            // parts[2] is the 3rd field, parts[3] is the 4th, parts[4] is the 5th
            int comma1 = -1;
            int comma2 = -1;
            int comma3 = -1;
            int comma4 = -1;
            
            int count = 0;
            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    count++;
                    if (count == 1) comma1 = j;
                    else if (count == 2) comma2 = j;
                    else if (count == 3) comma3 = j;
                    else if (count == 4) comma4 = j;
                }
            }

            // Field 2 (index 2): between comma2 and comma3
            int b = parseFastInt(row, comma2 + 1, comma3);
            // Field 3 (index 3): between comma3 and comma4
            int c = parseFastInt(row, comma3 + 1, comma4);
            
            // Field 4 (index 4): from comma4 to end
            boolean hot = false;
            if (comma4 != -1) {
                hot = isHot(row, comma4 + 1);
            } else {
                // If there are fewer than 4 commas, the logic depends on the input structure.
                // Based on the original split, parts[4] requires at least 4 commas.
                // However, the original code would throw ArrayIndexOutOfBoundsException if not present.
                // We assume the input follows the expected format.
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

    private static boolean isHot(String s, int start) {
        int len = s.length();
        // Check if substring starting at 'start' is "hot"
        if (start + 3 > len) return false;
        return s.charAt(start) == 'h' && 
               s.charAt(start + 1) == 'o' && 
               s.charAt(start + 2) == 't';
    }
}