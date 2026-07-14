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
			// find next quote
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip opening quote

			// identify field name
			start := i
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			field := row[start:i]
			i++ // skip closing quote

			// skip colon and optional spaces
			for i < n && (row[i] == ' ' || row[i] == ':') {
				i++
			}

			// parse value based on field
			switch field {
			case "user":
				for i < n && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
			case "score":
				for i < n && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
			case "flag":
				if i+4 <= n && row[i:i+4] == "true" {
					flag = true
					i += 4
				} else if i+5 <= n && row[i:i+5] == "false" {
					i += 5
				}
			default:
				// skip unknown field value
				for i < n && row[i] != ',' && row[i] != '}' {
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