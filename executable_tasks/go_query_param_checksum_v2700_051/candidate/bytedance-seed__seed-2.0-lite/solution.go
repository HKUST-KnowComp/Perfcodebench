package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Parse u value, starts after "u=" (2 bytes), ends at first &
		var u int
		i := 2
		for ; i < len(row) && row[i] != '&'; i++ {
			u = u*10 + int(row[i]-'0')
		}
		// Parse score value, starts after first & + "score=" (6 bytes), ends at second &
		var score int
		i += 7 // skip current & + 6 bytes of "score="
		for ; i < len(row) && row[i] != '&'; i++ {
			score = score*10 + int(row[i]-'0')
		}
		// Parse flag value, starts after second & + "flag=" (5 bytes)
		var flag int
		i += 6 // skip current & +5 bytes of "flag="
		for ; i < len(row); i++ {
			flag = flag*10 + int(row[i]-'0')
		}
		// Original hash calculation preserved exactly
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}