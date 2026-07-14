package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		
		// Manual scan to avoid Split and TrimPrefix allocations
		i := 0
		n := len(row)
	
		// Parse u=
		for i < n && row[i] != '=' {
			i++
		}
		i++ // skip '='
		for i < n && row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		i++ // skip '&'
	
		// Parse score=
		for i < n && row[i] != '=' {
			i++
		}
		i++ // skip '='
		for i < n && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		i++ // skip '&'
	
		// Parse flag=
		for i < n && row[i] != '=' {
			i++
		}
		i++ // skip '='
		for i < n {
			flag = flag*10 + int(row[i]-'0')
			i++
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}