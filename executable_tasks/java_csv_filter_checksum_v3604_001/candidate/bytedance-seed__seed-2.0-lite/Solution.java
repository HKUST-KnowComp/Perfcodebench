import java.util.List;

public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;
    private static final int HOT_BONUS = 29;

    public static long run(List<String> rows) {
        long hash = FNV_OFFSET;
        for (String row : rows) {
            // Locate only the commas needed to extract required columns
            int comma1 = row.indexOf(',');
            int comma2 = row.indexOf(',', comma1 + 1);
            int comma3 = row.indexOf(',', comma2 + 1);
            int comma4 = row.indexOf(',', comma3 + 1);

            // Parse integer fields with minimal overhead
            int b = parseInt(row, comma2 + 1, comma3);
            int c = parseInt(row, comma3 + 1, comma4);

            // Check for 'hot' without creating a substring
            boolean hot = false;
            int hotStart = comma4 + 1;
            if (row.length() >= hotStart + 3) {
                if (row.charAt(hotStart) == 'h' &&
                    row.charAt(hotStart + 1) == 'o' &&
                    row.charAt(hotStart + 2) == 't' &&
                    (row.length() == hotStart + 3 || row.charAt(hotStart + 3) == ',')) {
                    hot = true;
                }
            }

            // Exact same calculation as baseline to preserve checksum correctness
            long v = (b > 300 ? (b + c) : 0) + (hot ? HOT_BONUS : 0);
            hash ^= (v + 1);
            hash *= FNV_PRIME;
        }
        return hash;
    }

    /** Lightweight parser for positive integers to avoid Integer.parseInt overhead */
    private static int parseInt(String s, int start, int end) {
        int value = 0;
        for (int i = start; i < end; i++) {
            value = value * 10 + (s.charAt(i) - '0');
        }
        return value;
    }
}