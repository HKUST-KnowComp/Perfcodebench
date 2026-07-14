package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// Preallocate histogram once and reuse across iterations
	hist := make([]uint64, int(buckets))
	var h uint64
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	n := len(keys)

	for iter := 0; iter < iters; iter++ {
		// Accumulate values into histogram
		hst := hist
		i := 0
		// Unroll by 4 for better ILP
		for ; i+4 <= n; i += 4 {
			k0 := int(keys[i])
			k1 := int(keys[i+1])
			k2 := int(keys[i+2])
			k3 := int(keys[i+3])
			hst[k0] += uint64(vals[i])
			hst[k1] += uint64(vals[i+1])
			hst[k2] += uint64(vals[i+2])
			hst[k3] += uint64(vals[i+3])
		}
		for ; i < n; i++ {
			hst[int(keys[i])] += uint64(vals[i])
		}

		// Compute hash over histogram and clear it for next iteration
		h = offset
		m := len(hst)
		j := 0
		for ; j+4 <= m; j += 4 {
			v0 := hst[j]
			v1 := hst[j+1]
			v2 := hst[j+2]
			v3 := hst[j+3]

			h ^= v0 + 1
			h *= prime
			h ^= v1 + 1
			h *= prime
			h ^= v2 + 1
			h *= prime
			h ^= v3 + 1
			h *= prime

			hst[j] = 0
			hst[j+1] = 0
			hst[j+2] = 0
			hst[j+3] = 0
		}
		for ; j < m; j++ {
			v := hst[j]
			h ^= v + 1
			h *= prime
			hst[j] = 0
		}
	}
	return h
}
