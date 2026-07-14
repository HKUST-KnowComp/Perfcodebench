package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
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
			isHex(row[7]) && isHex(row[8]) && isHex(row[9]) && isHex(row[10]) &&
			isHex(row[11]) && isHex(row[12]) && isHex(row[13]) && isHex(row[14]) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func isHex(b byte) bool {
	return (b >= '0' && b <= '9') || (b >= 'A' && b <= 'F')
}