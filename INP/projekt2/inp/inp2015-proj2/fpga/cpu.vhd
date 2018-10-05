-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2015 Brno University of Technology,
--                    Faculty of Information Technology
-- Autor: Roman Nahálka, xnahal01
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 --signaly registru CNT
 signal CNT_inc : std_logic;
 signal CNT_dec : std_logic;
 signal CNT_jedna : std_logic;
 signal CNT_reg : std_logic_vector(7 downto 0);
 --signaly registru TMP
 signal TMP_reg : std_logic_vector(7 downto 0);
 --signaly registru PC
 signal PC_inc : std_logic;
 signal PC_dec : std_logic;
 signal PC_reg : std_logic_vector(12 downto 0);
 --signaly registru PTR
 signal PTR_inc : std_logic;
 signal PTR_dec : std_logic;
 signal PTR_reg : std_logic_vector(12 downto 0);
 --insturkcni typ procesoru
 type inst_type is(inc_ptr, dec_ptr, inc_data, dec_data, start_while, end_while,
                   print_data, get_data, save_data, save_tmp_data, halt, ostatni);
 signal ireg_dec : inst_type;
 --radic procesoru
 type fsm_state is(sidle, sfetch0, sfetch1, sdecode, sinc_ptr, sdec_ptr, sinc_data,
                   sdec_data, sget_data, ssave_data, ssave_tmp_data, shalt, 
                   sstart_while, send_while, sostatni, sprint_data, sinc_data1,
                   sdec_data1, sprint_data1, sget_data1, sstart_while1, sstart_while2,
                   sstart_while3, sstart_while4, send_while1, send_while2,
                   send_while3, send_while4, ssave_data1);
 signal pstate : fsm_state;
 signal nstate : fsm_state;
 --signal multiplexoru 1
 signal mx_1_sel : std_logic;
 --signal multiplexoru 2
 --signal mx_2_sel : std_logic_vector(1 downto 0); 

begin

