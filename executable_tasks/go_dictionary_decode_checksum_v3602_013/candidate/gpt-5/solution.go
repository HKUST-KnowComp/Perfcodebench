package main

func run(ids, dictVals []uint32, iters int) uint64 {
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	var h uint64
	n := len(ids)
	dv := dictVals
	s := ids

	for iter := 0; iter < iters; iter++ {
		h = offset
		i := 0
		for ; i+8 <= n; i += 8 {
			v0 := uint64(dv[s[i+0]]) + 1
			h ^= v0
			h *= prime

			v1 := uint64(dv[s[i+1]]) + 1
			h ^= v1
			h *= prime

			v2 := uint64(dv[s[i+2]]) + 1
			h ^= v2
			h *= prime

			v3 := uint64(dv[s[i+3]]) + 1
			h ^= v3
			h *= prime

			v4 := uint64(dv[s[i+4]]) + 1
			h ^= v4
			h *= prime

			v5 := uint64(dv[s[i+5]]) + 1
			h ^= v5
			h *= prime

			v6 := uint64(dv[s[i+6]]) + 1
			h ^= v6
			h *= prime

			v7 := uint64(dv[s[i+7]]) + 1
			h ^= v7
			h *= prime
		}
		for ; i < n; i++ {
			h ^= uint64(dv[s[i]]) + 1
			h *= prime
		}
	}
	return h
}
