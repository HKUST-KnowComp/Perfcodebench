package main

func run(deltas []int32, iters int) uint64 {
	n := len(deltas)
	if n == 0 {
		return 0
	}

	// Precompute the sum of prefix values and the constant offset once
	// prefix[i] = sum(deltas[0..i])
	// total = sum(prefix[i] + 0x9E3779B1) for all i
	//       = sum(prefix[i]) + n * 0x9E3779B1
	//
	// sum(prefix[i]) can be computed in one pass:
	// As we accumulate current += delta[i], we add current to prefixSum

	var current int64
	var prefixSum int64 // sum of all prefix values (can be negative)

	for _, d := range deltas {
		current += int64(d)
		prefixSum += current
	}

	// The constant part: n * 0x9E3779B1
	constantPart := uint64(n) * 0x9E3779B1

	// Compute total for one iteration
	// prefixSum can be negative, so we cast carefully
	oneIterTotal := uint64(prefixSum) + constantPart

	// Since the computation is identical each iteration, multiply
	return oneIterTotal * uint64(iters)
}
