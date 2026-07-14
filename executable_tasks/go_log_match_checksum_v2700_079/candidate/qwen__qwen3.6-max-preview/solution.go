package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			if row[0]-'A' < 26 && row[1]-'A' < 26 &&
				row[2]-'0' < 10 && row[3]-'0' < 10 && row[4]-'0' < 10 && row[5]-'0' < 10 &&
				row[6] == '-' &&
				isHex(row[7]) && isHex(row[8]) && isHex(row[9]) && isHex(row[10]) &&
				isHex(row[11]) && isHex(row[12]) && isHex(row[13]) && isHex(row[14]) {
				v = uint64(row[2] + row[7])
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func isHex(c byte) bool {
	return c-'0' < 10 || c-'A' < 6
}