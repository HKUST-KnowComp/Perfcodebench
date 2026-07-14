package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user, score, flag := scanRow(row)
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func scanRow(s string) (user, score int, flag bool) {
	n := len(s)
	for i := 0; i < n; i++ {
		if s[i] != '"' {
			continue
		}

		j := i + 1
		for j < n && s[j] != '"' {
			j++
		}
		if j >= n {
			break
		}

		keyStart := i + 1
		keyLen := j - keyStart
		k := j + 1

		for k < n {
			c := s[k]
			if c == ':' {
				k++
				break
			}
			if c != ' ' && c != '\t' {
				break
			}
			k++
		}
		if k > n {
			break
		}
		for k < n && (s[k] == ' ' || s[k] == '\t') {
			k++
		}
		if k >= n {
			break
		}

		switch keyLen {
		case 4:
			if s[keyStart] == 'u' && s[keyStart+1] == 's' && s[keyStart+2] == 'e' && s[keyStart+3] == 'r' {
				val, next := parseDigits(s, k)
				user = val
				i = next - 1
				continue
			}
			if s[keyStart] == 'f' && s[keyStart+1] == 'l' && s[keyStart+2] == 'a' && s[keyStart+3] == 'g' {
				if k+3 < n && s[k] == 't' && s[k+1] == 'r' && s[k+2] == 'u' && s[k+3] == 'e' {
					flag = true
					i = k + 3
					continue
				}
				if k+4 < n && s[k] == 'f' && s[k+1] == 'a' && s[k+2] == 'l' && s[k+3] == 's' && s[k+4] == 'e' {
					flag = false
					i = k + 4
					continue
				}
			}
		case 5:
			if s[keyStart] == 's' && s[keyStart+1] == 'c' && s[keyStart+2] == 'o' && s[keyStart+3] == 'r' && s[keyStart+4] == 'e' {
				val, next := parseDigits(s, k)
				score = val
				i = next - 1
				continue
			}
		}

		i = j
	}
	return
}

func parseDigits(s string, i int) (int, int) {
	n := len(s)
	v := 0
	for i < n {
		c := s[i]
		if c < '0' || c > '9' {
			break
		}
		v = v*10 + int(c-'0')
		i++
	}
	return v, i
}
