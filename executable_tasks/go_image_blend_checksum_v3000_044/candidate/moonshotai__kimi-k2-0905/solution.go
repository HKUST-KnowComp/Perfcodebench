package main

import "unsafe"

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		sp := uintptr(unsafe.Pointer(&src[0]))
		dp := uintptr(unsafe.Pointer(&dst[0]))
		op := uintptr(unsafe.Pointer(&out[0]))

		for i := 0; i < pixels; i++ {
			base := i * 4

			a := *(*uint8)(unsafe.Pointer(sp + uintptr(base+3)))
			s0 := *(*uint8)(unsafe.Pointer(sp + uintptr(base)))
			s1 := *(*uint8)(unsafe.Pointer(sp + uintptr(base+1)))
			s2 := *(*uint8)(unsafe.Pointer(sp + uintptr(base+2)))

			d0 := *(*uint8)(unsafe.Pointer(dp + uintptr(base)))
			d1 := *(*uint8)(unsafe.Pointer(dp + uintptr(base+1)))
			d2 := *(*uint8)(unsafe.Pointer(dp + uintptr(base+2)))

			*(*uint8)(unsafe.Pointer(op + uintptr(base))) = uint8((uint32(s0)*uint32(a) + uint32(d0)*uint32(255-a) + 127) / 255)
			*(*uint8)(unsafe.Pointer(op + uintptr(base+1))) = uint8((uint32(s1)*uint32(a) + uint32(d1)*uint32(255-a) + 127) / 255)
			*(*uint8)(unsafe.Pointer(op + uintptr(base+2))) = uint8((uint32(s2)*uint32(a) + uint32(d2)*uint32(255-a) + 127) / 255)
			*(*uint8)(unsafe.Pointer(op + uintptr(base+3))) = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}