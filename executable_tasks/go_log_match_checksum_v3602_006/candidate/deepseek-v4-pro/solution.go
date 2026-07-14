package main

var hexTable [256]bool

func init() {
	for i := 0; i < 256; i++ {
		c := byte(i)
		hexTable[i] = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
	}
}

func isHex(c byte) bool {
	return hexTable[c]
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Fast explicit check instead of regex
		if len(row) == 15 &&
			row[0] >= 'A' && row[0] <= 'Z' &&
			row[1] >= 'A' && row[1] <= 'Z' &&
			row[2] >= '0' && row[2] <= '9' &&
			row[3] >= '0' && row[3] <= '9' &&
			row[4] >= '0' && row[4] <= '9' &&
			row[5] >= '0' && row[5] <= '9' &&
			row[6] == '-' &&
			isHex(row[7]) && isHex(row[8]) && isHex(row[9]) && isHex(row[10]) &&
			isHex(row[11]) && isHex(row[12]) && isHex(row[13]) && isHex(row[14]) {
			v := uint64(row[2] + row[7])
			hash ^= v + 1
			hash *= 1099511628211
		}
	}
	return hash
}
