package main

func run(deltas []int32, iters int) uint64 {
	const key = 0x9E3779B1
	total := uint64(0)

	delta := deltas
	for iter := 0; iter < iters; iter++ {
		var cur int64
		var acc uint64

		// unroll 4x
		for len(delta) >= 4 {
			d0 := int64(delta[0])
			d1 := int64(delta[1])
			d2 := int64(delta[2])
			d3 := int64(delta[3])

			cur += d0
			acc += uint64(cur + key)
			cur += d1
			acc += uint64(cur + key)
			cur += d2
			acc += uint64(cur + key)
			cur += d3
			acc += uint64(cur + key)

			delta = delta[4:]
		}

		// handle tail
		for _, d := range delta {
			cur += int64(d)
			acc += uint64(cur + key)
		}

		total = acc
		delta = deltas // reset slice for next iteration
	}

	return total
}