package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const seed uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	var h uint64
	
	dv := dictVals
	n := len(ids)
	for iter := 0; iter < iters; iter++ {
		h = seed
		i := 0
		for i+7 < n {
			v0 := dv[ids[i]]
			v1 := dv[ids[i+1]]
			v2 := dv[ids[i+2]]
			v3 := dv[ids[i+3]]
			v4 := dv[ids[i+4]]
			v5 := dv[ids[i+5]]
			v6 := dv[ids[i+6]]
			v7 := dv[ids[i+7]]

			h ^= uint64(v0) + 1
			h *= prime
			h ^= uint64(v1) + 1
			h *= prime
			h ^= uint64(v2) + 1
			h *= prime
			h ^= uint64(v3) + 1
			h *= prime
			h ^= uint64(v4) + 1
			h *= prime
			h ^= uint64(v5) + 1
			h *= prime
			h ^= uint64(v6) + 1
			h *= prime
			h ^= uint64(v7) + 1
			h *= prime

			i += 8
		}
		for ; i < n; i++ {
			h ^= uint64(dv[ids[i]]) + 1
			h *= prime
		}
	}
	return h
}
