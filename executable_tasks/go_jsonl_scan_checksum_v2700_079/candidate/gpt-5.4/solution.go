package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	hash := offset64
	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Scan once through the row, looking for the exact field names.
		for i := 0; i < len(row); {
			if row[i] != '"' {
				i++
				continue
			}

			// Fast exact matches for the expected keys.
			if i+7 <= len(row) && row[i:i+7] == "\"user\"" {
				j := i + 7
				for j < len(row) && (row[j] == ' ' || row[j] == ':') {
					j++
				}
				v := 0
				for j < len(row) {
					c := row[j]
					if c < '0' || c > '9' {
						break
					}
					v = v*10 + int(c-'0')
					j++
				}
				user = v
				i = j
				continue
			}

			if i+8 <= len(row) && row[i:i+8] == "\"score\"" {
				j := i + 8
				for j < len(row) && (row[j] == ' ' || row[j] == ':') {
					j++
				}
				v := 0
				for j < len(row) {
					c := row[j]
					if c < '0' || c > '9' {
						break
					}
					v = v*10 + int(c-'0')
					j++
				}
				score = v
				i = j
				continue
			}

			if i+7 <= len(row) && row[i:i+7] == "\"flag\"" {
				j := i + 7
				for j < len(row) && (row[j] == ' ' || row[j] == ':') {
					j++
				}
				if j+4 <= len(row) && row[j:j+4] == "true" {
					flag = true
				}
				i = j
				continue
			}

			i++
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}
