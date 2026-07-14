import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 146959810394665603L; // Corrected constant from original logic if needed, but keeping original value: 1469598103934665603L
        hash = 1469598103934665603L;

        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            // Manual parsing to replace Regex
            // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
            
            int cursor = 0;
            
            // src=S
            if (len < 6 || row.charAt(0) != 's' || row.charAt(1) != 'r' || row.charAt(2) != 'c' || 
                row.charAt(3) != '=' || row.charAt(4) != 'S') return hash; // Should not happen per spec
            cursor = 5;
            
            // Parse src
            int src = 0;
            while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
                src = src * 10 + (row.charAt(cursor) - '0');
                cursor++;
            }
            
            // lat=
            if (cursor + 4 > len || row.charAt(cursor) != ' ' || row.charAt(cursor+1) != 'l' || 
                row.charAt(cursor+2) != 'a' || row.charAt(cursor+3) != 't' || row.charAt(cursor+4) != '=') return hash;
            cursor += 5;
            
            // Parse lat
            int lat = 0;
            while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
                lat = lat * 10 + (row.charAt(cursor) - '0');
                cursor++;
            }
            
            //  code=
            if (cursor + 6 > len || row.charAt(cursor) != ' ' || row.charAt(cursor+1) != 'c' || 
                row.charAt(cursor+2) != 'o' || row.charAt(cursor+3) != 'd' || 
                row.charAt(cursor+4) != 'e' || row.charAt(cursor+5) != '=') return hash;
            cursor += 6;
            
            // Parse code
            int code = 0;
            while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
                code = code * 10 + (row.charAt(cursor) - '0');
                cursor++;
            }
            
            //  ok=
            if (cursor + 4 > len || row.charAt(cursor) != ' ' || row.charAt(cursor+1) != 'o' || 
                row.charAt(cursor+2) != 'k' || row.charAt(cursor+3) != '=') return hash;
            cursor += 4;
            
            // Parse ok (yes/no)
            boolean ok = false;
            if (cursor + 3 <= len && row.charAt(cursor) == 'y' && row.charAt(cursor+1) == 'e' && row.charAt(cursor+2) == 's') {
                ok = true;
                cursor += 3;
            } else if (cursor + 2 <= len && row.charAt(cursor) == 'n' && row.charAt(cursor+1) == 'o') {
                ok = false;
                cursor += 2;
            } else {
                return hash;
            }
            
            // Check end of string
            if (cursor != len) return hash;

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}