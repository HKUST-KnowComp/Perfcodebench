package main

func run(rows []string) uint64 {
	const fnvOffsetBasis = 1469598103934665603
	const fnvPrime = 1099511628211
	var hash uint64 = fnvOffsetBasis
	for _, s := range rows {
		i := 2 // skip "u="
		u := 0
		for i < len(s) && s[i] >= '0' && s[i] <= '9' {
			u = u*10 + int(s[i]-'0')
			i++
		}
		i++ // skip '&'
		i += 6 // skip "score="
		score := 0
		for i < len(s) && s[i] >= '0' && s[i] <= '9' {
			score = score*10 + int(s[i]-'0')
			i++
		}
		i++ // skip '&'
		i += 5 // skip "flag="
		flag := 0
		for i < len(s) && s[i] >= '0' && s[i] <= '9' {
			flag = flag*10 + int(s[i]-'0')
			i++
		}
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}