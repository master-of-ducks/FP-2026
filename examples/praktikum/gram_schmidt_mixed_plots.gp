set terminal pngcairo size 3000,1200 enhanced font ',16'
set output './gnuplot/mgs_plot_seed_u_v_730_169_2026-08-16_16-50.png'
set multiplot title 'Mixed precision Gram-Schmidt baseline FP8 to FP64'
set origin 0,0.94
set size 0.99,0.06
unset border
unset tics
unset title
unset xlabel
unset ylabel
unset logscale x
unset logscale y
set xrange [0:1]
set yrange [0:1]
set key horizontal center top box opaque
set key samplen 1.2
set key spacing 1.1
plot \
    2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', \
    2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', \
    2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', \
    2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', \
    2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', \
    2 with lines lw 2 lt 1 dt 2 title 'FP8 unit roundoff', \
    2 with lines lw 2 lt 2 dt 2 title 'bfloat16 unit roundoff', \
    2 with lines lw 2 lt 3 dt 2 title 'FP16 unit roundoff', \
    2 with lines lw 2 lt 4 dt 2 title 'FP32 unit roundoff', \
    2 with lines lw 2 lt 5 dt 2 title 'FP64 unit roundoff'
set origin 0,0.47
set size 0.25,0.47
set title 'CGS loss of orthogonality vs kappa'
set xlabel 'kappa'
set ylabel 'orthogonality error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/cgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/cgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/cgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/cgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/cgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/cgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
set origin 0.25,0.47
set size 0.25,0.47
set title 'MGS loss of orthogonality vs kappa'
set xlabel 'kappa'
set ylabel 'orthogonality error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/mgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/mgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/mgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/mgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/mgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/mgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
set origin 0.5,0.47
set size 0.25,0.47
set title 'CGS2 loss of orthogonality vs kappa'
set xlabel 'kappa'
set ylabel 'orthogonality error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/cgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/cgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/cgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/cgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/cgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/cgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
set origin 0.75,0.47
set size 0.25,0.47
set title 'MGS2 loss of orthogonality vs kappa'
set xlabel 'kappa'
set ylabel 'orthogonality error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/mgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/mgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/mgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/mgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/mgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/mgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_orthogonality_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
set origin 0,0
set size 0.25,0.47
set title 'CGS representation error vs kappa'
set xlabel 'kappa'
set ylabel 'relative representation error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/cgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/cgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/cgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/cgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/cgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/cgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
set origin 0.25,0
set size 0.25,0.47
set title 'MGS representation error vs kappa'
set xlabel 'kappa'
set ylabel 'relative representation error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/mgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/mgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/mgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/mgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/mgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/mgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
set origin 0.5,0
set size 0.25,0.47
set title 'CGS2 representation error vs kappa'
set xlabel 'kappa'
set ylabel 'relative representation error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/cgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/cgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/cgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/cgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/cgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/cgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/cgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/cgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
set origin 0.75,0
set size 0.25,0.47
set title 'MGS2 representation error vs kappa'
set xlabel 'kappa'
set ylabel 'relative representation error'
set logscale x
set logscale y
set grid
set border
set tics
unset key
unset xrange
unset yrange
plot \
    "gnuplot/data/mgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:2 with linespoints lw 2 lt 1 title 'FP8/FP8/FP8/FP8', "gnuplot/data/mgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP8_FP8_FP8_FP8.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:2 with linespoints lw 2 lt 2 title 'bfloat16/bfloat16/bfloat16/bfloat16', "gnuplot/data/mgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_representation_error_bfloat16_bfloat16_bfloat16_bfloat16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:2 with linespoints lw 2 lt 3 title 'FP16/FP16/FP16/FP16', "gnuplot/data/mgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP16_FP16_FP16_FP16.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:2 with linespoints lw 2 lt 4 title 'FP32/FP32/FP32/FP32', "gnuplot/data/mgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP32_FP32_FP32_FP32.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle, \
    "gnuplot/data/mgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:2 with linespoints lw 2 lt 5 title 'FP64/FP64/FP64/FP64', "gnuplot/data/mgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(6.25000000000000000e-02) with lines lw 2 lt 1 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(3.90625000000000000e-03) with lines lw 2 lt 2 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(4.88281250000000000e-04) with lines lw 2 lt 3 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(5.96046447753906250e-08) with lines lw 2 lt 4 dt 2 notitle, "gnuplot/data/mgs2_representation_error_FP64_FP64_FP64_FP64.dat" using 1:(1.11022302462515654e-16) with lines lw 2 lt 5 dt 2 notitle
unset logscale x
unset logscale y
unset multiplot
set output
