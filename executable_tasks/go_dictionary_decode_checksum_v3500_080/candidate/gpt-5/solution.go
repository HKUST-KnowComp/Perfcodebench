package main

func run(ids, dictVals []uint32, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	h := offset
	n := len(ids)
	i := 0
	for ; i+7 < n; i += 8 {
		idx0 := int(ids[i])
		idx1 := int(ids[i+1])
		idx2 := int(ids[i+2])
		idx3 := int(ids[i+3])
		idx4 := int(ids[i+4])
		idx5 := int(ids[i+5])
		idx6 := int(ids[i+6])
		idx7 := int(ids[i+7])

		h ^= uint64(dictVals[idx0]) + 1
		h *= prime
		h ^= uint64(dictVals[idx1]) + 1
		h *= prime
		h ^= uint64(dictVals[idx2]) + 1
		h *= prime
		h ^= uint64(dictVals[idx3]) + 1
		h *= prime
		h ^= uint64(dictVals[idx4]) + 1
		h *= prime
		h ^= uint64(dictVals[idx5]) + 1
		h *= prime
		h ^= uint64(dictVals[idx6]) + 1
		h *= prime
		h ^= uint64(dictVals[idx7]) + 1
		h *= prime
	}
	for ; i < n; i++ {
		h ^= uint64(dictVals[int(ids[i])]) + 1
		h *= prime
	}
	return h
}
