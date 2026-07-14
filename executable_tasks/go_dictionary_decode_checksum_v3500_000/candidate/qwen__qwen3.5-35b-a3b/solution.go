package main

func run(ids, dictVals []uint32, iters int) uint64 {
	decoded := make([]uint32, len(ids))
	for i, id := range ids {
		decoded[i] = dictVals[id]
	}
	var h uint64 = 1469598103934665603
	for _, v := range decoded {
		h ^= uint64(v) + 1
		h *= 1099511628211
	}
	return h
}