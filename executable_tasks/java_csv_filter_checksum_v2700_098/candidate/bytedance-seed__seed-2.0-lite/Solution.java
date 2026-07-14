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
            int comma1 = row.indexOf(',');
            int comma2 = row.indexOf(',', comma1 + 1);
            int comma3 = row.indexOf(',', comma2 + 1);
            int comma4 = row.indexOf(',', comma3 + 1);
            
            int b = parseInt(row, comma2 + 1, comma3);
            int c = parseInt(row, comma3 + 1, comma4);
            
            int part5End = row.indexOf(',', comma4 + 1);
            if (part5End == -1) part5End = row.length();
            boolean hot = (part5End - (comma4 + 1) == 3)
                && row.charAt(comma4 + 1) == 'h'
                && row.charAt(comma4 + 2) == 'o'
                && row.charAt(comma4 + 3) == 't';
            
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}