package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// row format: "u=<int>&score=<int>&flag=<int>"
		// parse u: starts at index 2 (after "u=")
		i := 2
		n := len(row)
		u := 0
		neg := false
		if i < n && row[i] == '-' {
			neg = true
			i++
		}
		for i < n && row[i] != '&' {
			u = u*10 + int(row[i]-'0')
			i++
		}
		if neg {
			u = -u
		}
		// skip "&score="
		i += 7
		score := 0
		neg = false
		if i < n && row[i] == '-' {
			neg = true
			i++
		}
		for i < n && row[i] != '&' {
			score = score*10 + int(row[i]-'0')
			i++
		}
		if neg {
			score = -score
		}
		// skip "&flag="
		i += 6
		flag := 0
		neg = false
		if i < n && row[i] == '-' {
			neg = true
			i++
		}
		for i < n {
			flag = flag*10 + int(row[i]-'0')
			i++
		}
		if neg {
			flag = -flag
		}
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
