package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	h := offset

	n := len(ids)
	i := 0
	for n-i >= 4 {
		v0 := dictVals[ids[i]]
		h ^= uint64(v0) + 1
		h *= prime

		v1 := dictVals[ids[i+1]]
		h ^= uint64(v1) + 1
		h *= prime

		v2 := dictVals[ids[i+2]]
		h ^= uint64(v2) + 1
		h *= prime

		v3 := dictVals[ids[i+3]]
		h ^= uint64(v3) + 1
		h *= prime

		i += 4
	}
	for ; i < n; i++ {
		v := dictVals[ids[i]]
		h ^= uint64(v) + 1
		h *= prime
	}
	return h
}
