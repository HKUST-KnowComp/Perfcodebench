package main

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	hash := uint64(fnvOffset)

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		inName := true
		nameStart := 0
		i := 0
		n := len(row)

		for i < n {
			c := row[i]
			switch c {
			case '"':
				if inName {
					// entering field name
					nameStart = i + 1
					inName = false
				} else {
					// closing field name
					field := row[nameStart:i]
					// skip to colon
					i++
					for i < n && row[i] != ':' {
						i++
					}
					i++ // skip colon
					// skip optional space
					for i < n && row[i] == ' ' {
						i++
					}
					// parse value
					switch field {
					case "user":
						v := 0
						for i < n && row[i] >= '0' && row[i] <= '9' {
							v = v*10 + int(row[i]-'0')
							i++
						}
						user = v
					case "score":
						v := 0
						for i < n && row[i] >= '0' && row[i] <= '9' {
							v = v*10 + int(row[i]-'0')
							i++
						}
						score = v
					case "flag":
						if i+4 <= n && row[i:i+4] == "true" {
							flag = true
							i += 4
						} else {
							// false or malformed; ignore
							for i < n && row[i] != ',' && row[i] != '}' {
								i++
							}
						}
					default:
						// unknown field; skip value
						for i < n && row[i] != ',' && row[i] != '}' {
							i++
						}
					}
					// skip to next field or end
					for i < n && row[i] != ',' && row[i] != '}' {
						i++
					}
					inName = true
				}
			case ',', '}':
				inName = true
			}
			i++
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}