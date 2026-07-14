package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Parse: u=<int>&score=<int>&flag=<int>
		// We know the format, so we parse directly by scanning for delimiters
		s := row
		n := len(s)
		
		// Find first '=' after "u"
		i := 2 // skip "u="
		u := 0
		for i < n && s[i] != '&' {
			u = u*10 + int(s[i]-'0')
			i++
		}
		
		// Skip "&score="
		i += 7 // skip "&score="
		score := 0
		for i < n && s[i] != '&' {
			score = score*10 + int(s[i]-'0')
			i++
		}
		
		// Skip "&flag="
		i += 6 // skip "&flag="
		flag := 0
		for i < n {
			flag = flag*10 + int(s[i]-'0')
			i++
		}
		
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}