package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const initHash = uint64(1469598103934665603)
	const hashMul = uint64(1099511628211)
	var h uint64
	for iter := 0; iter < iters; iter++ {
		h = initHash
		for _, id := range ids {
			v := dictVals[id]
			h ^= uint64(v) + 1
			h *= hashMul
		}
	}
	return h
}