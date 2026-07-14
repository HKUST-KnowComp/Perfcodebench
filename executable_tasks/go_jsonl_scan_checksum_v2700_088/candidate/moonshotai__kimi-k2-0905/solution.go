package main

func run(rows []string) uint64 {
	const (
		userKey   = `"user":`
		scoreKey  = `"score":`
		flagKey   = `"flag":true`
		keyLenU   = len(userKey)
		keyLenS   = len(scoreKey)
		keyLenF   = len(flagKey)
	)

	var hash uint64 = 1469598103934665603

	for _, line := range rows {
		var user, score int
		var flag bool

		l := len(line)
		i := 0

		for i < l {
			// user field
			if i+keyLenU <= l && line[i:i+keyLenU] == userKey {
				i += keyLenU
				for i < l && (line[i] < '0' || line[i] > '9') {
					i++
				}
				for i < l && line[i] >= '0' && line[i] <= '9' {
					user = user*10 + int(line[i]-'0')
					i++
				}
				continue
			}

			// score field
			if i+keyLenS <= l && line[i:i+keyLenS] == scoreKey {
				i += keyLenS
				for i < l && (line[i] < '0' || line[i] > '9') {
					i++
				}
				for i < l && line[i] >= '0' && line[i] <= '9' {
					score = score*10 + int(line[i]-'0')
					i++
				}
				continue
			}

			// flag field
			if i+keyLenF <= l && line[i:i+keyLenF] == flagKey {
				flag = true
				i += keyLenF
				continue
			}

			i++
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}

	return hash
}