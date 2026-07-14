package main

func isHexUpper(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var v uint64
		if len(row) == 15 {
			// Help bounds-check elimination for subsequent indices
			_ = row[14]
			if row[0] >= 'A' && row[0] <= 'Z' &&
				row[1] >= 'A' && row[1] <= 'Z' &&
				row[2] >= '0' && row[2] <= '9' &&
				row[3] >= '0' && row[3] <= '9' &&
				row[4] >= '0' && row[4] <= '9' &&
				row[5] >= '0' && row[5] <= '9' &&
				row[6] == '-' {
				c7 := row[7]
				c8 := row[8]
				c9 := row[9]
				c10 := row[10]
				c11 := row[11]
				c12 := row[12]
				c13 := row[13]
				c14 := row[14]
				if isHexUpper(c7) && isHexUpper(c8) && isHexUpper(c9) && isHexUpper(c10) &&
					isHexUpper(c11) && isHexUpper(c12) && isHexUpper(c13) && isHexUpper(c14) {
					v = uint64(row[2] + row[7])
				}
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
