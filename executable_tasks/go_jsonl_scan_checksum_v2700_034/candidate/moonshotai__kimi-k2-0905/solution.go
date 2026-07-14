package main

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	hash := uint64(fnvOffset)

	for _, row := range rows {
		user, score := 0, 0
		flag := false

		inName, inVal := false, false
		nameStart := 0
		valStart := 0

		for i := 0; i < len(row); i++ {
			c := row[i]
			switch c {
			case '"':
				if !inName && !inVal {
					inName = true
					nameStart = i + 1
				} else if inName {
					inName = false
					inVal = true
					valStart = i + 2 // skip "":
					if row[i+1] != ':' {
						inVal = false
					}
				} else if inVal {
					inVal = false
				}
			case ',', '}':
				if inVal {
					// parse value
					switch row[nameStart] {
					case 'u': // user
						v := 0
						for j := valStart; j < i; j++ {
							d := row[j]
							if d >= '0' && d <= '9' {
								v = v*10 + int(d-'0')
							}
						}
						user = v
					case 's': // score
						v := 0
						for j := valStart; j < i; j++ {
							d := row[j]
							if d >= '0' && d <= '9' {
								v = v*10 + int(d-'0')
							}
						}
						score = v
					case 'f': // flag
						flag = flag || (i-valStart >= 4 && row[valStart] == 't' && row[valStart+1] == 'r' && row[valStart+2] == 'u' && row[valStart+3] == 'e')
					}
					inVal = false
				}
			}
		}

		v := uint64((user&1023)+score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}