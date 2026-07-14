package main

func run(rows []string) uint64 {
	var h uint64 = 14695981039346656037
	for _, r := range rows {
		var v uint64
		if len(r) == 17 &&
			r[0] >= 'A' && r[0] <= 'Z' &&
			r[1] >= 'A' && r[1] <= 'Z' &&
			r[2] >= '0' && r[2] <= '9' &&
			r[3] >= '0' && r[3] <= '9' &&
			r[4] >= '0' && r[4] <= '9' &&
			r[5] >= '0' && r[5] <= '9' &&
			r[6] == '-' &&
			isHex(r[7]) && isHex(r[8]) && isHex(r[9]) && isHex(r[10]) &&
			isHex(r[11]) && isHex(r[12]) && isHex(r[13]) && isHex(r[14]) &&
			isHex(r[15]) && isHex(r[16]) {
			v = uint64(r[2] + r[7])
		}
		h ^= v + 1
		h *= 1099511628211
	}
	return h
}

func isHex(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}