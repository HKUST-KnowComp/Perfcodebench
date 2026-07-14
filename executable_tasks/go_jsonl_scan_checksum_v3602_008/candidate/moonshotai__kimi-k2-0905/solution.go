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
			// find next key start
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

			// skip whitespace and colon
			for i < n && (row[i] == ' ' || row[i] == ':') {
				i++
			}
			if i >= n {
				break
			}

			switch key {
			case "user":
				// parse integer
				v := 0
				for i < n && row[i] >= '0' && row[i] <= '9' {
					v = v*10 + int(row[i]-'0')
					i++
				}
				user = v
			case "score":
				// parse integer
				v := 0
				for i < n && row[i] >= '0' && row[i] <= '9' {
					v = v*10 + int(row[i]-'0')
					i++
				}
				score = v
			case "flag":
				// check for true
				if i+4 <= n && row[i:i+4] == "true" {
					flag = true
					i += 4
				} else if i+5 <= n && row[i:i+5] == "false" {
					i += 5
				}
			default:
				// skip value: scan until next comma or end
				for i < n && row[i] != ',' {
					i++
				}
			}
			// skip to next field
			for i < n && row[i] != ',' {
				i++
			}
			if i < n && row[i] == ',' {
				i++
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