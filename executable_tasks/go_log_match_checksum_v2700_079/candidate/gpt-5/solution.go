package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			// Check the fixed-format: AA9999-XXXXXXXX (A=uppercase letter, 9=digit, X=uppercase hex)
			b0 := row[0]
			b1 := row[1]
			if b0 >= 'A' && b0 <= 'Z' && b1 >= 'A' && b1 <= 'Z' {
				d0 := row[2]
				d1 := row[3]
				d2 := row[4]
				d3 := row[5]
				if d0 >= '0' && d0 <= '9' && d1 >= '0' && d1 <= '9' && d2 >= '0' && d2 <= '9' && d3 >= '0' && d3 <= '9' && row[6] == '-' {
					// Hex part: 8 chars of [0-9A-F]
					h0 := row[7]
					h1 := row[8]
					h2 := row[9]
					h3 := row[10]
					h4 := row[11]
					h5 := row[12]
					h6 := row[13]
					h7 := row[14]
					if ((h0 >= '0' && h0 <= '9') || (h0 >= 'A' && h0 <= 'F')) &&
						((h1 >= '0' && h1 <= '9') || (h1 >= 'A' && h1 <= 'F')) &&
						((h2 >= '0' && h2 <= '9') || (h2 >= 'A' && h2 <= 'F')) &&
						((h3 >= '0' && h3 <= '9') || (h3 >= 'A' && h3 <= 'F')) &&
						((h4 >= '0' && h4 <= '9') || (h4 >= 'A' && h4 <= 'F')) &&
						((h5 >= '0' && h5 <= '9') || (h5 >= 'A' && h5 <= 'F')) &&
						((h6 >= '0' && h6 <= '9') || (h6 >= 'A' && h6 <= 'F')) &&
						((h7 >= '0' && h7 <= '9') || (h7 >= 'A' && h7 <= 'F')) {
						v = uint64(d0 + h0)
					}
				}
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
