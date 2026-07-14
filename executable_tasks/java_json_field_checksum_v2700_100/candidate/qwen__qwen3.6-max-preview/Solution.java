import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int uEnd = row.indexOf(',', 5);
            int u = 0;
            for (int i = 5; i < uEnd; i++) {
                u = u * 10 + (row.charAt(i) - '0');
            }
            
            int sStart = uEnd + 5;
            int sEnd = row.indexOf(',', sStart);
            int s = 0;
            for (int i = sStart; i < sEnd; i++) {
                s = s * 10 + (row.charAt(i) - '0');
            }
            
            boolean f = row.charAt(sEnd + 5) == 't';
            
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}