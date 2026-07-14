package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag uint64
		i := 0
		n := len(row)

		// parse u
		for i < n && (row[i] < '0' || row[i] > '9') {
			i++
		}
		for i < n && row[i] >= '0' && row[i] <= '9' {
			u = u*10 + uint64(row[i]-'0')
			i++
		}

		// parse score
		for i < n && (row[i] < '0' || row[i] > '9') {
			i++
		}
		for i < n && row[i] >= '0' && row[i] <= '9' {
			score = score*10 + uint64(row[i]-'0')
			i++
		}

		// parse flag
		for i < n && (row[i] < '0' || row[i] > '9') {
			i++
		}
		for i < n && row[i] >= '0' && row[i] <= '9' {
			flag = flag*10 + uint64(row[i]-'0')
			i++
		}

		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}