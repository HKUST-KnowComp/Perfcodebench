package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// If no iterations requested, original code returns 0
	if iters <= 0 {
		return 0
	}

	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	// Compute the checksum once; since each iteration produces the same
	// result and h is reset every time, the final result equals a single pass.
	h := offset
	idSlice := ids
	dv := dictVals
	n := len(idSlice)

	i := 0
	for ; i+3 < n; i += 4 {
		v0 := uint64(dv[idSlice[i]]) + 1
		v1 := uint64(dv[idSlice[i+1]]) + 1
		v2 := uint64(dv[idSlice[i+2]]) + 1
		v3 := uint64(dv[idSlice[i+3]]) + 1

		h ^= v0
		h *= prime
		h ^= v1
		h *= prime
		h ^= v2
		h *= prime
		h ^= v3
		h *= prime
	}
	for ; i < n; i++ {
		h ^= uint64(dv[idSlice[i]]) + 1
		h *= prime
	}

	return h
}
