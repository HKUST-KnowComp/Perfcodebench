package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if len(ids) == 0 {
		return 0
	}

	var h uint64
	// Pre-calculate constants to assist compiler
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211

	for iter := 0; iter < iters; iter++ {
		h = fnvOffset
		// Single pass: decode and hash simultaneously to maximize cache locality
		// and eliminate the need for the intermediate 'decoded' slice.
		for i := 0; i < len(ids); i++ {
			val := dictVals[ids[i]]
			h ^= uint64(val) + 1
			h *= fnvPrime
		}
	}
	return h
}