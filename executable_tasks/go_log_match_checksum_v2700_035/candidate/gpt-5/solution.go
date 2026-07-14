package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(rows); i++ {
		row := rows[i]
		var v uint64
		if len(row) == 15 {
			c0 := row[0]
			c1 := row[1]
			// [A-Z]{2}
			if c0-'A' <= 'Z'-'A' && c1-'A' <= 'Z'-'A' {
				c2 := row[2]
				c3 := row[3]
				c4 := row[4]
				c5 := row[5]
				// [0-9]{4} and '-'
				if c2-'0' <= 9 && c3-'0' <= 9 && c4-'0' <= 9 && c5-'0' <= 9 && row[6] == '-' {
					c7 := row[7]
					c8 := row[8]
					c9 := row[9]
					c10 := row[10]
					c11 := row[11]
					c12 := row[12]
					c13 := row[13]
					c14 := row[14]
					// [0-9A-F]{8}
					if ((c7-'0' <= 9) || (c7-'A' <= 5)) &&
						((c8-'0' <= 9) || (c8-'A' <= 5)) &&
						((c9-'0' <= 9) || (c9-'A' <= 5)) &&
						((c10-'0' <= 9) || (c10-'A' <= 5)) &&
						((c11-'0' <= 9) || (c11-'A' <= 5)) &&
						((c12-'0' <= 9) || (c12-'A' <= 5)) &&
						((c13-'0' <= 9) || (c13-'A' <= 5)) &&
						((c14-'0' <= 9) || (c14-'A' <= 5)) {
						v = uint64(c2 + c7)
					}
				}
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
