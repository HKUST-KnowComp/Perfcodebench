import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int pos = 0;

            // Expect "{" 
            pos++; // skip {
            
            // Find "u":
            pos += 4; // skip "u":
            int startU = pos;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                pos++;
            }
            int u = parseFastInt(row, startU, pos);

            // Find "s":
            pos += 4; // skip ,"s":
            int startS = pos;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                pos++;
            }
            int s = parseFastInt(row, startS, pos);

            // Find "f":
            pos += 4; // skip ,"f":
            boolean f = row.charAt(pos) == 't';
            pos += (f ? 4 : 5); // skip true or false

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