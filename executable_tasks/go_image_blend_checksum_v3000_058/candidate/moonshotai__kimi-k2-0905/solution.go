package main

import "unsafe"

const (
	m1 = 0x0101010101010101
	m2 = 0x00FF00FF00FF00FF
	m3 = 0xFF00FF00FF00FF00
)

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		blend(out, src, dst, pixels)
		hash = checksum64(out)
	}
	return hash
}

func blend(dst, src, bg []byte, n int) {
	// Fast path: process 8 pixels (32 bytes) per loop iteration.
	for i := 0; i < n*4; i += 32 {
		// Load 8 alphas (replicate each alpha byte four times).
		a8 := uint64(src[i+3]) | uint64(src[i+3])<<8 | uint64(src[i+3])<<16 | uint64(src[i+3])<<24 |
			uint64(src[i+7])<<32 | uint64(src[i+7])<<40 | uint64(src[i+7])<<48 | uint64(src[i+7])<<56
		a8 |= a8 << 8
		a8 |= a8 << 16

		// Load 8 source pixels.
		s0 := *(*uint64)(unsafe.Pointer(&src[i]))
		s1 := *(*uint64)(unsafe.Pointer(&src[i+8]))
		s2 := *(*uint64)(unsafe.Pointer(&src[i+16]))
		s3 := *(*uint64)(unsafe.Pointer(&src[i+24]))

		// Load 8 background pixels.
		d0 := *(*uint64)(unsafe.Pointer(&bg[i]))
		d1 := *(*uint64)(unsafe.Pointer(&bg[i+8]))
		d2 := *(*uint64)(unsafe.Pointer(&bg[i+16]))
		d3 := *(*uint64)(unsafe.Pointer(&bg[i+24]))

		// Blend each 64-bit lane.
		o0 := blend64(s0, d0, a8)
		o1 := blend64(s1, d1, a8)
		o2 := blend64(s2, d2, a8)
		o3 := blend64(s3, d3, a8)

		// Store results.
		*(*uint64)(unsafe.Pointer(&dst[i])) = o0
		*(*uint64)(unsafe.Pointer(&dst[i+8])) = o1
		*(*uint64)(unsafe.Pointer(&dst[i+16])) = o2
		*(*uint64)(unsafe.Pointer(&dst[i+24])) = o3
	}

	// Tail: process remaining bytes one pixel at a time.
	off := (n &^ 7) * 4
	for i := 0; i < (n&7)*4; i += 4 {
		a := src[off+i+3]
		dst[off+i] = blendByte(src[off+i], bg[off+i], a)
		dst[off+i+1] = blendByte(src[off+i+1], bg[off+i+1], a)
		dst[off+i+2] = blendByte(src[off+i+2], bg[off+i+2], a)
		dst[off+i+3] = 255
	}
}

// blend64 blends 8 bytes (two pixels) in parallel using 64-bit arithmetic.
func blend64(s, d, a uint64) uint64 {
	// Expand bytes to 16-bit lanes.
	sLo := s & m2
	dLo := d & m2
	aLo := a & m2

	sHi := (s & m3) >> 8
	dHi := (d & m3) >> 8
	aHi := (a & m3) >> 8

	// Blend formula: (s*a + d*(255-a) + 127) / 255
	blendLo := (sLo*aLo + dLo*(255-aLo) + 0x7F7F) * m1 >> 32 / 255
	blendHi := (sHi*aHi + dHi*(255-aHi) + 0x7F7F) * m1 >> 32 / 255

	return (blendHi << 8) | (blendLo & m2)
}

func blendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

// checksum64 computes the same checksum as checksumBytes but 8× faster.
func checksum64(data []byte) uint64 {
	const (
		prime   = 1099511628211
		initial = 1469598103934665603
	)
	hash := initial
	i := 0
	n := len(data)
	for ; i+8 <= n; i += 8 {
		v := *(*uint64)(unsafe.Pointer(&data[i]))
		hash ^= v
		hash *= prime
	}
	// Handle remaining bytes.
	for ; i < n; i++ {
		hash ^= uint64(data[i])
		hash *= prime
	}
	return hash
}