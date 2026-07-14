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

		// single pass over the row
		i := 0
		n := len(row)
		for i < n {
			// find next key start '"'
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip opening quote
			keyStart := i
			// find closing quote
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			key := row[keyStart:i]
			i++ // skip closing quote

			// expect colon
			if i >= n || row[i] != ':' {
				continue
			}
			i++ // skip colon

			switch key {
			case "user":
				// parse integer value
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			case "score":
				// parse integer value
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			case "flag":
				// check for literal true
				if i+3 <= n && row[i] == 't' && row[i+1] == 'r' && row[i+2] == 'u' && row[i+3] == 'e' {
					flag = true
					i += 4
				}
			default:
				// skip value by finding next comma or end
				for i < n && row[i] != ',' {
					i++
				}
			}
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