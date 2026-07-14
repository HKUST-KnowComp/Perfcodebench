package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	// Allocate histogram once and reuse across iterations.
	hist := make([]uint64, buckets)
	blen := int(buckets)
	n := len(keys)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Local aliases to help the compiler and reduce bounds checks.
		hk := hist
		kv := keys
		vv := vals

		// Unrolled accumulation for better throughput.
		limit := n - n%4
		i := 0
		for ; i < limit; i += 4 {
			k0 := kv[i]
			hk[k0] += uint64(vv[i])
			k1 := kv[i+1]
			hk[k1] += uint64(vv[i+1])
			k2 := kv[i+2]
			hk[k2] += uint64(vv[i+2])
			k3 := kv[i+3]
			hk[k3] += uint64(vv[i+3])
		}
		for ; i < n; i++ {
			hk[kv[i]] += uint64(vv[i])
		}

		// Compute FNV-like checksum while resetting the histogram for the next iteration.
		h = offset
		for j := 0; j < blen; j++ {
			v := hk[j]
			h ^= v + 1
			h *= prime
			hk[j] = 0
		}
	}
	return h
}
