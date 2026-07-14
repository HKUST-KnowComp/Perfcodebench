package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// If no iterations requested, match baseline behavior: return 0 without work.
	if iters <= 0 {
		return 0
	}

	const (
		offset = uint64(1469598103934665603)
		prime  = uint64(1099511628211)
	)

	h := offset
	n := len(ids)
	i := 0

	// Unroll by 8 for better ILP and fewer loop overheads.
	for n-i >= 8 {
		id0 := ids[i]
		id1 := ids[i+1]
		id2 := ids[i+2]
		id3 := ids[i+3]
		id4 := ids[i+4]
		id5 := ids[i+5]
		id6 := ids[i+6]
		id7 := ids[i+7]

		v0 := uint64(dictVals[id0]) + 1
		v1 := uint64(dictVals[id1]) + 1
		v2 := uint64(dictVals[id2]) + 1
		v3 := uint64(dictVals[id3]) + 1
		v4 := uint64(dictVals[id4]) + 1
		v5 := uint64(dictVals[id5]) + 1
		v6 := uint64(dictVals[id6]) + 1
		v7 := uint64(dictVals[id7]) + 1

		h ^= v0
		h *= prime
		h ^= v1
		h *= prime
		h ^= v2
		h *= prime
		h ^= v3
		h *= prime
		h ^= v4
		h *= prime
		h ^= v5
		h *= prime
		h ^= v6
		h *= prime
		h ^= v7
		h *= prime

		i += 8
	}

	for i < n {
		v := uint64(dictVals[ids[i]]) + 1
		h ^= v
		h *= prime
		i++
	}

	return h
}
