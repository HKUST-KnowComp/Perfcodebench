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
		b := []byte(row)
		user, score := 0, 0
		flag := false

		// one linear pass over the row bytes
		i := 0
		for i < len(b) {
			// fast‐forward to next key start
			for i < len(b) && b[i] != '"' {
				i++
			}
			if i+6 >= len(b) {
				break
			}
			tok := b[i+1]
			switch tok {
			case 'u': // "user"
				if i+6 < len(b) && b[i+2] == 's' && b[i+3] == 'e' && b[i+4] == 'r' && b[i+5] == '"' && b[i+6] == ':' {
					i += 7
					for i < len(b) && (b[i] < '0' || b[i] > '9') {
						i++
					}
					for i < len(b && b[i] >= '0' && b[i] <= '9' {
						user = user*10 + int(b[i]-'0')
						i++
					}
				}
			case 's': // "score"
				if i+7 < len(b) && b[i+2] == 'c' && b[i+3] == 'o' && b[i+4] == 'r' && b[i+5] == 'e' && b[i+6] == '"' && b[i+7] == ':' {
					i += 8
					for i < len(b) && (b[i] < '0' || b[i] > '9') {
						i++
					}
					for i < len(b) && b[i] >= '0' && b[i] <= '9' {
						score = score*10 + int(b[i]-'0')
						i++
					}
				}
			case '': // "true" (flag)
				if i+5 < len(b) && b[i+2] == 'r' && b[i+3] == 'u' && b[i+4] == 'e' && b[i+5] == '"' {
					flag = true
					i += 6
				}
			}
			i++
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