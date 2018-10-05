--Autor: Roman Nahálka, xnahal01
--Projekt INP č.1 - zobrazení iniciálů na displeji 8x8


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
  port  (
          RESET: in std_logic;
          SMCLK: in std_logic;
          ROW: out std_logic_vector(7 downto 0);
          LED: out std_logic_vector(7 downto 0);
        );
end ledc8x8;

architecture behv of ledc8x8 is

signal SMCLK_256: std_logic;
signal clock_enable: std_logic;
signal druhe_pismeno: std_logic;
signal count: std_logic_vector(21 downto 0);
signal radky: std_logic_vector(7 downto 0);
signal ledky: std_logic_vector(7 downto 0);

begin

-- citac
citac: process(RESET, SMCLK)
begin
  if (RESET = '1') then
    count <= "0000000000000000000000";
  elsif (SMCLK'event and SMCLK = '1') then
    count <= count + 1;
    if(count(7 downto 0) = "11111111" then
      clock_enable = '1';
    else
      clock_enable = '0';
    end if;
    if(count(21 downto 0) = "1111010000100100000000" then
      druhe_pismeno = '1'; 
    end if; 
  end if;
end process;

--rotace
rotace: process(RESET, SMCLK, clock_enable)
begin
  if(RESET = '1') then
    radky <= "1000000";
  elsif(SMCLK'event and SMCLK = '1' and clock_enable = '1') then
    radky <= radky(0) and radky(7 downto 1);
  end if;
end process;

--dekoder
dekoder: process(ROW, druhe_pismeno)
begin
  if(druhe_pismeno = '0') then
    case radky is
      when "1000000" => ledky <= "00011111";
      when "0100000" => ledky <= "01011111";
      when "0010000" => ledky <= "00011111";
      when "0001000" => ledky <= "01011111";
      when "0000100" => ledky <= "01101111";
      when "0000010" => ledky <= "11111111";
      when "0000001" => ledky <= "11111111";
      when "1000000" => ledky <= "11111111";
      when others => ledky <= "11111111";
    end case;
  elsif(druhe_pismeno = '1') then
    case radky is
      when "1000000" => ledky <= "00011111";
      when "0100000" => ledky <= "01011111";
      when "0010000" => ledky <= "00011111";
      when "0001000" => ledky <= "01111111";
      when "0000100" => ledky <= "01100110";
      when "0000010" => ledky <= "11110010";
      when "0000001" => ledky <= "11110100";
      when "1000000" => ledky <= "00001001";
      when others => ledky <= "11111111";
    end case;
  end if;
end process;

ROW <= radky;
LED <= ledky;

end architecture;