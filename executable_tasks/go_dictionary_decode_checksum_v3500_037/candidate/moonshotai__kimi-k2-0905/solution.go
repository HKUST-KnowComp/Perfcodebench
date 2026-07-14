package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const seed   uint64 = 1469598103934665603
	const factor uint64 = 1099511628211

	var h uint64 = seed
	for iter := 0; iter < iters; iter++ {
		h = seed
		for _, id := range ids {
			v := dictVals[id]
			h = (h ^ uint64(v+1)) * factor
		}
	}
	return h
}