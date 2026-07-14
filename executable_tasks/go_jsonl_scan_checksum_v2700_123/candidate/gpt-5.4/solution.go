package main

func run(rows []string) uint64 {
	const (
		offset64 = uint64(1469598103934665603)
		prime64  = uint64(1099511628211)
	)

	var hash uint64 = offset64

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		for i := 0; i < len(row); {
			if row[i] != '"' {
				i++
				continue
			}

			j := i + 1
			for j < len(row) && row[j] != '"' {
				j++
			}
			if j >= len(row) {
				break
			}

			keyStart := i + 1
			keyLen := j - keyStart
			k := j + 1

			for k < len(row) && (row[k] == ' ' || row[k] == '\t' || row[k] == '\n' || row[k] == '\r') {
				k++
			}
			if k >= len(row) || row[k] != ':' {
				i = j + 1
				continue
			}
			k++
			for k < len(row) && (row[k] == ' ' || row[k] == '\t' || row[k] == '\n' || row[k] == '\r') {
				k++
			}
			if k >= len(row) {
				break
			}

			switch keyLen {
			case 4:
				if row[keyStart] == 'u' && row[keyStart+1] == 's' && row[keyStart+2] == 'e' && row[keyStart+3] == 'r' {
					v := 0
					for k < len(row) {
						c := row[k]
						if c >= '0' && c <= '9' {
							v = v*10 + int(c-'0')
							k++
							continue
						}
						break
					}
					user = v
					i = k
					continue
				}
			case 5:
				if row[keyStart] == 's' && row[keyStart+1] == 'c' && row[keyStart+2] == 'o' && row[keyStart+3] == 'r' && row[keyStart+4] == 'e' {
					v := 0
					for k < len(row) {
						c := row[k]
						if c >= '0' && c <= '9' {
							v = v*10 + int(c-'0')
							k++
							continue
						}
						break
					}
					score = v
					i = k
					continue
				}
			case 4:
				// unreachable duplicate case kept out by compiler; handled above
			case 4 + 0:
			}

			if keyLen == 4 && row[keyStart] == 'f' && row[keyStart+1] == 'l' && row[keyStart+2] == 'a' && row[keyStart+3] == 'g' {
				if k+3 < len(row) && row[k] == 't' && row[k+1] == 'r' && row[k+2] == 'u' && row[k+3] == 'e' {
					flag = true
					i = k + 4
					continue
				}
				if k+4 < len(row) && row[k] == 'f' && row[k+1] == 'a' && row[k+2] == 'l' && row[k+3] == 's' && row[k+4] == 'e' {
					flag = false
					i = k + 5
					continue
				}
			}

			i = j + 1
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
