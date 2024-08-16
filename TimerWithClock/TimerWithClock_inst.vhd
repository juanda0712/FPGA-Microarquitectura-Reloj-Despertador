	component TimerWithClock is
		port (
			clk_clk                              : in  std_logic                     := 'X';             -- clk
			led_external_connection_export       : out std_logic_vector(9 downto 0);                     -- export
			sseg_hour_external_connection_export : out std_logic_vector(13 downto 0);                    -- export
			sseg_min_external_connection_export  : out std_logic_vector(13 downto 0);                    -- export
			sseg_sec_external_connection_export  : out std_logic_vector(13 downto 0);                    -- export
			switch_external_connection_export    : in  std_logic                     := 'X';             -- export
			buttons_external_connection_export   : in  std_logic_vector(3 downto 0)  := (others => 'X')  -- export
		);
	end component TimerWithClock;

	u0 : component TimerWithClock
		port map (
			clk_clk                              => CONNECTED_TO_clk_clk,                              --                           clk.clk
			led_external_connection_export       => CONNECTED_TO_led_external_connection_export,       --       led_external_connection.export
			sseg_hour_external_connection_export => CONNECTED_TO_sseg_hour_external_connection_export, -- sseg_hour_external_connection.export
			sseg_min_external_connection_export  => CONNECTED_TO_sseg_min_external_connection_export,  --  sseg_min_external_connection.export
			sseg_sec_external_connection_export  => CONNECTED_TO_sseg_sec_external_connection_export,  --  sseg_sec_external_connection.export
			switch_external_connection_export    => CONNECTED_TO_switch_external_connection_export,    --    switch_external_connection.export
			buttons_external_connection_export   => CONNECTED_TO_buttons_external_connection_export    --   buttons_external_connection.export
		);

