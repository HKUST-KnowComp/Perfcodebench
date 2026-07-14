package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}

	// FNV-1a 64-bit parameters
	var h uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	// Single pass: decode and hash directly
	for i := 0; i < len(ids); i++ {
		v := dictVals[ids[i]]
		h ^= uint64(v) + 1
		h *= prime
	}
	return h
}
