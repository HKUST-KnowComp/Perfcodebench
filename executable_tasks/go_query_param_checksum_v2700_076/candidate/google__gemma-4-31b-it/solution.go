package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var current int
		var foundU, foundScore, foundFlag bool
		
		// Manual scan to avoid Split and TrimPrefix allocations
		for i := 0; i < len(row); {
			// Find key
			start := i
			for i < len(row) && row[i] != '=' {
				i++
			}
			key := row[start:i]
			
			i++ // skip '='
			
			// Find value
			current = 0
			for i < len(row) && row[i] != '&' {
				current = current*10 + int(row[i]-'0')
				i++
			}
			
			switch key {
			case "u":
				u = current
				foundU = true
			case "score":
				score = current
				foundScore = true
			case "flag":
				flag = current
				foundFlag = true
			}
			
			i++ // skip '&'
		}
		
		if foundU && foundScore && foundFlag {
			v := uint64((u & 2047) + score + flag*19)
			hash ^= v + 1
			hash *= 1099511628211
		}
	}
	return hash
}