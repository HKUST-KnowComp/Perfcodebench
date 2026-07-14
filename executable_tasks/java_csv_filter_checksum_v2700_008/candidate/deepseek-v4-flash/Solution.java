import java.util.*;

public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(List<String> rows) {
        long hash = FNV_OFFSET;
        for (String row : rows) {
            int len = row.length();
            // locate the positions of the first five commas (fields 0..4)
            int comma1 = row.indexOf(',');
            int comma2 = row.indexOf(',', comma1 + 1);
            int comma3 = row.indexOf(',', comma2 + 1);
            int comma4 = row.indexOf(',', comma3 + 1);
            // end of fifth field (field index 4): either next comma or end of string
            int comma5 = row.indexOf(',', comma4 + 1);
            int end5 = (comma5 == -1) ? len : comma5;

            // parse b (third field) and c (fourth field) as integers
            int b = parseInt(row, comma2 + 1, comma3);
            int c = parseInt(row, comma3 + 1, comma4);

            // check if the fifth field is exactly "hot"
            boolean hot = (end5 - (comma4 + 1) == 3)
                    && row.charAt(comma4 + 1) == 'h'
                    && row.charAt(comma4 + 2) == 'o'
                    && row.charAt(comma4 + 3) == 't';

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= FNV_PRIME;
        }
        return hash;
    }

    private static int parseInt(String s, int start, int end) {
        int val = 0;
        boolean neg = false;
        if (s.charAt(start) == '-') {
            neg = true;
            start++;
        }
        for (int i = start; i < end; i++) {
            val = val * 10 + (s.charAt(i) - '0');
        }
        return neg ? -val : val;
    }
}