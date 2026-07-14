package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// row format: "u=<int>&score=<int>&flag=<int>"
		// We parse inline to avoid allocations.

		i := 2 // skip "u="
		n := len(row)

		// parse u
		var u int
		for i < n && row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}

		// skip "&score="
		i += 7

		// parse score
		var score int
		for i < n && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}

		// skip "&flag="
		i += 6

		// parse flag
		var flag int
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
