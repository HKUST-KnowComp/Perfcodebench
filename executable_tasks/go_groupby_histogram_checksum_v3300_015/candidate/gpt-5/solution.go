package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	hist := make([]uint64, int(buckets))
	n := len(keys)
	var h uint64

	for iter := 0; iter < iters; iter++ {
		// Accumulate into histogram
		hst := hist
		ks := keys
		vs := vals

		i := 0
		limit := n &^ 3
		for i < limit {
			k0 := ks[i]
			k1 := ks[i+1]
			k2 := ks[i+2]
			k3 := ks[i+3]
			hst[k0] += uint64(vs[i])
			hst[k1] += uint64(vs[i+1])
			hst[k2] += uint64(vs[i+2])
			hst[k3] += uint64(vs[i+3])
			i += 4
		}
		for i < n {
			hst[ks[i]] += uint64(vs[i])
			i++
		}

		// Compute hash and clear histogram for next iteration in one pass
		h = offset
		m := len(hst)
		j := 0
		limit2 := m &^ 3
		for j < limit2 {
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
			j += 4
		}
		for j < m {
			v := hst[j]
			h ^= v + 1
			h *= prime
			hst[j] = 0
			j++
		}
	}
	return h
}
