package main

func run(ids, dictVals []uint32, iters int) uint64 {
	// Preserve baseline behavior when no iterations are requested
	if iters <= 0 {
		return 0
	}

	// Compute the checksum once since it is identical across iterations
	const (
		offset = uint64(1469598103934665603)
		prime  = uint64(1099511628211)
	)

	h := offset
	dv := dictVals
	id := ids
	n := len(id)

	// Unroll by 8 for better throughput
	i := 0
	n8 := n &^ 7
	for i < n8 {
		id0 := id[i]
		id1 := id[i+1]
		id2 := id[i+2]
		id3 := id[i+3]
		id4 := id[i+4]
		id5 := id[i+5]
		id6 := id[i+6]
		id7 := id[i+7]

		h ^= uint64(dv[id0]) + 1
		h *= prime
		h ^= uint64(dv[id1]) + 1
		h *= prime
		h ^= uint64(dv[id2]) + 1
		h *= prime
		h ^= uint64(dv[id3]) + 1
		h *= prime
		h ^= uint64(dv[id4]) + 1
		h *= prime
		h ^= uint64(dv[id5]) + 1
		h *= prime
		h ^= uint64(dv[id6]) + 1
		h *= prime
		h ^= uint64(dv[id7]) + 1
		h *= prime

		i += 8
	}
	for ; i < n; i++ {
		h ^= uint64(dv[id[i]]) + 1
		h *= prime
	}

	return h
}
