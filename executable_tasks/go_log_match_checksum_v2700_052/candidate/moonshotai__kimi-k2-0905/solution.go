package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211
	hash := fnvOffset
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 14 &&
			row[2] >= '0' && row[2] <= '9' &&
			row[7] >= '0' && row[7] <= '9' &&
			row[6] == '-' &&
			((row[0]|row[1])&0x20) == 0 && // both uppercase letters
			((row[3]|row[4]|row[5])&0x20) == 0 &&
			((row[8]|row[9]|row[10]|row[11]|row[12]|row[13])&0x20) == 0 {
			v = uint64(row[2]) + uint64(row[7])
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}