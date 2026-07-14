import java.util.List;

public class Solution {
    private static int parseNum(String s, int start, int end) {
        int num = 0;
        for (int i = start; i < end; i++) {
            num = num * 10 + (s.charAt(i) - '0');
        }
        return num;
    }

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long MULTIPLIER = 1099511628211L;
        for (String row : rows) {
            int firstSpace = row.indexOf(' ', 5);
            int src = parseNum(row, 5, firstSpace);
            
            int secondSpace = row.indexOf(' ', firstSpace + 1);
            int lat = parseNum(row, firstSpace + 5, secondSpace);
            
            int thirdSpace = row.indexOf(' ', secondSpace + 1);
            int code = parseNum(row, secondSpace + 6, thirdSpace);
            
            boolean ok = row.charAt(thirdSpace + 4) == 'y';
            
            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= MULTIPLIER;
        }
        return hash;
    }
}