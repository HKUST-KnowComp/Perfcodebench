package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	hash := uint64(fnvOffset)

	for _, s := range rows {
		var u, score, flag int
		i := 2 // Skip leading "u=" to reach first digit of u

		// Parse u until first & separator
		for s[i] != '&' {
			u = u*10 + int(s[i]-'0')
			i++
		}
		i++ // Skip the &, align to start of "score="
		i += 6 // Skip "score=" to reach first digit of score

		// Parse score until second & separator
		for s[i] != '&' {
			score = score*10 + int(s[i]-'0')
			i++
		}
		i++ // Skip the &, align to start of "flag="
		i += 5 // Skip "flag=" to reach first digit of flag

		// Parse flag to end of string
		for i < len(s) {
			flag = flag*10 + int(s[i]-'0')
			i++
		}

		// Exact same value and hash update logic as original to preserve correctness
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}