--Regist CNT
cnt_counter: process(RESET, CLK, CNT_reg, CNT_inc, CNT_dec)
begin
  if(RESET = '1') then
    CNT_reg <= "00000000";
  elsif(CLK'event and CLK = '1') then
    if(CNT_inc = '1') then
      CNT_reg <= CNT_reg + 1;
    elsif(CNT_dec = '1') then
      CNT_reg <= CNT_reg - 1;
    elsif(CNT_jedna = '1') then
      CNT_reg <= "00000001";
    end if;
  end if;
end process;

--Registr PC
pc_coutner: process(RESET, CLK, PC_reg, PC_inc, PC_dec)
begin
  if(RESET = '1') then
    PC_reg <= "0000000000000";
  elsif(CLK'event and CLK = '1') then
    if(PC_inc = '1') then
      PC_reg <= PC_reg + 1;
    elsif(PC_dec = '1') then
      PC_reg <= PC_reg - 1;
    end if;
  end if;
end process;

--Registr PTR
ptr_counter: process(RESET, CLK, PTR_reg, PTR_inc, PTR_dec)
begin
  if(RESET = '1') then
    PTR_reg <= "1000000000000";
  elsif(CLK'event and CLK = '1') then
    if(PTR_inc = '1') then
      PTR_reg <= PTR_reg + 1;
    elsif(PTR_dec = '1') then
      PTR_reg <= PTR_reg - 1;
    end if;
  end if;
end process;

--multiplexor 1
mx1: process(mx_1_sel, PC_reg, PTR_reg)
begin
   case mx_1_sel is
    when '0' => DATA_ADDR <= PC_reg;
    when '1' => DATA_ADDR <= PTR_reg;
    when others => DATA_ADDR <= "0000000000000";
  end case;
end process;
  
--multiplexor 2
--mx2: process(mx_2_sel, IN_DATA, TMP_reg, DATA_RDATA)
--begin
--  case mx_2_sel is
--    when "00" => DATA_WDATA <= IN_DATA;
--    when "01" => DATA_WDATA <= TMP_reg;
--    when "10" => DATA_WDATA <= DATA_RDATA - 1;
--    when "11" => DATA_WDATA <= DATA_RDATA + 1;
--    when others => DATA_WDATA <= "00000000";
--  end case;
--end process;

--dekoder instrukci
instruction_decoder: process(DATA_RDATA)
begin
  case DATA_RDATA is
    when X"3E" => ireg_dec <= inc_ptr;
    when X"3C" => ireg_dec <= dec_ptr;
    when X"2B" => ireg_dec <= inc_data;
    when X"2D" => ireg_dec <= dec_data;
    when X"5B" => ireg_dec <= start_while;
    when X"5D" => ireg_dec <= end_while;
    when X"2E" => ireg_dec <= print_data;
    when X"2C" => ireg_dec <= get_data;
    when X"24" => ireg_dec <= save_data;
    when X"21" => ireg_dec <= save_tmp_data;
    when X"00" => ireg_dec <= halt;
    when others => ireg_dec <= ostatni;
  end case;
end process;

--FSM present state
fsm_pstate: process(RESET, CLK, nstate)
begin
  if(RESET = '1') then
    pstate <= sidle;
  elsif (CLK'event and CLK = '1') then
    if(EN = '1') then
      pstate <= nstate;
    end if;
  end if;
end process;

--FSM next state logic
nsl: process(pstate, ireg_dec, DATA_RDATA, OUT_BUSY, IN_VLD, IN_DATA, CNT_reg)
begin
  --INIT
  DATA_EN <= '0';
  DATA_RDWR <= '1';
  DATA_WDATA <= "00000000";
  PC_inc <= '0';
  PC_dec <= '0';
  CNT_inc <= '0';
  CNT_dec <= '0';
  CNT_jedna <= '0';
  PTR_inc <= '0';
  PTR_dec <= '0';
  OUT_WE <= '0';
  IN_REQ <= '0';
  
  case pstate is
    --IDLE
    when sidle => nstate <= sfetch0;
    -- INSTRUCTION FETCH
    when sfetch0 =>
      nstate <= sfetch1;
    
    when sfetch1 =>
      mx_1_sel <= '0';
      DATA_EN <= '1';
      nstate <= sdecode;
    
    -- INSTRUCTION DECODE
    when sdecode =>
      case ireg_dec is
        when halt =>
          nstate <=shalt;
        when ostatni =>
          nstate <=sostatni;
        when inc_ptr =>
          nstate <= sinc_ptr;
        when dec_ptr =>
          nstate <= sdec_ptr;
        when inc_data =>
          nstate <= sinc_data;
        when dec_data =>
          nstate <= sdec_data;
        when start_while =>
          nstate <= sstart_while;
        when end_while =>
          nstate <= send_while;
        when print_data =>
          nstate <= sprint_data;
        when get_data =>
          nstate <= sget_data;
        when save_data =>
          nstate <= ssave_data;
        when save_tmp_data =>
          nstate <= ssave_tmp_data;
      end case;
    --HALT
    when shalt =>
      nstate <= shalt;
    --OSTATNI
    when sostatni =>
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sfetch0;
    --ZVYSENI HODNOTY UKAZETELE
    when sinc_ptr =>
      PTR_inc <= '1';
      PTR_dec <= '0';
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sfetch0; 
    --SNIZENI HODNOTY UKAZETELE
    when sdec_ptr =>
      PTR_inc <= '0';
      PTR_dec <= '1';
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sfetch0;
    --ZVYSENI HODNOTY DAT
    when sinc_data => --1. faze, precetem data
      mx_1_sel <= '1';
      DATA_EN <= '1';
      DATA_RDWR <= '1';
      nstate <= sinc_data1;
    when sinc_data1 => --2. faze, inkrementujeme data
      mx_1_sel <= '1';
      DATA_EN <= '1';
      DATA_RDWR <= '0';
      DATA_WDATA <= DATA_RDATA + 1;
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sfetch0;
    --SNIZENI HODNOTY DAT
    when sdec_data => --1. faze, preceteme data
      mx_1_sel <= '1';
      DATA_EN <= '1';
      DATA_RDWR <= '1';
      nstate <= sdec_data1;
    when sdec_data1 => --2. faze, dekrementujeme data
      mx_1_sel <= '1';
      DATA_EN <= '1';
      DATA_RDWR <= '0';
      DATA_WDATA <= DATA_RDATA - 1;
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sfetch0;
    --TISKNUTI DAT
    when sprint_data => --1. faze, preceteme si data
      DATA_EN <= '1';
      mx_1_sel <= '1';
      DATA_RDWR <= '1';
      nstate <= sprint_data1;
    when sprint_data1 => -- 2.faze, vypiseme data
      if(OUT_BUSY = '0') then
        OUT_WE <= '1';
        OUT_DATA <= DATA_RDATA;
        PC_inc <= '1';
        PC_dec <= '0';
        nstate <= sfetch0;
      else nstate <= sprint_data1;
      end if;
    --NACTENI HODNOTY
    when sget_data => --1.faze, budeme pozadovat data
      IN_REQ <= '1';
      nstate <= sget_data1;
    when sget_data1 => --2.faze, ziskame data
      if(IN_VLD = '1') then --Dostali jsme platna data?
        DATA_EN <= '1';
        mx_1_sel <= '1';
        DATA_RDWR <= '0';
        DATA_WDATA <= IN_DATA;
        PC_inc <= '1';
        PC_dec <= '0';
        nstate <= sfetch0;
      else nstate <= sget_data1; --Ne? Tak cekame dal
      end if;
    --START WHILE   
    when sstart_while => --1. faze, preceteme si data
      PC_inc <= '1';
      PC_dec <= '0';
      DATA_EN <= '1';
      mx_1_sel <= '1';
      DATA_RDWR <= '1';
      nstate <= sstart_while1;
    when sstart_while1 => --2. faze, nastavime pocitadlo na jednicku
       if(DATA_RDATA = "00000000") then --if (ram[PTR] == 0)
        CNT_jedna <= '1';
        nstate <= sstart_while2;
       else
        nstate <= sfetch0;
       end if;
    when sstart_while2 => --3. faze 
      DATA_EN <= '1';
      mx_1_sel <= '0';
      nstate <= sstart_while3;
    when sstart_while3 => --4. faze, precteni instukce v cyklu
      if(ireg_dec = start_while) then --Zacatek cyklu, CNT + 1
        CNT_inc <= '1';
        CNT_dec <= '0';
      elsif(ireg_dec = end_while) then --Konec cyklu, CNT - 1
        CNT_inc <= '0';
        CNT_dec <= '1';
      end if;
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sstart_while4;
    when sstart_while4 =>  --5. faze
      if(CNT_reg = "00000000") then --uz byli vsechny cykli ukonceny?
        nstate <= sfetch0;
      else nstate <= sstart_while2; --jestli ne, tak pokracujeme
      end if;      
    --END WHILE
    when send_while => --1. faze, preceteme si data
      DATA_EN <= '1';
      mx_1_sel <= '1';
      DATA_RDWR <= '1';
      nstate <= send_while1;
    when send_while1 => --2. faze
      if(DATA_RDATA = "00000000") then -- if (ram[PTR] == 0)
        PC_inc <= '1';
        PC_dec <= '0';
        nstate <= sfetch0;
      else
        CNT_jedna <= '1';
        PC_inc <= '0';
        PC_dec <= '1';
        nstate <= send_while2;
      end if;
    when send_while2 => --3. faze
      DATA_EN <= '1';
      mx_1_sel <= '0';
      nstate <= send_while3;
    when send_while3 => -- 4.faze, precteme si instukci
      nstate <= send_while4;
      if(ireg_dec = end_while) then --Konec cyklu, CNT + 1
        CNT_inc <= '1';
        CNT_dec <= '0';
      elsif(ireg_dec = start_while) then --Zacatek cyklu, CNT - 1
        CNT_inc <= '0';
        CNT_dec <= '1';
      end if;
    when send_while4 => -- 5. faze
      if(CNT_reg = "00000000") then --Byli uz vsechny cykly ukonceny?                       
        PC_inc <= '1';
        PC_dec <= '0';
        nstate <= sfetch0;
      else  -- Jestli ne, tak pokracujeme
        PC_inc <= '0';
        PC_dec <= '1';
        nstate <= send_while2;
      end if;
    --ULOZENI DAT
    when ssave_data => --1.faze, precteme si data
      mx_1_sel <= '1';
      DATA_EN <= '1';
      DATA_RDWR <= '1';
      nstate <= ssave_data1;
    when ssave_data1 => --2.faze, ulozime data do pomocneho registru
      TMP_reg <= DATA_RDATA;
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sfetch0;
    --NACTENI ULOZENYCH DAT
    when ssave_tmp_data => --Nacteme data z pomocneho registru
      mx_1_sel <= '1';
      DATA_EN <= '1';
      DATA_RDWR <= '0';
      DATA_WDATA <= TMP_reg;
      PC_inc <= '1';
      PC_dec <= '0';
      nstate <= sfetch0;    
    when others =>
      nstate <= sostatni;
    end case;
  end process;
end behavioral;
 
