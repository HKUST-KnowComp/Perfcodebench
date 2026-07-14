package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		var currentVal int
		var foundU, foundScore, foundFlag bool

		for i := 0; i < len(row); {
			// Find key
			start := i
			for i < len(row) && row[i] != '=' {
				i++
			}
			key := row[start:i]
			i++ // skip '='

			// Find value
			start = i
			for i < len(row) && row[i] != '&' {
				i++
			}
			valStr := row[start:i]
			i++ // skip '&'

			// Fast parse integer
			val := 0
			for j := 0; j < len(valStr); j++ {
				val = val*10 + int(valStr[j]-'0')
			}

			switch key {
			case "u":
				u = val
				foundU = true
			case "score":
				score = val
				foundScore = true
			case "flag":
				flag = val
				foundFlag = true
			}
			
			if foundU && foundScore && foundFlag {
				break
			}
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}