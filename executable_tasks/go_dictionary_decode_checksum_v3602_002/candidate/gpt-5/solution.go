package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// Preserve baseline behavior: if iters == 0, no work is done and h remains 0
	if iters == 0 {
		return 0
	}

	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	h := offset
	is := ids
	dv := dictVals
	n := len(is)
	i := 0
	for ; i+4 <= n; i += 4 {
		id0 := is[i]
		v0 := uint64(dv[id0]) + 1
		h ^= v0
		h *= prime

		id1 := is[i+1]
		v1 := uint64(dv[id1]) + 1
		h ^= v1
		h *= prime

		id2 := is[i+2]
		v2 := uint64(dv[id2]) + 1
		h ^= v2
		h *= prime

		id3 := is[i+3]
		v3 := uint64(dv[id3]) + 1
		h ^= v3
		h *= prime
	}
	for ; i < n; i++ {
		id := is[i]
		v := uint64(dv[id]) + 1
		h ^= v
		h *= prime
	}
	return h
}
