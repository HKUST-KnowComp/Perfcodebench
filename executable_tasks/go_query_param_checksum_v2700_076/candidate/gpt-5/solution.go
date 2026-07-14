package main

func parseIntRange(s string, start, end int, prefix string) int {
	// Skip expected prefix if present
	if plen := len(prefix); end-start >= plen && plen > 0 {
		matched := true
		for k := 0; k < plen; k++ {
			if s[start+k] != prefix[k] {
				matched = false
				break
			}
		}
		if matched {
			start += len(prefix)
		}
	}
	if start >= end {
		return 0
	}
	// Optional sign
	sign := 1
	sw := s[start]
	if sw == '+' {
		start++
	} else if sw == '-' {
		sign = -1
		start++
	}
	if start >= end {
		return 0
	}
	// Parse digits; must consume entire remainder
	val := 0
	for i := start; i < end; i++ {
		c := s[i]
		if c < '0' || c > '9' {
			return 0
		}
		val = val*10 + int(c-'0')
	}
	return sign * val
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		n := len(row)
		i := 0
		// field 1: [0, i)
		for i < n && row[i] != '&' {
			i++
		}
		u := parseIntRange(row, 0, i, "u=")
		if i < n {
			i++ // skip '&'
		}
		// field 2: [start, i)
		start := i
		for i < n && row[i] != '&' {
			i++
		}
		score := parseIntRange(row, start, i, "score=")
		if i < n {
			i++ // skip '&'
		}
		// field 3: [start, i)
		start = i
		for i < n && row[i] != '&' {
			i++
		}
		flag := parseIntRange(row, start, i, "flag=")

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
