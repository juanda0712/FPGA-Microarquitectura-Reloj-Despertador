	component TimerWithClock is
		port (
			buttons_export         : in  std_logic_vector(3 downto 0) := (others => 'X'); -- export
			clk_clk                : in  std_logic                    := 'X';             -- clk
			leds_export            : out std_logic_vector(9 downto 0);                    -- export
			sseg_hour_tens_export  : out std_logic_vector(6 downto 0);                    -- export
			sseg_hour_units_export : out std_logic_vector(6 downto 0);                    -- export
			sseg_min_units_export  : out std_logic_vector(6 downto 0);                    -- export
			sseg_mins_tens_export  : out std_logic_vector(6 downto 0);                    -- export
			sseg_sec_tens_export   : out std_logic_vector(6 downto 0);                    -- export
			sseg_sec_units_export  : out std_logic_vector(6 downto 0);                    -- export
			switch_export          : in  std_logic                    := 'X';             -- export
			buzzer_export          : out std_logic                                        -- export
		);
	end component TimerWithClock;

	u0 : component TimerWithClock
		port map (
			buttons_export         => CONNECTED_TO_buttons_export,         --         buttons.export
			clk_clk                => CONNECTED_TO_clk_clk,                --             clk.clk
			leds_export            => CONNECTED_TO_leds_export,            --            leds.export
			sseg_hour_tens_export  => CONNECTED_TO_sseg_hour_tens_export,  --  sseg_hour_tens.export
			sseg_hour_units_export => CONNECTED_TO_sseg_hour_units_export, -- sseg_hour_units.export
			sseg_min_units_export  => CONNECTED_TO_sseg_min_units_export,  --  sseg_min_units.export
			sseg_mins_tens_export  => CONNECTED_TO_sseg_mins_tens_export,  --  sseg_mins_tens.export
			sseg_sec_tens_export   => CONNECTED_TO_sseg_sec_tens_export,   --   sseg_sec_tens.export
			sseg_sec_units_export  => CONNECTED_TO_sseg_sec_units_export,  --  sseg_sec_units.export
			switch_export          => CONNECTED_TO_switch_export,          --          switch.export
			buzzer_export          => CONNECTED_TO_buzzer_export           --          buzzer.export
		);

