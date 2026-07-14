import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            // Expected format: {"u":(digits),"s":(digits),"f":(true|false),"tag":"x(digits)"}
            int len = row.length();
            int pos = 0;
            
            // Skip to "u":
            pos += 5; // {"u":
            int startU = pos;
            while (row.charAt(pos) != ',') pos++;
            int u = parseFastInt(row, startU, pos);
            
            // Skip to "s":
            pos += 5; // ,"s":
            int startS = pos;
            while (row.charAt(pos) != ',') pos++;
            int s = parseFastInt(row, startS, pos);
            
            // Skip to "f":
            pos += 5; // ,"f":
            boolean f = row.charAt(pos) == 't';
            
            long v = (u & 1023) + s + (f ? 17 : 0);
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
}