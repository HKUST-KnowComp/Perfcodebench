package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Allocate histogram once and reuse across iterations.
	hist := make([]uint64, int(buckets))
	var h uint64

	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram
		k := keys
		v := vals
		n := len(k)
		// Unroll by 4 for better throughput
		limit := n &^ 3 // n - n%4
		for i := 0; i < limit; i += 4 {
			hist[k[i]] += uint64(v[i])
			hist[k[i+1]] += uint64(v[i+1])
			hist[k[i+2]] += uint64(v[i+2])
			hist[k[i+3]] += uint64(v[i+3])
		}
		for i := limit; i < n; i++ {
			hist[k[i]] += uint64(v[i])
		}

		// Compute checksum and simultaneously zero histogram for the next iteration
		h = 1469598103934665603
		const prime uint64 = 1099511628211
		L := len(hist)
		limitZ := L &^ 3
		for i := 0; i < limitZ; i += 4 {
			v0 := hist[i]
			h ^= v0 + 1
			h *= prime
			v1 := hist[i+1]
			h ^= v1 + 1
			h *= prime
			v2 := hist[i+2]
			h ^= v2 + 1
			h *= prime
			v3 := hist[i+3]
			h ^= v3 + 1
			h *= prime
			hist[i] = 0
			hist[i+1] = 0
			hist[i+2] = 0
			hist[i+3] = 0
		}
		for i := limitZ; i < L; i++ {
			vv := hist[i]
			h ^= vv + 1
			h *= prime
			hist[i] = 0
		}
	}
	return h
}
