import java.util.*;

public class Solution {
    private static int parseInt(String s, int start, int end) {
        int val = 0;
        for (int i = start; i < end; i++) {
            val = val * 10 + (s.charAt(i) - '0');
        }
        return val;
    }

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long multiplier = 1099511628211L;
        for (String row : rows) {
            int pos1 = row.indexOf(',');
            int pos2 = row.indexOf(',', pos1 + 1);
            int pos3 = row.indexOf(',', pos2 + 1);
            int pos4 = row.indexOf(',', pos3 + 1);
            
            int b = parseInt(row, pos2 + 1, pos3);
            int c = parseInt(row, pos3 + 1, pos4);
            
            boolean hot = row.charAt(pos4 + 1) == 'h' 
                      && row.charAt(pos4 + 2) == 'o' 
                      && row.charAt(pos4 + 3) == 't';
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= multiplier;
        }
        return hash;
    }
}