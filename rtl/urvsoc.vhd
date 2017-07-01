-- 
-- uRV - a tiny and dumb RISC-V core
-- Copyright (c) 2015 CERN
-- Author: Tomasz Włostowski <tomasz.wlostowski@cern.ch>

-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 3 of the License, or (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-- 

--
-- rev1_top.vhd - top level for rev 1.1. PCB FPGA
--
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.gencores_pkg.all;
use work.wishbone_pkg.all;

library unisim;
use unisim.vcomponents.all;

entity urvsoc is
  generic (
    g_riscv_firmware : string  := "uart-bootloader.dat";
    g_riscv_mem_size : integer := 65536;
    g_simulation     : boolean := true;
    g_num_slaves     : integer := 4
    );
  port (
    resetn: in std_logic := '1';
    
    CLK100MHZ : in std_logic;  -- 125 MHz PLL reference

    uart_txd_o : out std_logic;
    uart_rxd_i : in  std_logic;

    --irq_i      : in std_logic_vector(7 downto 0);

    pad_cs_o   : out std_logic_vector(g_num_slaves-1 downto 0);
    pad_sclk_o : out std_logic;
    pad_mosi_o : out std_logic;
    pad_miso_i : in  std_logic;    

    leds_o: out std_logic_vector(3 downto 0);
    sw    : in  std_logic_vector(3 downto 0);
    ck_io : out std_logic_vector(41 downto 0) 
    );

end urvsoc;

architecture rtl of urvsoc is

  component urvsoc_reset_gen
    port (
      clk_sys_i        : in  std_logic;
      rst_pcie_n_a_i   : in  std_logic;
      rst_button_n_a_i : in  std_logic;
      rst_n_o          : out std_logic);
  end component;

  component urv_core is
    generic (
      g_internal_ram_size      : integer;
      g_internal_ram_init_file : string;
      g_simulation : boolean;
      g_address_bits           : integer--;
      --g_wishbone_start         : unsigned(31 downto 0)
      );
    port (
      clk_sys_i    : in  std_logic;
      rst_n_i      : in  std_logic;
      cpu_rst_i    : in  std_logic                    := '0';
      irq_i        : in  std_logic_vector(7 downto 0);
      dwb_o        : out t_wishbone_master_out;
      dwb_i        : in  t_wishbone_master_in;
      host_slave_i : in  t_wishbone_slave_in          := cc_dummy_slave_in;
      host_slave_o : out t_wishbone_slave_out);
  end component urv_core;

  component wb_spi is
    generic (
      g_interface_mode      : t_wishbone_interface_mode;
      g_address_granularity : t_wishbone_address_granularity;
      g_num_slaves          : integer);
    port (
      clk_sys_i : in std_logic;
      rst_n_i   : in std_logic;

      -- Wishbone
      slave_i : in  t_wishbone_slave_in;
      slave_o : out t_wishbone_slave_out;
      desc_o  : out t_wishbone_device_descriptor;

      pad_cs_o   : out std_logic_vector(g_num_slaves-1 downto 0);
      pad_sclk_o : out std_logic;
      pad_mosi_o : out std_logic;
      pad_miso_i : in  std_logic);
  end component wb_spi;

  component clk_pll is
    port
    (clk_out1  : out  std_logic;
     locked     : out  std_logic;
     dac_clk    : out  std_logic;
     ndac_clk : out std_logic;
     clk_in1   : in  std_logic);
  end component clk_pll;

  constant c_cnx_slave_ports  : integer := 1;
  constant c_cnx_master_ports : integer := 3;

  constant c_master_cpu : integer := 0;

  constant c_slave_gpio     : integer := 0;
  constant c_slave_uart      : integer := 1;
  constant c_slave_spi      : integer := 2;

  signal cnx_slave_in  : t_wishbone_slave_in_array(c_cnx_slave_ports-1 downto 0);
  signal cnx_slave_out : t_wishbone_slave_out_array(c_cnx_slave_ports-1 downto 0);

  signal cnx_master_in  : t_wishbone_master_in_array(c_cnx_master_ports-1 downto 0);
  signal cnx_master_out : t_wishbone_master_out_array(c_cnx_master_ports-1 downto 0);

  constant c_cfg_base_addr : t_wishbone_address_array(c_cnx_master_ports-1 downto 0) :=
    (c_slave_gpio => x"80001000",                  -- GPIO
     c_slave_uart => x"80000000",                  -- UART
     c_slave_spi  => x"80003000"
     );
 
  constant c_cfg_base_mask : t_wishbone_address_array(c_cnx_master_ports-1 downto 0) :=
    (c_slave_gpio => x"8000f000",
     c_slave_uart => x"8000f000",
     c_slave_spi  => x"8000f000"
   );

  signal clk_125m_pllref : std_logic;
  signal pllout_dac_clk: std_logic;
  signal reset_n_i : std_logic;
  signal pllout_clk_fb_pllref, pllout_clk_sys, clk_sys, sys_locked, sys_locked_n : std_logic;
  signal rst_n_sys, rst_sys : std_logic;

  signal dummy, gpio_out, gpio_in, gpio_oen : std_logic_vector(31 downto 0);
  signal dac_out : std_logic_vector(11 downto 0);
  signal dac_clk : std_logic;
begin  -- rtl


 U_Buf_CLK_PLL : IBUFG
    port map
    (O  => clk_125m_pllref,             -- Buffer output
     I  => CLK100MHZ);  -- buffer input (connect directly to top-level port)   

  cmp_sys_clk_pll : clk_pll
    port map
    (clk_out1  => pllout_clk_sys,
     dac_clk  => pllout_dac_clk,
     ndac_clk => ck_io(26),
     locked   => sys_locked,
     clk_in1    => clk_125m_pllref);

  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
  cmp_clk_sys_buf : BUFG
    port map
    (O => clk_sys,
     I => pllout_clk_sys);      

  cmp_dac_clk_buf : BUFG
    port map
    (O => dac_clk,
     I => pllout_dac_clk); 

  rst_sys <= not rst_n_sys;
  sys_locked_n <= not sys_locked;
  
  U_Reset_Gen : urvsoc_reset_gen
    port map (
      clk_sys_i        => clk_sys,
      rst_pcie_n_a_i   => sys_locked,
      rst_button_n_a_i => reset_n_i,
      rst_n_o          => rst_n_sys);

 U_CPU: urv_core
   generic map (
     g_internal_ram_size      => g_riscv_mem_size,
     g_internal_ram_init_file => g_riscv_firmware,
     g_simulation => g_simulation,
     g_address_bits           => 32--,
     --g_wishbone_start         => x"00020000"
     )
   port map (
     clk_sys_i    => clk_sys,
     rst_n_i      => rst_n_sys,
     cpu_rst_i    => '0',
     irq_i        => x"00",
     dwb_o        => cnx_slave_in(0),
     dwb_i        => cnx_slave_out(0));
 
  U_Intercon : xwb_crossbar
    generic map (
      g_num_masters => c_cnx_slave_ports,
      g_num_slaves  => c_cnx_master_ports,
      g_registered  => true,
      g_address     => c_cfg_base_addr,
      g_mask        => c_cfg_base_mask)
    port map (
      clk_sys_i => clk_sys,
      rst_n_i   => rst_n_sys,
      slave_i   => cnx_slave_in,
      slave_o   => cnx_slave_out,
      master_i  => cnx_master_in,
      master_o  => cnx_master_out);

  U_UART : xwb_simple_uart
    generic map (
      g_interface_mode      => PIPELINED,
      g_address_granularity => BYTE)
    port map (
      clk_sys_i  => clk_sys,
      rst_n_i    => rst_n_sys,
      slave_i    => cnx_master_out(c_slave_uart),
      slave_o    => cnx_master_in(c_slave_uart),
      uart_rxd_i => uart_rxd_i,
      uart_txd_o => uart_txd_o);

  U_SPI : wb_spi
    generic map (
      g_interface_mode      => PIPELINED,
      g_address_granularity => BYTE,
      g_num_slaves          => g_num_slaves)
    port map (
      clk_sys_i  => clk_sys,
      rst_n_i    => rst_n_sys,
      slave_i    => cnx_master_out(c_slave_spi),
      slave_o    => cnx_master_in(c_slave_spi),
      pad_cs_o   => pad_cs_o,
      pad_sclk_o => pad_sclk_o,
      pad_mosi_o => pad_mosi_o,
      pad_miso_i => pad_miso_i);
  

  U_GPIO : xwb_gpio_port
    generic map (
      g_interface_mode         => PIPELINED,
      g_address_granularity    => BYTE,
      g_num_pins               => 32,
      -- we don't want a 3-state output
      g_with_builtin_tristates => false)
    port map (
      clk_sys_i  => clk_sys,
      rst_n_i    => rst_n_sys,
      slave_i    => cnx_master_out(c_slave_gpio),
      slave_o    => cnx_master_in(c_slave_gpio),
      gpio_b     => dummy,
      gpio_out_o => gpio_out,
      gpio_in_i  => gpio_in,
      gpio_oen_o => gpio_oen);

  reset_n_i <= resetn;
  leds_o <= gpio_out(3 downto 0);
  --gpio_in(7 downto 4) <= sw;
  
  -- ck_io(26) <= dac_clk;
  ck_io(41 downto 30) <= dac_out;
end rtl;

