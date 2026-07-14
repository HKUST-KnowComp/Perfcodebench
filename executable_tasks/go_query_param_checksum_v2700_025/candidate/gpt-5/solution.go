package main

func run(rows []string) uint64 {
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	var hash uint64 = offset
	for _, row := range rows {
		// Parse: u=<num>&score=<num>&flag=<num>
		s := row
		// u=
		u, i := parseUintUntilAmp(s, 2) // start after "u="
		// & then score=
		if i < len(s) {
			i++ // skip '&'
		}
		i += 6 // skip "score="
		score, i2 := parseUintUntilAmp(s, i)
		// & then flag=
		i = i2
		if i < len(s) {
			i++ // skip '&'
		}
		i += 5 // skip "flag="
		flag, _ := parseUintUntilAmp(s, i)

		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= prime
	}
	return hash
}

// parseUintUntilAmp parses digits starting at index i until '&' or end, returns value and index where it stopped.
func parseUintUntilAmp(s string, i int) (uint64, int) {
	n := len(s)
	var v uint64
	for i < n {
		c := s[i]
		if c == '&' {
			break
		}
		v = v*10 + uint64(c-'0')
		i++
	}
	return v, i
}
