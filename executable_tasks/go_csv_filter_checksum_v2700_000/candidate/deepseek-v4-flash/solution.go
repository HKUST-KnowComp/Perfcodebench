package main

func run(rows []string) uint64 {
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	hash := fnvOffset
	for _, row := range rows {
		var b, c uint64
		hot := false
		commaCount := 0
		start := 0
		i := 0
		for i < len(row) {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					b = parseUint(row, start, i)
				} else if commaCount == 3 {
					c = parseUint(row, start, i)
				} else if commaCount == 4 {
					// field after this comma is the 5th field (index 4)
					fieldStart := i + 1
					if fieldStart+3 <= len(row) && row[fieldStart] == 'h' && row[fieldStart+1] == 'o' && row[fieldStart+2] == 't' {
						hot = true
					}
					break
				}
				start = i + 1
			}
			i++
		}

		v := b + c + 7
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}

// parseUint converts the substring row[l:r] (which must consist only of digits) to uint64.
func parseUint(row string, l, r int) uint64 {
	n := uint64(0)
	for i := l; i < r; i++ {
		n = n*10 + uint64(row[i]-'0')
	}
	return n
}
