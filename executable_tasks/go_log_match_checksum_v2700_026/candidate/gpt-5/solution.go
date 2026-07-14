package main

const (
	fnvOffset64 = 1469598103934665603
	fnvPrime64  = 1099511628211
)

func isHex(b byte) bool {
	// Uppercase hex digits only: 0-9 or A-F
	return (b >= '0' && b <= '9') || (b >= 'A' && b <= 'F')
}

func run(rows []string) uint64 {
	hash := uint64(fnvOffset64)
	for _, row := range rows {
		var v uint64
		if len(row) == 15 {
			b0 := row[0]
			b1 := row[1]
			b2 := row[2]
			b3 := row[3]
			b4 := row[4]
			b5 := row[5]
			b6 := row[6]
			if b0 >= 'A' && b0 <= 'Z' &&
				b1 >= 'A' && b1 <= 'Z' &&
				b2 >= '0' && b2 <= '9' &&
				b3 >= '0' && b3 <= '9' &&
				b4 >= '0' && b4 <= '9' &&
				b5 >= '0' && b5 <= '9' &&
				b6 == '-' {
				b7 := row[7]
				b8 := row[8]
				b9 := row[9]
				b10 := row[10]
				b11 := row[11]
				b12 := row[12]
				b13 := row[13]
				b14 := row[14]
				if isHex(b7) && isHex(b8) && isHex(b9) && isHex(b10) &&
					isHex(b11) && isHex(b12) && isHex(b13) && isHex(b14) {
					v = uint64(b2 + b7)
				}
			}
		}
		hash ^= v + 1
		hash *= fnvPrime64
	}
	return hash
}
