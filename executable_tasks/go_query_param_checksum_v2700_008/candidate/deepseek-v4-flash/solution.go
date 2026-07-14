package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		u, score, flag := parseRow(row)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseRow(row string) (u, score, flag int) {
	i := 0
	// skip to first '='
	for row[i] != '=' {
		i++
	}
	i++ // skip '='
	// parse u
	for i < len(row) && row[i] >= '0' && row[i] <= '9' {
		u = u*10 + int(row[i]-'0')
		i++
	}
	i++ // skip '&'
	// skip to second '='
	for row[i] != '=' {
		i++
	}
	i++ // skip '='
	// parse score
	for i < len(row) && row[i] >= '0' && row[i] <= '9' {
		score = score*10 + int(row[i]-'0')
		i++
	}
	i++ // skip '&'
	// skip to third '='
	for row[i] != '=' {
		i++
	}
	i++ // skip '='
	// parse flag
	for i < len(row) && row[i] >= '0' && row[i] <= '9' {
		flag = flag*10 + int(row[i]-'0')
		i++
	}
	return
}