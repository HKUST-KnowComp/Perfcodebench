package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	nBuckets := int(buckets)
	hist := make([]uint64, nBuckets)

	var h uint64
	nKeys := len(keys)

	for iter := 0; iter < iters; iter++ {
		// Accumulate into histogram with unrolling
		i := 0
		for ; i+4 <= nKeys; i += 4 {
			k0 := keys[i]
			k1 := keys[i+1]
			k2 := keys[i+2]
			k3 := keys[i+3]
			hist[k0] += uint64(vals[i])
			hist[k1] += uint64(vals[i+1])
			hist[k2] += uint64(vals[i+2])
			hist[k3] += uint64(vals[i+3])
		}
		for ; i < nKeys; i++ {
			k := keys[i]
			hist[k] += uint64(vals[i])
		}

		// Compute checksum while clearing histogram for next iteration
		h = fnvOffset
		j := 0
		for ; j+4 <= nBuckets; j += 4 {
			v0 := hist[j]
			h ^= v0 + 1
			h *= fnvPrime
			hist[j] = 0

			v1 := hist[j+1]
			h ^= v1 + 1
			h *= fnvPrime
			hist[j+1] = 0

			v2 := hist[j+2]
			h ^= v2 + 1
			h *= fnvPrime
			hist[j+2] = 0

			v3 := hist[j+3]
			h ^= v3 + 1
			h *= fnvPrime
			hist[j+3] = 0
		}
		for ; j < nBuckets; j++ {
			v := hist[j]
			h ^= v + 1
			h *= fnvPrime
			hist[j] = 0
		}
	}
	return h
}
