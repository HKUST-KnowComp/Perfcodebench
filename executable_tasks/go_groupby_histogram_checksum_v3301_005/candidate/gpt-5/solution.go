package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Preallocate histogram once and reuse across iterations.
	hist := make([]uint64, buckets)

	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	var h uint64
	n := len(keys)

	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram; unroll by 4 for throughput.
		i := 0
		for i+3 < n {
			k0 := keys[i]
			k1 := keys[i+1]
			k2 := keys[i+2]
			k3 := keys[i+3]
			hist[k0] += uint64(vals[i])
			hist[k1] += uint64(vals[i+1])
			hist[k2] += uint64(vals[i+2])
			hist[k3] += uint64(vals[i+3])
			i += 4
		}
		for i < n {
			hist[keys[i]] += uint64(vals[i])
			i++
		}

		// Compute hash while clearing histogram for next iteration.
		h = offset
		for j := uint32(0); j < buckets; j++ {
			v := hist[j]
			h ^= v + 1
			h *= prime
			hist[j] = 0
		}
	}

	return h
}
