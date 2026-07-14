package main

const offset64 uint64 = 1469598103934665603
const prime64 uint64 = 1099511628211

func isHexUpper(b byte) bool {
    return (b >= '0' && b <= '9') || (b >= 'A' && b <= 'F')
}

func run(rows []string) uint64 {
    hash := offset64
    for _, row := range rows {
        var v uint64
        if len(row) == 15 {
            b0 := row[0]
            b1 := row[1]
            if b0 >= 'A' && b0 <= 'Z' && b1 >= 'A' && b1 <= 'Z' {
                b2 := row[2]
                b3 := row[3]
                b4 := row[4]
                b5 := row[5]
                if b2 >= '0' && b2 <= '9' && b3 >= '0' && b3 <= '9' && b4 >= '0' && b4 <= '9' && b5 >= '0' && b5 <= '9' && row[6] == '-' {
                    if isHexUpper(row[7]) && isHexUpper(row[8]) && isHexUpper(row[9]) && isHexUpper(row[10]) &&
                        isHexUpper(row[11]) && isHexUpper(row[12]) && isHexUpper(row[13]) && isHexUpper(row[14]) {
                        v = uint64(b2 + row[7])
                    }
                }
            }
        }
        hash ^= v + 1
        hash *= prime64
    }
    return hash
}
