main: process.c mps_cv.c mps.c scheduler.c ready_queue.c scheduler_cv.c
	gcc mps_cv.c scheduler_cv.c process.c ready_queue.c -o mps_cv -I . -lm
	gcc mps.c scheduler.c process.c ready_queue.c -o mps -I . -lm