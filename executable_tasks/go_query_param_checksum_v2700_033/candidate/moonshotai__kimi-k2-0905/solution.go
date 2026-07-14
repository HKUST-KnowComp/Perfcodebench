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
		i := 0

		// skip "u="
		i += 2
		for ; b[i] >= '0' && b[i] <= '9'; i++ {
			u = u*10 + int(b[i]-'0')
		}
		i++ // '&'

		// skip "score="
		i += 6
		for ; b[i] >= '0' && b[i] <= '9'; i++ {
			score = score*10 + int(b[i]-'0')
		}
		i++ // '&'

		// skip "flag="
		i += 5
		for ; i < len(b) && b[i] >= '0' && b[i] <= '9'; i++ {
			flag = flag*10 + int(b[i]-'0')
		}

		v := uint64((u&2047)+score+flag*19) + 1
		hash ^= v
		hash *= prime
	}
	return hash
}