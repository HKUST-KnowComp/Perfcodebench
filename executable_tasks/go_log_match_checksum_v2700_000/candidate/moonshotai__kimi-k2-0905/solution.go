package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211

	hash := fnvOffset
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 14 &&
			row[2] == '-' &&
			row[7] == '-' &&
			isUpper(row[0]) && isUpper(row[1]) &&
			isDigit(row[3]) && isDigit(row[4]) && isDigit(row[5]) && isDigit(row[6]) &&
			isHex(row[8]) && isHex(row[9]) && isHex(row[10]) && isHex(row[11]) && isHex(row[12]) && isHex(row[13]) {
			v = uint64(row[2]) + uint64(row[7])
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}

func isUpper(b byte) bool { return b >= 'A' && b <= 'Z' }
func isDigit(b byte) bool { return b >= '0' && b <= '9' }
func isHex(b byte) bool {
	return (b >= '0' && b <= '9') || (b >= 'A' && b <= 'F')
}