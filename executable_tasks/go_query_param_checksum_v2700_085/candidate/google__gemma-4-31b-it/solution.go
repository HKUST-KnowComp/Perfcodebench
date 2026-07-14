package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		
		// Manual scan to avoid strings.Split and strings.TrimPrefix
		// Expected format: "u=X&score=Y&flag=Z"
		
		// Parse u
		i := 0
		if i < len(row) && row[i] == 'u' && i+1 < len(row) && row[i+1] == '=' {
			i += 2
			for i < len(row) && row[i] != '&' {
				u = u*10 + int(row[i]-'0')
				i++
			}
		}
		
		// Parse score
		if i < len(row) && row[i] == '&' {
			i++
		}
		// skip "score="
		i += 6
		for i < len(row) && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		
		// Parse flag
		if i < len(row) && row[i] == '&' {
			i++
		}
		// skip "flag="
		i += 5
		for i < len(row) {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}