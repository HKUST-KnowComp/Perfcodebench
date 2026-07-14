package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	const (
		fnvOffset uint64 = 1469598103934665603
		fnvPrime  uint64 = 1099511628211
	)

	// Allocate histogram once and reuse across iterations.
	hist := make([]uint64, buckets)
	nb := int(buckets)

	var h uint64
	for iter := 0; iter < iters; iter++ {
		// Build histogram with loop unrolling.
		n := len(keys)
		m := n - n%4
		for i := 0; i < m; i += 4 {
			k0 := int(keys[i])
			k1 := int(keys[i+1])
			k2 := int(keys[i+2])
			k3 := int(keys[i+3])
			hist[k0] += uint64(vals[i])
			hist[k1] += uint64(vals[i+1])
			hist[k2] += uint64(vals[i+2])
			hist[k3] += uint64(vals[i+3])
		}
		for i := m; i < n; i++ {
			hist[int(keys[i])] += uint64(vals[i])
		}

		// Compute checksum and clear histogram in the same pass.
		h = fnvOffset
		j := 0
		for ; j+4 <= nb; j += 4 {
			v0 := hist[j]
			v1 := hist[j+1]
			v2 := hist[j+2]
			v3 := hist[j+3]

			h ^= v0 + 1
			h *= fnvPrime
			h ^= v1 + 1
			h *= fnvPrime
			h ^= v2 + 1
			h *= fnvPrime
			h ^= v3 + 1
			h *= fnvPrime

			hist[j] = 0
			hist[j+1] = 0
			hist[j+2] = 0
			hist[j+3] = 0
		}
		for ; j < nb; j++ {
			v := hist[j]
			h ^= v + 1
			h *= fnvPrime
			hist[j] = 0
		}
	}
	return h
}
