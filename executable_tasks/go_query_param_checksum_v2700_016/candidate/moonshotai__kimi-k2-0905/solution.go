package main

func run(rows []string) uint64 {
	const (
		prime  = 1099511628211
		offset = 1469598103934665603
	)
	hash := offset
	for _, row := range rows {
		b := []byte(row)
		// parse u
		i := 2 // skip "u="
		u := 0
		for ; b[i] != '&'; i++ {
			u = u*10 + int(b[i]-'0')
		}
		i++ // skip '&'
		// parse score
		i += 6 // skip "score="
		score := 0
		for ; b[i] != '&'; i++ {
			score = score*10 + int(b[i]-'0')
		}
		i++ // skip '&'
		// parse flag
		i += 5 // skip "flag="
		flag := 0
		for ; i < len(b); i++ {
			flag = flag*10 + int(b[i]-'0')
		}
		v := uint64((u&2047)+score+flag*19) + 1
		hash ^= v
		hash *= prime
	}
	return hash
}