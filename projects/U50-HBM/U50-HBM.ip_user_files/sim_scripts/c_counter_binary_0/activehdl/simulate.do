onbreak {quit -force}
onerror {quit -force}

asim +access +r +m+c_counter_binary_0 -L xpm -L unisims_ver -L unimacro_ver -L secureip -O5 xil_defaultlib.c_counter_binary_0 xil_defaultlib.glbl

set NumericStdNoWarnings 1
set StdArithNoWarnings 1

do {wave.do}

view wave
view structure

do {c_counter_binary_0.udo}

run -all

endsim

quit -force
