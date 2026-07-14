package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters == 0 {
		return 0
	}

	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	h := offset
	dv := dictVals
	idsLocal := ids
	n := len(idsLocal)
	i := 0
	for ; i+7 < n; i += 8 {
		id0 := idsLocal[i]
		id1 := idsLocal[i+1]
		id2 := idsLocal[i+2]
		id3 := idsLocal[i+3]
		id4 := idsLocal[i+4]
		id5 := idsLocal[i+5]
		id6 := idsLocal[i+6]
		id7 := idsLocal[i+7]

		v0 := uint64(dv[id0]) + 1
		h ^= v0
		h *= prime
		v1 := uint64(dv[id1]) + 1
		h ^= v1
		h *= prime
		v2 := uint64(dv[id2]) + 1
		h ^= v2
		h *= prime
		v3 := uint64(dv[id3]) + 1
		h ^= v3
		h *= prime
		v4 := uint64(dv[id4]) + 1
		h ^= v4
		h *= prime
		v5 := uint64(dv[id5]) + 1
		h ^= v5
		h *= prime
		v6 := uint64(dv[id6]) + 1
		h ^= v6
		h *= prime
		v7 := uint64(dv[id7]) + 1
		h ^= v7
		h *= prime
	}
	for ; i < n; i++ {
		v := uint64(dv[idsLocal[i]]) + 1
		h ^= v
		h *= prime
	}

	return h
}
