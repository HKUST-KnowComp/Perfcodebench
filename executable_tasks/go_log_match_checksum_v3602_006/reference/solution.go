package main

func isHex(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, s := range rows {
		v := uint64(0)
		ok := len(s) == 15 &&
			s[0] >= 'A' && s[0] <= 'Z' &&
			s[1] >= 'A' && s[1] <= 'Z' &&
			s[2] >= '0' && s[2] <= '9' &&
			s[3] >= '0' && s[3] <= '9' &&
			s[4] >= '0' && s[4] <= '9' &&
			s[5] >= '0' && s[5] <= '9' &&
			s[6] == '-' &&
			isHex(s[7]) && isHex(s[8]) && isHex(s[9]) && isHex(s[10]) &&
			isHex(s[11]) && isHex(s[12]) && isHex(s[13]) && isHex(s[14])
		if ok {
			v = uint64(s[2] + s[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
