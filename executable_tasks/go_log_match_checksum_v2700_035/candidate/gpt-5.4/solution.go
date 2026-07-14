package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var v uint64
		if len(row) == 15 &&
			isUpper(row[0]) &&
			isUpper(row[1]) &&
			isDigit(row[2]) &&
			isDigit(row[3]) &&
			isDigit(row[4]) &&
			isDigit(row[5]) &&
			row[6] == '-' &&
			isHexUpper(row[7]) &&
			isHexUpper(row[8]) &&
			isHexUpper(row[9]) &&
			isHexUpper(row[10]) &&
			isHexUpper(row[11]) &&
			isHexUpper(row[12]) &&
			isHexUpper(row[13]) &&
			isHexUpper(row[14]) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func isUpper(b byte) bool {
	return b >= 'A' && b <= 'Z'
}

func isDigit(b byte) bool {
	return b >= '0' && b <= '9'
}

func isHexUpper(b byte) bool {
	return (b >= '0' && b <= '9') || (b >= 'A' && b <= 'F')
}
