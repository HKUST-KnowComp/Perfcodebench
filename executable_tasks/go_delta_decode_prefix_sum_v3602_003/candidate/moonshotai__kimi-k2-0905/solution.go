package main

func run(deltas []int32, iters int) uint64 {
	const magic = 0x9E3779B1
	dlen := len(deltas)
	total := uint64(0)

	for iter := 0; iter < iters; iter++ {
		var sum int64
		var cur int64
		i := 0

		// main unrolled part (4×)
		for ; i+3 < dlen; i += 4 {
			d0 := int64(deltas[i])
			d1 := int64(deltas[i+1])
			d2 := int64(deltas[i+2])
			d3 := int64(deltas[i+3])

			cur += d0
			sum += cur + magic
			cur += d1
			sum += cur + magic
			cur += d2
			sum += cur + magic
			cur += d3
			sum += cur + magic
		}
		// tail <4 elements
		for ; i < dlen; i++ {
			cur += int64(deltas[i])
			sum += cur + magic
		}
		total = uint64(sum)
	}
	return total
}