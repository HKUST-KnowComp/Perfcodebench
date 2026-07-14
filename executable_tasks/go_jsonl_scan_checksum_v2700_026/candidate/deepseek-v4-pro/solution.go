package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		user := 0
		score := 0
		flag := false
		i := 0
		n := len(row)
		for i < n {
			// skip commas and spaces
			if row[i] == ',' || row[i] == ' ' {
				i++
				continue
			}
			if row[i] == '"' {
				// read field name
				i++ // skip opening quote
				start := i
				for i < n && row[i] != '"' {
					i++
				}
				fieldName := row[start:i]
				i++ // skip closing quote
				// expect colon, maybe with spaces
				for i < n && (row[i] == ' ' || row[i] == ':') {
					if row[i] == ':' {
						i++
						break
					}
					i++
				}
				// skip spaces after colon if any
				for i < n && row[i] == ' ' {
					i++
				}
				switch fieldName {
				case "user":
					user = 0
					for i < n && row[i] >= '0' && row[i] <= '9' {
						user = user*10 + int(row[i]-'0')
						i++
					}
				case "score":
					score = 0
					for i < n && row[i] >= '0' && row[i] <= '9' {
						score = score*10 + int(row[i]-'0')
						i++
					}
				case "flag":
					if i+3 < n && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
						flag = true
						i += 4
					} else {
						// skip false or unknown
						for i < n && row[i] != ',' {
							i++
						}
					}
				}
				continue
			}
			i++
		}
		v := uint64((user&1023)+score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
