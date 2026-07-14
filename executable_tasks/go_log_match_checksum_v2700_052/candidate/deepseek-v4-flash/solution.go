package main

func run(rows []string) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	hash := offset
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
			isHexDigit(row[7]) &&
			isHexDigit(row[8]) &&
			isHexDigit(row[9]) &&
			isHexDigit(row[10]) &&
			isHexDigit(row[11]) &&
			isHexDigit(row[12]) &&
			isHexDigit(row[13]) &&
			isHexDigit(row[14]) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}

func isHexDigit(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}
