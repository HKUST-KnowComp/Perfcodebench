package main

func run(deltas []int32, iters int) uint64 {
	var total uint64
	for iter := 0; iter < iters; iter++ {
		var current int64
		var sum uint64
		n := len(deltas)
		i := 0
		
		// Unrolled loop for better ILP
		for i+3 < n {
			current += int64(deltas[i])
			sum += uint64(current + 0x9E3779B1)
			
			current += int64(deltas[i+1])
			sum += uint64(current + 0x9E3779B1)
			
			current += int64(deltas[i+2])
			sum += uint64(current + 0x9E3779B1)
			
			current += int64(deltas[i+3])
			sum += uint64(current + 0x9E3779B1)
			
			i += 4
		}
		
		// Handle remaining elements
		for i < n {
			current += int64(deltas[i])
			sum += uint64(current + 0x9E3779B1)
			i++
		}
		
		total = sum
	}
	return total
}
