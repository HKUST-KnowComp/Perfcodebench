package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	d := deltas
	n := len(d)
	const addConst uint64 = 0x9E3779B1

	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64

		i := 0
		for ; i+7 < n; i += 8 {
			current += int64(d[i])
			sum += uint64(current) + addConst
			current += int64(d[i+1])
			sum += uint64(current) + addConst
			current += int64(d[i+2])
			sum += uint64(current) + addConst
			current += int64(d[i+3])
			sum += uint64(current) + addConst
			current += int64(d[i+4])
			sum += uint64(current) + addConst
			current += int64(d[i+5])
			sum += uint64(current) + addConst
			current += int64(d[i+6])
			sum += uint64(current) + addConst
			current += int64(d[i+7])
			sum += uint64(current) + addConst
		}
		for ; i < n; i++ {
			current += int64(d[i])
			sum += uint64(current) + addConst
		}
		total = sum
	}
	return total
}
