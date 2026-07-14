package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// row format: "u=<int>&score=<int>&flag=<int>"
		// Find first '&' to get u value
		i := 0
		for row[i] != '&' {
			i++
		}
		// u value starts at index 2 (after "u=")
		u := 0
		for j := 2; j < i; j++ {
			u = u*10 + int(row[j]-'0')
		}

		// Find second '&' to get score value
		// score starts at i+1+6 = i+7 (after "&score=")
		start := i + 7
		i = start
		for row[i] != '&' {
			i++
		}
		score := 0
		for j := start; j < i; j++ {
			score = score*10 + int(row[j]-'0')
		}

		// flag starts at i+1+5 = i+6 (after "&flag=")
		start = i + 6
		flag := 0
		for j := start; j < len(row); j++ {
			flag = flag*10 + int(row[j]-'0')
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
