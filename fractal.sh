#!/bin/bash
set -x
./fractal -maxiter 5 -width 10 -height 5 -output fractal-10_5.bmp
./fractal -maxiter 5 -width 25 -height 21 -output fractal-25_21.bmp
./fractal -maxiter 5 -width 10 -height 5 -task -output fractal-task10_5.bmp
./fractal -maxiter 5 -width 25 -height 21 -task -output fractal-task25_21.bmp
./fractal -maxiter 5 -width 10 -height 5 -row -output fractal-row10_5.bmp
./fractal -maxiter 5 -width 25 -height 21 -row -output fractal-row25_21.bmp
./fractal -threads 10 -maxiter 5 -width 10 -height 5 -task -output fractal-task-threads10_5.bmp
./fractal -threads 10 -maxiter 5 -width 25 -height 21 -task -output fractal-task-threads25_21.bmp
./fractal -threads 10 -maxiter 5 -width 10 -height 5 -row -output fractal-row-threads10_5.bmp
./fractal -threads 10 -maxiter 5 -width 25 -height 21 -row -output fractal-row-threads25_21.bmp
diff fractal-10_5.bmp fractal-task-threads10_5.bmp
diff fractal-10_5.bmp fractal-row-threads10_5.bmp
diff fractal-25_21.bmp fractal-task-threads25_21.bmp
diff fractal-25_21.bmp fractal-row-threads25_21.bmp
diff fractal-10_5.bmp fractal-task10_5.bmp
diff fractal-10_5.bmp fractal-row10_5.bmp
diff fractal-25_21.bmp fractal-task25_21.bmp
diff fractal-25_21.bmp fractal-row25_21.bmp
