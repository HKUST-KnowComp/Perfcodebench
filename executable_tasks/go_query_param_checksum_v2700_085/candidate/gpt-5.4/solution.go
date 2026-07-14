package main

func run(rows []string) uint64 {
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	hash := offset
	for _, row := range rows {
		// Expected format: u=<int>&score=<int>&flag=<int>
		// Parse directly to avoid allocations from Split/TrimPrefix and overhead from strconv.
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

		// skip "score="
		i += 6
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

		// skip "flag="
		i += 5
		flag := 0
		for i < n {
			flag = flag*10 + int(row[i]-'0')
			i++
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
