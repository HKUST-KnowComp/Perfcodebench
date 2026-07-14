package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var i int
		
		// Parse u=
		if i < len(row) && row[i] == 'u' && i+1 < len(row) && row[i+1] == '=' {
			i += 2
			for i < len(row) && row[i] != '&' {
				u = u*10 + int(row[i]-'0')
				i++
			}
		}
		
		// Skip to score=
		for i < len(row) && row[i] != 's' {
			i++
		}
		if i < len(row) && row[i] == 's' && i+1 < len(row) && row[i+1] == 'c' {
			i += 7 // "score="
			for i < len(row) && row[i] != '&' {
				score = score*10 + int(row[i]-'0')
				i++
			}
		}
		
		// Skip to flag=
		for i < len(row) && row[i] != 'f' {
			i++
		}
		if i < len(row) && row[i] == 'f' && i+1 < len(row) && row[i+1] == 'l' {
			i += 5 // "flag="
			for i < len(row) && row[i] != '&' {
				flag = flag*10 + int(row[i]-'0')
				i++
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}