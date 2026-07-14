package main

func parseDigits(s string) int {
	n := 0
	for i := range s {
		n = n*10 + int(s[i]-'0')
	}
	return n
}

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	hash := fnvOffset
	for _, row := range rows {
		var firstAmp, secondAmp int
		var foundFirst bool
		for i := range row {
			if row[i] == '&' {
				if !foundFirst {
					firstAmp = i
					foundFirst = true
				} else {
					secondAmp = i
					break
				}
			}
		}
		u := parseDigits(row[2:firstAmp])
		score := parseDigits(row[firstAmp+7:secondAmp])
		flag := parseDigits(row[secondAmp+6:])
		v := uint64((u&2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}