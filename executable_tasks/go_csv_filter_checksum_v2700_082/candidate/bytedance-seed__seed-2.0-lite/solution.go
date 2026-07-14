package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, s := range rows {
		var commaPos [4]int
		found := 0
		for i := 0; i < len(s) && found < 4; i++ {
			if s[i] == ',' {
				commaPos[found] = i
				found++
			}
		}
		// Parse b from parts[2]
		b := 0
		for i := commaPos[1] + 1; i < commaPos[2]; i++ {
			b = b*10 + int(s[i]-'0')
		}
		// Parse c from parts[3]
		c := 0
		for i := commaPos[2] + 1; i < commaPos[3]; i++ {
			c = c*10 + int(s[i]-'0')
		}
		// Check if parts[4] is exactly "hot"
		hot := false
		hotStart := commaPos[3] + 1
		if len(s) >= hotStart+3 {
			if s[hotStart] == 'h' && s[hotStart+1] == 'o' && s[hotStart+2] == 't' {
				if len(s) == hotStart+3 || s[hotStart+3] == ',' {
					hot = true
				}
			}
		}
		// Exact same value and hash calculation as original
		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}