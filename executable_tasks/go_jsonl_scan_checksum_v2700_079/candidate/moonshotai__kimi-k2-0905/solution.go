package main

func run(rows []string) uint64 {
	const (
		FNV_OFFSET = 1469598103934665603
		FNV_PRIME  = 1099511628211
		USER_MASK  = 1023
		FLAG_DELTA = 17
	)

	hash := uint64(FNV_OFFSET)

	for _, row := range rows {
		b := []byte(row)
		user, score := 0, 0
		flag := false

		i := 0
		n := len(b)

		for i < n {
			// fast-path: skip until '"'
			for i < n && b[i] != '"' {
				i++
			}
			if i+6 >= n { // shortest key is 6 chars ("flag":)
				break
			}
			i++ // skip '"'

			// dispatch on first char of key
			switch b[i] {
			case 'u': // "user":
				if i+5 < n && b[i+1] == 's' && b[i+2] == 'e' && b[i+3] == 'r' && b[i+4] == '"' && b[i+5] == ':' {
					i += ixDigit(&b, i+6, &user)
				}
			case 's': // "score":
				if i+6 < n && b[i+1] == 'c' && b[i+2] == 'o' && b[i+3] == 'o' && b[i+4] == 'r' && b[i+5] == 'e' && b[i+6] == '"' && b[i+7] == ':' {
					i += asixDigit(&b, i+8, &score)
				}
			case 'f': // "flag":true
				if i+5 < n && b[i+1] == 'l' && b[i+2] == 'a' && b[i+3] == 'g' && b[i+4] == '"' && b[i+5] == ':' {
					j := i + 6
					skipSpace(&b, &j)
					if j+3 < n && b[j] == 't' && b[j+1] == 'r' && b[j+2] == 'u' && b[j+3] == 'e' {
						flag = true
					}
				}
			default:
				i++
			}
		}

		v := uint64((user & USER_MASK) + score)
		if flag {
			v += FLAG_DELTA
		}
		hash ^= v + 1
		hash *= FNV_PRIME
	}
	return hash
}

// asixDigit advances index and fills *out
func asixDigit(b *[]byte, start int, out *int) int {
	i := start
	for i < len(*b) {
		c := (*b)[i]
		if c < '0' || c > '9' {
			break
		}
		*out = *out*10 + int(c-'0')
		i++
	}
	return i - start
}

// alias for user
func ixDigit(b *[]byte, start int, out *int) int { return asixDigit(b, start, out) }

func skipSpace(b *[]byte, idx *int) {
	for *idx < len(*b) && ((*b)[*idx] == ' ' || (*b)[*idx] == '\t') {
		*idx++
	}
}