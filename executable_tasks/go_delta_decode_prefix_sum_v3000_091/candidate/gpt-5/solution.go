package main

func run(deltas []int32, iters int) uint64 {
	const addend = uint64(0x9E3779B1)
	var total uint64
	
	n := len(deltas)
	if n == 0 || iters <= 0 {
		return 0
	}
	
	d := deltas
	addendTotal := addend * uint64(n)
	
	for iter := 0; iter < iters; iter++ {
		var current int64
		subtotal := addendTotal
		
		i := 0
		n8 := n &^ 7 // largest multiple of 8 <= n
		for i < n8 {
			current += int64(d[i+0])
			subtotal += uint64(current)
			current += int64(d[i+1])
			subtotal += uint64(current)
			current += int64(d[i+2])
			subtotal += uint64(current)
			current += int64(d[i+3])
			subtotal += uint64(current)
			current += int64(d[i+4])
			subtotal += uint64(current)
			current += int64(d[i+5])
			subtotal += uint64(current)
			current += int64(d[i+6])
			subtotal += uint64(current)
			current += int64(d[i+7])
			subtotal += uint64(current)
			i += 8
		}
		for i < n {
			current += int64(d[i])
			subtotal += uint64(current)
			i++
		}
		
		total = subtotal
	}
	return total
}
