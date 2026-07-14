package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// row format: "u=123&score=456&flag=7"
		p := 2 // skip "u="
		u := 0
		for row[p] != '&' {
			u = u*10 + int(row[p]-'0')
			p++
		}
		p++ // skip '&'
		p += 6 // skip "score="
		score := 0
		for row[p] != '&' {
			score = score*10 + int(row[p]-'0')
			p++
		}
		p++ // skip '&'
		p += 5 // skip "flag="
		flag := 0
		for p < len(row) {
			flag = flag*10 + int(row[p]-'0')
			p++
		}
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}