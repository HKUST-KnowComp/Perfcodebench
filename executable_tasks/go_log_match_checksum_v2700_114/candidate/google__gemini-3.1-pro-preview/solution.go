package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			// Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
			// Check [A-Z]{2}
			if row[0] >= 'A' && row[0] <= 'Z' &&
				row[1] >= 'A' && row[1] <= 'Z' &&
				// Check [0-9]{4}
				row[2] >= '0' && row[2] <= '9' &&
				row[3] >= '0' && row[3] <= '9' &&
				row[4] >= '0' && row[4] <= '9' &&
				row[5] >= '0' && row[5] <= '9' &&
				// Check -
				row[6] == '-' &&
				// Check [0-9A-F]{8}
				isHex(row[7]) && isHex(row[8]) && isHex(row[9]) && isHex(row[10]) &&
				isHex(row[11]) && isHex(row[12]) && isHex(row[13]) && isHex(row[14]) {
				v = uint64(row[2] + row[7])
			}
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}

// isHex checks if a byte is in [0-9A-F]
func isHex(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}