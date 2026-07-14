import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 146959810394665603L; // Corrected constant from baseline if needed, but keeping baseline logic
        // Re-aligning to baseline exact constant: 1469598103934665603L
        hash = 1469598103934665603L;

        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            // Manual parsing to avoid split() and substring()
            // Part 0: u (starts at index 2, ends at first '&')
            int ampersand1 = row.indexOf('&');
            int u = 0;
            for (int j = 2; j < ampersand1; j++) {
                u = u * 10 + (row.charAt(j) - '0');
            }

            // Part 1: s (starts at index ampersand1 + 7, ends at second '&')
            int ampersand2 = row.indexOf('&', ampersand1 + 1);
            int s = 0;
            for (int j = ampersand1 + 7; j < ampersand2; j++) {
                s = s * 10 + (row.charAt(j) - '0');
            }

            // Part 2: f (starts at index ampersand2 + 6)
            int f = 0;
            for (int j = ampersand2 + 6; j < len; j++) {
                f = f * 10 + (row.charAt(j) - '0');
            }

            long v = (u & 2047) + s + (long) f * 19L;
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}