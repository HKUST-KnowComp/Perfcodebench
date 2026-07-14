import java.util.List;

public class Solution {
    private static int parseInt(String s, int start, int end) {
        int num = 0;
        for (int i = start; i < end; i++) {
            num = num * 10 + (s.charAt(i) - '0');
        }
        return num;
    }

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            // Parse u field: first comma follows u's value
            int firstComma = row.indexOf(',');
            int u = parseInt(row, 5, firstComma);
            
            // Parse s field: second comma follows s's value
            int secondComma = row.indexOf(',', firstComma + 1);
            int s = parseInt(row, firstComma + 5, secondComma);
            
            // Parse f flag only need the first character to check true/false
            boolean f = row.charAt(secondComma + 5) == 't';
            
            // Original checksum logic preserved exactly
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}