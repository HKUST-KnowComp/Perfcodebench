package main

func run(deltas []int32, iters int) uint64 {
	const c int64 = 0x9E3779B1
	var total uint64
	n := len(deltas)

	for iter := 0; iter < iters; iter++ {
		var current int64
		var t uint64

		i := 0
		for i+3 < n {
			current += int64(deltas[i])
			t += uint64(current + c)

			current += int64(deltas[i+1])
			t += uint64(current + c)

			current += int64(deltas[i+2])
			t += uint64(current + c)

			current += int64(deltas[i+3])
			t += uint64(current + c)
			i += 4
		}
		for i < n {
			current += int64(deltas[i])
			t += uint64(current + c)
			i++
		}

		total = t
	}
	return total
}
