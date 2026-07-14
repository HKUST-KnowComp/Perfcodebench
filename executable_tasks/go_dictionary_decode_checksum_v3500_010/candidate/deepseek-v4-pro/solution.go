package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const (
		h0  uint64 = 1469598103934665603
		mul uint64 = 1099511628211
	)
	h := h0
	n := len(ids)
	if n == 0 {
		return h
	}
	i := 0
	for i+3 < n {
		v0 := dictVals[ids[i]]
		h ^= uint64(v0) + 1
		h *= mul
		v1 := dictVals[ids[i+1]]
		h ^= uint64(v1) + 1
		h *= mul
		v2 := dictVals[ids[i+2]]
		h ^= uint64(v2) + 1
		h *= mul
		v3 := dictVals[ids[i+3]]
		h ^= uint64(v3) + 1
		h *= mul
		i += 4
	}
	for ; i < n; i++ {
		v := dictVals[ids[i]]
		h ^= uint64(v) + 1
		h *= mul
	}
	return h
}
