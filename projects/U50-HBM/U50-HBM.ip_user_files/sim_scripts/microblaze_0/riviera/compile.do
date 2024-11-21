vlib work
vlib riviera

vlib riviera/xpm
vlib riviera/microblaze_v11_0_9
vlib riviera/xil_defaultlib

vmap xpm riviera/xpm
vmap microblaze_v11_0_9 riviera/microblaze_v11_0_9
vmap xil_defaultlib riviera/xil_defaultlib

vlog -work xpm  -sv2k12 \
"/usr/pack/vitis-2022.1-zr/Vivado/2022.1/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \
"/usr/pack/vitis-2022.1-zr/Vivado/2022.1/data/ip/xpm/xpm_fifo/hdl/xpm_fifo.sv" \
"/usr/pack/vitis-2022.1-zr/Vivado/2022.1/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \

vcom -work xpm -93 \
"/usr/pack/vitis-2022.1-zr/Vivado/2022.1/data/ip/xpm/xpm_VCOMP.vhd" \

vcom -work microblaze_v11_0_9 -93 \
"../../../ipstatic/hdl/microblaze_v11_0_vh_rfs.vhd" \

vcom -work xil_defaultlib -93 \
"../../../../U50-HBM.gen/sources_1/ip/microblaze_0/sim/microblaze_0.vhd" \

vlog -work xil_defaultlib \
"glbl.v"

