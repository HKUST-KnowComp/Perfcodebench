package main

func run(ids, dictVals []uint32, iters int) uint64 {
	n := len(ids)
	if n == 0 {
		var h uint64
		for iter := 0; iter < iters; iter++ {
			h = 1469598103934665603
		}
		return h
	}

	decoded := make([]uint32, n)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		// Unrolled decode loop (8x)
		i := 0
		for ; i <= n-8; i += 8 {
			decoded[i] = dictVals[ids[i]]
			decoded[i+1] = dictVals[ids[i+1]]
			decoded[i+2] = dictVals[ids[i+2]]
			decoded[i+3] = dictVals[ids[i+3]]
			decoded[i+4] = dictVals[ids[i+4]]
			decoded[i+5] = dictVals[ids[i+5]]
			decoded[i+6] = dictVals[ids[i+6]]
			decoded[i+7] = dictVals[ids[i+7]]
		}
		for ; i < n; i++ {
			decoded[i] = dictVals[ids[i]]
		}

		// FNV-1a style hash computation
		h = 1469598103934665603
		
		// Unrolled hash loop (4x)
		j := 0
		for ; j <= n-4; j += 4 {
			h ^= uint64(decoded[j]) + 1
			h *= 1099511628211
			h ^= uint64(decoded[j+1]) + 1
			h *= 1099511628211
			h ^= uint64(decoded[j+2]) + 1
			h *= 1099511628211
			h ^= uint64(decoded[j+3]) + 1
			h *= 1099511628211
		}
		for ; j < n; j++ {
			h ^= uint64(decoded[j]) + 1
			h *= 1099511628211
		}
	}
	return h
}
