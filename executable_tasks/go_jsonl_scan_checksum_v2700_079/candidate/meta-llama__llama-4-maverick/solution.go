package main

func run(rows []string) uint64 {
    var hash uint64 = 1469598103934665603
    for _, row := range rows {
        user, score, flag := false, false, false
        v := uint64(0)
        for i := 0; i < len(row); i++ {
            if row[i] == '"' {
                if i+6 < len(row) && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
                    i += 7
                    user = true
                    for i < len(row) && (row[i] >= '0' && row[i] <= '9') {
                        v = v*10 + uint64(row[i]-'0')
                        i++
                    }
                    v &= 1023

                } else if i+7 < len(row) && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
                    i += 8
                    score = true
                    for i < len(row) && (row[i] >= '0' && row[i] <= '9') {
                        v = v*10 + uint64(row[i]-'0')
                        i++
                    }
                } else if i+6 < len(row) && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' {
                    i += 7
                    if i+3 < len(row) && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
                        flag = true
                    }
                }
            }
        }
        if user && score {
            if flag {
                v += 17
            }
            hash ^= v + 1
            hash *= 1099511628211
        }
    }
    return hash
}
