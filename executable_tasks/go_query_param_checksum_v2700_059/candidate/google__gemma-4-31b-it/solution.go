package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		
		// Manual scan for u=, score=, flag=
		// Assuming format: u=X&score=Y&flag=Z
		
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
		if i+6 < len(row) && row[i] == 's' && row[i+1] == 'c' && row[i+2] == 'o' && row[i+3] == 'r' && row[i+4] == 'e' && row[i+5] == '=' {
			i += 6
			for i < len(row) && row[i] != '&' {
				score = score*10 + int(row[i]-'0')
				i++
			}
		}
		
		// Parse flag
		if i < len(row) && row[i] == '&' {
			i++
		}
		if i+5 < len(row) && row[i] == 'f' && row[i+1] == 'l' && row[i+2] == 'a' && row[i+3] == 'g' && row[i+4] == '=' {
			i += 5
			for i < len(row) {
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