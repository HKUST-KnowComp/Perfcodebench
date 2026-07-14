package main

func isHexUpper(b byte) bool {
	return (b >= '0' && b <= '9') || (b >= 'A' && b <= 'F')
}

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
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
					b7 := row[7]
					b8 := row[8]
					b9 := row[9]
					b10 := row[10]
					b11 := row[11]
					b12 := row[12]
					b13 := row[13]
					b14 := row[14]
					if isHexUpper(b7) && isHexUpper(b8) && isHexUpper(b9) && isHexUpper(b10) && isHexUpper(b11) && isHexUpper(b12) && isHexUpper(b13) && isHexUpper(b14) {
						v = uint64(b2 + b7)
					}
				}
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
