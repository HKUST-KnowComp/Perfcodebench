package main

var validHex = [256]bool{
	'0': true, '1': true, '2': true, '3': true, '4': true,
	'5': true, '6': true, '7': true, '8': true, '9': true,
	'A': true, 'B': true, 'C': true, 'D': true, 'E': true, 'F': true,
}

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
			validHex[row[7]] &&
			validHex[row[8]] &&
			validHex[row[9]] &&
			validHex[row[10]] &&
			validHex[row[11]] &&
			validHex[row[12]] &&
			validHex[row[13]] &&
			validHex[row[14]] {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
