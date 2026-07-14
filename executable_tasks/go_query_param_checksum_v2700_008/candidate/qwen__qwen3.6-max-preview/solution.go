package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag int
		i := 0
		n := len(row)

		for i < n && row[i] != '=' {
			i++
		}
		i++
		for i < n && row[i] >= '0' && row[i] <= '9' {
			u = u*10 + int(row[i]-'0')
			i++
		}

		for i < n && row[i] != '=' {
			i++
		}
		i++
		for i < n && row[i] >= '0' && row[i] <= '9' {
			score = score*10 + int(row[i]-'0')
			i++
		}

		for i < n && row[i] != '=' {
			i++
		}
		i++
		for i < n && row[i] >= '0' && row[i] <= '9' {
			flag = flag*10 + int(row[i]-'0')
			i++
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
