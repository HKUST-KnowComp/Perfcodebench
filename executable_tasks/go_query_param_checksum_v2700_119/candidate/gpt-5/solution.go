package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, s := range rows {
		n := len(s)
		i := 2 // after "u="

		// parse u until '&'
		var u int
		sign := 1
		if i < n && s[i] == '-' {
			sign = -1
			i++
		}
		for i < n {
			c := s[i]
			if c == '&' {
				i++
				break
			}
			u = u*10 + int(c-'0')
			i++
		}
		u *= sign

		// skip "score="
		i += 6
		var score int
		sign = 1
		if i < n && s[i] == '-' {
			sign = -1
			i++
		}
		for i < n {
			c := s[i]
			if c == '&' {
				i++
				break
			}
			score = score*10 + int(c-'0')
			i++
		}
		score *= sign

		// skip "flag="
		i += 5
		var flag int
		sign = 1
		if i < n && s[i] == '-' {
			sign = -1
			i++
		}
		for i < n {
			c := s[i]
			if c == '&' {
				break
			}
			flag = flag*10 + int(c-'0')
			i++
		}
		flag *= sign

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
