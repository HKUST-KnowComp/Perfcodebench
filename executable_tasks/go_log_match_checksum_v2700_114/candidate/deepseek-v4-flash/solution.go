package main

var hexValid [256]bool

func init() {
    for i := '0'; i <= '9'; i++ {
        hexValid[i] = true
    }
    for i := 'A'; i <= 'F'; i++ {
        hexValid[i] = true
    }
}

func run(rows []string) uint64 {
    var hash uint64 = 1469598103934665603
    for _, row := range rows {
        v := uint64(0)
        if len(row) == 15 &&
            row[0] >= 'A' && row[0] <= 'Z' &&
            row[1] >= 'A' && row[1] <= 'Z' &&
            row[2] >= '0' && row[2] <= '9' &&
            row[3] >= '0' && row[3] <= '9' &&
            row[4] >= '0' && row[4] <= '9' &&
            row[5] >= '0' && row[5] <= '9' &&
            row[6] == '-' &&
            hexValid[row[7]] && hexValid[row[8]] && hexValid[row[9]] && hexValid[row[10]] &&
            hexValid[row[11]] && hexValid[row[12]] && hexValid[row[13]] && hexValid[row[14]] {
            v = uint64(row[2] + row[7])
        }
        hash ^= v + 1
        hash *= 1099511628211
    }
    return hash
}