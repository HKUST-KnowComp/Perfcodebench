package main

func run(rows []string) uint64 {
	const (
		prime  = 1099511628211
		offset = 1469598103934665603
	)
	hash := offset
	for _, row := range rows {
		b := []byte(row)
		var u, score, flag int
		// skip "u="
		i := 2
		for ; b[i] >= '0' && b[i] <= '9'; i++ {
			u = u*10 + int(b[i]-'0')
		}
		// skip "&score="
		i += 7
		for ; b[i] >= '0' && b[i] <= '9'; i++ {
			score = score*10 + int(b[i]-'0')
		}
		// skip "&flag="
		i += 6
		for ; i < len(b) && b[i] >= '0' && b[i] <= '9'; i++ {
			flag = flag*10 + int(b[i]-'0')
		}
		v := uint64((u&2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}
	return hash
}