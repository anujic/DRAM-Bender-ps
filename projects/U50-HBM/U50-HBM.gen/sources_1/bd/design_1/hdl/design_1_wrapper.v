//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2022.1 (lin64) Build 3526262 Mon Apr 18 15:47:01 MDT 2022
//Date        : Thu Nov 21 10:47:30 2024
//Host        : tardis-c14 running 64-bit Debian GNU/Linux 10 (buster)
//Command     : generate_target design_1_wrapper.bd
//Design      : design_1_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module design_1_wrapper
   (Clk,
    M0_AXIS_0_tdata,
    M0_AXIS_0_tlast,
    M0_AXIS_0_tready,
    M0_AXIS_0_tvalid,
    S0_AXIS_0_tdata,
    S0_AXIS_0_tlast,
    S0_AXIS_0_tready,
    S0_AXIS_0_tvalid,
    reset_rtl);
  input Clk;
  output [31:0]M0_AXIS_0_tdata;
  output M0_AXIS_0_tlast;
  input M0_AXIS_0_tready;
  output M0_AXIS_0_tvalid;
  input [31:0]S0_AXIS_0_tdata;
  input S0_AXIS_0_tlast;
  output S0_AXIS_0_tready;
  input S0_AXIS_0_tvalid;
  input reset_rtl;

  wire Clk;
  wire [31:0]M0_AXIS_0_tdata;
  wire M0_AXIS_0_tlast;
  wire M0_AXIS_0_tready;
  wire M0_AXIS_0_tvalid;
  wire [31:0]S0_AXIS_0_tdata;
  wire S0_AXIS_0_tlast;
  wire S0_AXIS_0_tready;
  wire S0_AXIS_0_tvalid;
  wire reset_rtl;

  design_1 design_1_i
       (.Clk(Clk),
        .M0_AXIS_0_tdata(M0_AXIS_0_tdata),
        .M0_AXIS_0_tlast(M0_AXIS_0_tlast),
        .M0_AXIS_0_tready(M0_AXIS_0_tready),
        .M0_AXIS_0_tvalid(M0_AXIS_0_tvalid),
        .S0_AXIS_0_tdata(S0_AXIS_0_tdata),
        .S0_AXIS_0_tlast(S0_AXIS_0_tlast),
        .S0_AXIS_0_tready(S0_AXIS_0_tready),
        .S0_AXIS_0_tvalid(S0_AXIS_0_tvalid),
        .reset_rtl(reset_rtl));
endmodule
