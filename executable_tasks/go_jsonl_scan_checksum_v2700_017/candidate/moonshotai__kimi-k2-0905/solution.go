package main

func run(rows []string) uint64 {
	const (
		initHash = 1469598103934665603
		prime    = 1099511628211
		mask     = 1023
		bias     = 17
	)

	hash := initHash

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		b := []byte(row)
		i := 0
		n := len(b)

		for i < n {
			// fast-forward to a quote
			for i < n && b[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++
			if i+4 > n {
				break
			}

			// "user"
			if b[i] == 'u' && i+4 <= n && b[i+1] == 's' && b[i+2] == 'e' && b[i+3] == 'r' {
				i += 4
				// skip to colon
				for i < n && b[i] != ':' {
					i++
				}
				if i < n {
					i++
					// skip optional spaces
					for i < n && (b[i] == ' ' || b[i] == '\t') {
						i++
					}
					// skip optional quote
					if i < n && b[i] == '"' {
						i++
					}
					v := 0
					for i < n && b[i] >= '0' && b[i] <= '9' {
						v = v*10 + int(b[i]-'0')
						i++
					}
					user = v
				}
				continue
			}

			// "score"
			if b[i] == 's' && i+5 <= n && b[i+1] == 'c' && b[i+2] == 'o' && b[i+3] == 'r' && b[i+4] == 'e' {
				i += 5
				// skip to colon
				for i < n && b[i] != ':' {
					i++
				}
				if i < n {
					i++
					// skip optional spaces
					for i < n && (b[i] == ' ' || b[i] == '\t') {
						i++
					}
					// skip optional quote
					if i < n && b[i] == '"' {
						i++
					}
					v := 0
					for i < n && b[i] >= '0' && b[i] <= '9' {
						v = v*10 + int(b[i]-'0')
						i++
					}
					score = v
				}
				continue
			}

			// "flag"
			if b[i] == 'f' && i+4 <= n && b[i+1] == 'l' && b[i+2] == 'a' && b[i+3] == 'g' {
				i += 4
				// skip to colon
				for i < n && b[i] != ':' {
					i++
				}
				if i < n {
					i++
					// skip optional spaces
					for i < n && (b[i] == ' ' || b[i] == '\t') {
						i++
					}
					if i+4 <= n && b[i] == 't' && b[i+1] == 'r' && b[i+2] == 'u' && b[i+3] == 'e' {
						flag = true
						i += 4
					}
				}
				continue
			}
		}

		v := uint64((user & mask) + score)
		if flag {
			v += bias
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}