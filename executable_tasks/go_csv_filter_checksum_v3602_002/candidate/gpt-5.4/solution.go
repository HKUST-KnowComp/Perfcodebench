package main

func run(rows []string) uint64 {
	const (
		offset64 = 1469598103934665603
		prime64  = 1099511628211
	)

	hash := uint64(offset64)
	for _, row := range rows {
		field := 0
		val := 11
		num := 0
		neg := false
		hot := false

		for i := 0; i <= len(row); i++ {
			var ch byte
			if i < len(row) {
				ch = row[i]
			} else {
				ch = ',' // flush last field
			}

			if ch == ',' {
				switch field {
				case 2, 3:
					if neg {
						val -= num
					} else {
						val += num
					}
				case 4:
					// baseline semantics: hot iff field exactly equals "hot"
					// detect during scan without allocation
					if i >= 3 {
						start := i - 3
						if start >= 0 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
							// ensure field length is exactly 3
							prevComma := -1
							for j := start - 1; j >= 0; j-- {
								if row[j] == ',' {
									prevComma = j
									break
								}
							}
							if prevComma == start-1 {
								hot = true
							}
						}
					}
				}
				field++
				num = 0
				neg = false
				continue
			}

			if field == 2 || field == 3 {
				if ch == '-' && num == 0 {
					neg = true
				} else {
					num = num*10 + int(ch-'0')
				}
			}
		}

		if hot {
			val += 29
		}
		hash ^= uint64(val + 1)
		hash *= prime64
	}
	return hash
}
