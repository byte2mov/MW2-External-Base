#pragma once

class c_globals {
public:
	int width, height;
	bool render_menu = 0;


	bool box = true;
	bool skeletons = true;
	bool bounding_box = false;
	bool username = false;
	bool distance = true;
	bool snaplines = true;
	bool enemy_count = true;
	bool platform_steam;
	bool platform_battlenet;
	bool is_in_game;
	int player_count;
	bool aimbot = true;
	float smooth = 1.0f;
	float field_of_view = 150.0f;
	bool render_field_of_view = 1;

	int aimbot_key = VK_RBUTTON;

	bool triggerbot;
	int custom_delay = 50;
	int maximum_distance = 20;
	int render_distance = 150;
	bool has_clicked;
	std::chrono::steady_clock::time_point tb_begin;
	std::chrono::steady_clock::time_point tb_end;
	int tb_time_since;


}; static c_globals* globals = new c_globals();