import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int i = 0;
            
            while (row.charAt(i) != '=') i++;
            int uStart = ++i;
            while (row.charAt(i) != '&') i++;
            int u = parseInt(row, uStart, i);
            i++;
            
            while (row.charAt(i) != '=') i++;
            int sStart = ++i;
            while (row.charAt(i) != '&') i++;
            int s = parseInt(row, sStart, i);
            i++;
            
            while (row.charAt(i) != '=') i++;
            int fStart = ++i;
            int f = parseInt(row, fStart, len);
            
            long v = (u & 2047) + s + f * 19L;
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
    
    private static int parseInt(String s, int begin, int end) {
        int res = 0;
        int i = begin;
        if (i < end && s.charAt(i) == '-') {
            i++;
            for (; i < end; i++) res = res * 10 - (s.charAt(i) - '0');
            return res;
        }
        for (; i < end; i++) res = res * 10 + (s.charAt(i) - '0');
        return res;
    }
}