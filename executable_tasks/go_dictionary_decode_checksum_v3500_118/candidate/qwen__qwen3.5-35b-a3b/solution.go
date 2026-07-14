package main

func run(ids, dictVals []uint32, iters int) uint64 {
	decoded := make([]uint32, len(ids))
	var h uint64 = 1469598103934665603
	for i, id := range ids {
		decoded[i] = dictVals[id]
	}
	for iter := 0; iter < iters; iter++ {
		for _, v := range decoded {
			h ^= uint64(v) + 1
			h *= 1099511628211
		}
	}
	return h
}