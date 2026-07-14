package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var u, score, flag int
		i := 0
		// skip "u="
		i += 2
		// parse u
		for i < len(row) && row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		i++ // skip '&'
		// skip "score="
		i += 6
		for i < len(row) && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		i++ // skip '&'
		// skip "flag="
		i += 5
		for i < len(row) && row[i] != '&' {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		v := uint64((u&2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}