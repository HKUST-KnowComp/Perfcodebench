package main

func run(rows []string) uint64 {
    const (
        fnvBase   uint64 = 1469598103934665603
        fnvPrime  uint64 = 1099511628211
    )
    hash := fnvBase
    for _, row := range rows {
        b, c, hot := parseRow(row)
        v := uint64(b + c + 20)
        if hot {
            v += 29
        }
        hash ^= v + 1
        hash *= fnvPrime
    }
    return hash
}

func parseRow(row string) (b, c int, hot bool) {
    start := 0
    commaCount := 0
    for i := 0; i <= len(row); i++ {
        if i == len(row) || row[i] == ',' {
            field := row[start:i]
            if commaCount == 2 {
                b = fastAtoi(field)
            } else if commaCount == 3 {
                c = fastAtoi(field)
            } else if commaCount == 4 {
                hot = field == "hot"
                break
            }
            start = i + 1
            commaCount++
        }
    }
    return
}

func fastAtoi(s string) int {
    if len(s) == 0 {
        return 0
    }
    n := 0
    for i := 0; i < len(s); i++ {
        n = n*10 + int(s[i]-'0')
    }
    return n
}