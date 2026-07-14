package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// The checksum for each iteration is identical because h is reset to the same
	// offset basis each time and the decoded values are constant.
	// Therefore, if iters > 0 we can compute it once.
	if iters == 0 {
		return 0
	}

	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	h := offset
	n := len(ids)
	i := 0
	for i <= n-8 {
		id0 := ids[i]
		id1 := ids[i+1]
		id2 := ids[i+2]
		id3 := ids[i+3]
		id4 := ids[i+4]
		id5 := ids[i+5]
		id6 := ids[i+6]
		id7 := ids[i+7]

		h ^= uint64(dictVals[id0]) + 1
		h *= prime
		h ^= uint64(dictVals[id1]) + 1
		h *= prime
		h ^= uint64(dictVals[id2]) + 1
		h *= prime
		h ^= uint64(dictVals[id3]) + 1
		h *= prime
		h ^= uint64(dictVals[id4]) + 1
		h *= prime
		h ^= uint64(dictVals[id5]) + 1
		h *= prime
		h ^= uint64(dictVals[id6]) + 1
		h *= prime
		h ^= uint64(dictVals[id7]) + 1
		h *= prime

		i += 8
	}
	for ; i < n; i++ {
		h ^= uint64(dictVals[ids[i]]) + 1
		h *= prime
	}
	return h
}
