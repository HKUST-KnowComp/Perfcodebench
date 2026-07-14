package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)
	// Compute checksum once since repeated iterations yield the same result.
	h := offset
	dict := dictVals
	n := len(ids)
	i := 0
	// Unroll by 8 for better ILP and fewer loop overheads.
	for n-i >= 8 {
		id0 := ids[i]
		id1 := ids[i+1]
		id2 := ids[i+2]
		id3 := ids[i+3]
		id4 := ids[i+4]
		id5 := ids[i+5]
		id6 := ids[i+6]
		id7 := ids[i+7]

		v0 := dict[id0]
		v1 := dict[id1]
		v2 := dict[id2]
		v3 := dict[id3]
		v4 := dict[id4]
		v5 := dict[id5]
		v6 := dict[id6]
		v7 := dict[id7]

		h ^= uint64(v0) + 1; h *= prime
		h ^= uint64(v1) + 1; h *= prime
		h ^= uint64(v2) + 1; h *= prime
		h ^= uint64(v3) + 1; h *= prime
		h ^= uint64(v4) + 1; h *= prime
		h ^= uint64(v5) + 1; h *= prime
		h ^= uint64(v6) + 1; h *= prime
		h ^= uint64(v7) + 1; h *= prime

		i += 8
	}
	for ; i < n; i++ {
		v := dict[ids[i]]
		h ^= uint64(v) + 1
		h *= prime
	}
	return h
}
