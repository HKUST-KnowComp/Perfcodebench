package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		// Expected format:
		// u=<int>&score=<int>&flag=<int>
		// Parse directly to avoid allocations from Split/TrimPrefix and
		// overhead from strconv.
		n := len(row)
		i := 2 // skip "u="

		u := 0
		for i < n {
			c := row[i]
			if c == '&' {
				break
			}
			u = u*10 + int(c-'0')
			i++
		}

		// skip "&score="
		i += 7
		score := 0
		for i < n {
			c := row[i]
			if c == '&' {
				break
			}
			score = score*10 + int(c-'0')
			i++
		}

		// skip "&flag="
		i += 6
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
