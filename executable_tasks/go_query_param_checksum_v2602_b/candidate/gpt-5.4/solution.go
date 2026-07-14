package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Expected format: u=<int>&score=<int>&flag=<int>
		// Parse directly without allocations or strconv.
		n := len(row)
		i := 2 // skip "u="
		u := 0
		for i < n {
			c := row[i]
			if c == '&' {
				i++
				break
			}
			u = u*10 + int(c-'0')
			i++
		}

		i += 6 // skip "score="
		score := 0
		for i < n {
			c := row[i]
			if c == '&' {
				i++
				break
			}
			score = score*10 + int(c-'0')
			i++
		}

		i += 5 // skip "flag="
		flag := 0
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
