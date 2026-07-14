package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	var h uint64
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h = offset
		i := 0
		for ; i+3 < n; i += 4 {
			id0 := ids[i]
			id1 := ids[i+1]
			id2 := ids[i+2]
			id3 := ids[i+3]

			v0 := uint64(dictVals[id0]) + 1
			h ^= v0
			h *= prime

			v1 := uint64(dictVals[id1]) + 1
			h ^= v1
			h *= prime

			v2 := uint64(dictVals[id2]) + 1
			h ^= v2
			h *= prime

			v3 := uint64(dictVals[id3]) + 1
			h ^= v3
			h *= prime
		}
		for ; i < n; i++ {
			v := uint64(dictVals[ids[i]]) + 1
			h ^= v
			h *= prime
		}
	}
	return h
}
