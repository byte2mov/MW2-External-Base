#pragma once
#include "../render/Vectors.h"
#include <array>


namespace drawing
{
	void DrawFilledRect(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImColor{ 255, 255, 255, 255 }, 0, 0);
	}

	class draw
	{


	public:
		static auto DrawNormalBox(int x, int y, int w, int h, int borderPx, ImColor color) -> void
		{
			DrawFilledRect(x + borderPx, y, w, borderPx, color); //top 
			DrawFilledRect(x + w - w + borderPx, y, w, borderPx, color); //top 
			DrawFilledRect(x, y, borderPx, h, color); //left 
			DrawFilledRect(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
			DrawFilledRect(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
			DrawFilledRect(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
			DrawFilledRect(x + w + borderPx, y, borderPx, h, color);//right 
			DrawFilledRect(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
		}
		void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
		{
			auto window = ImGui::GetBackgroundDrawList();;
			/*float a = (color >> 24) & 0xff;
			float r = (color >> 16) & 0xff;
			float g = (color >> 8) & 0xff;
			float b = (color) & 0xff;*/
			window->AddLine(from, to, color, thickness);
		}
		void draw_box(const float x, const float y, const float width, const float height, const uint32_t color, float thickness)
		{
			draw_line(ImVec2(x, y), ImVec2(x + width, y), color, thickness);
			draw_line(ImVec2(x, y), ImVec2(x, y + height), color, thickness);
			draw_line(ImVec2(x, y + height), ImVec2(x + width, y + height), color, thickness);
			draw_line(ImVec2(x + width, y), ImVec2(x + width, y + height), color, thickness);
		}

	};
}static drawing::draw* draw = new drawing::draw();

class entity {
public:
	uintptr_t
		actor,
		skeletal_mesh,
		root_component,
		player_state;
	int
		team_index;
};

std::vector<entity> entity_list;
std::vector<entity> temporary_entity_list;

enum GAME_INFORMATION : int {
	GAME_THREAD_FAILED = 0,
	GAME_SETUP_SUCCESSFUL = 1
};
struct bounds_t
{
	float left, right, top, bottom;
};

ImColor visible_color = ImColor(255, 0, 0);
bool in_lobby = false;

namespace game {
	class c_game {
	public:

		auto Setup() -> GAME_INFORMATION {

			//std::thread(game::c_game::CacheData).detach();
			std::cout << "Process Enivroment Block :";
			std::cout << decrypt->peb;
			return GAME_INFORMATION::GAME_SETUP_SUCCESSFUL;
		}

		static auto CacheData() -> void {

			for (;;) 
			{
			   // incase you want to use caching
			}

		}
		auto ActorLoop() -> void
		{
			globals->is_in_game = Utilities->is_user_in_game();
			globals->player_count = Utilities->player_count();
			if (globals->is_in_game)
			{
				pointer->client_info = decrypt->Client_Information(request->m_image_base, decrypt->peb);
				pointer->client_info_base = decrypt->Client_Base(request->m_image_base, decrypt->peb, pointer->client_info);
				auto ref_def_pointer = decrypt_refdef->retrieve_ref_def();
				decrypt_refdef->ref_def_nn = request->read<ref_def_t>(ref_def_pointer);
				player Local_Player(pointer->client_info_base + (Utilities->local_player_index() * offset::player_size));
				auto Local_Player_Position = Local_Player.get_position();
				auto Local_Player_Team = Local_Player.team_id();
				auto Entity_Bone_Base = decrypt->bone_base(request->m_image_base, decrypt->peb);
				auto Entity_Bone_Position = Utilities->retrieve_bone_position_vec(pointer->client_info);

				for (int i = 0; i < globals->player_count; i++)
				{


					player entity(pointer->client_info_base + (i * offset::player_size));


					fvector2d player_screen;
					fvector2d entity_head;
					fvector position = entity.get_position();
					auto Bone_Pointer_Index = decrypt->bone_index(i, request->m_image_base);
					auto bone_pointer = entity.bone_pointer(Entity_Bone_Base, Bone_Pointer_Index);

					auto corner_height = abs(entity_head.y - player_screen.y);
					auto corner_width = corner_height * 0.65;

					fvector2d screen_middle = { (float)decrypt_refdef->ref_def_nn.width / 2, (float)decrypt_refdef->ref_def_nn.height / 2 };

					auto screen_x = decrypt_refdef->ref_def_nn.width;
					auto screen_y = decrypt_refdef->ref_def_nn.height;

					if (!entity.is_player_valid())
						continue;
					if (!entity.get_name_entry(i).health < 0)
						continue;
					if (!bone_pointer)
						continue;


					if (Utilities->w2s(position, player_screen))
					{
						NameEntry player_name_entry = entity.get_name_entry(i);

						if (player_name_entry.name == NULL)
							continue;

						auto entity_distance = Utilities->units_to_m(Local_Player_Position.distance_to(position));

						if (entity_distance < globals->maximum_distance)
						{
							if (globals->distance){
								ImGui::GetBackgroundDrawList()->AddText(ImVec2(player_screen.x - (ImGui::CalcTextSize(Utilities->ConvertDistanceToString((int)entity_distance).c_str()).x / 2 + 1), player_screen.y), ImColor(255, 255, 255, 255), Utilities->ConvertDistanceToString((int)entity_distance).c_str());
							}
							if (globals->box) {
								draw->draw_box(entity_head.x - (corner_width / 2), entity_head.y, corner_width, corner_height, ImColor(255, 255, 255, 255),  1.0f);
							}

							if (globals->aimbot) {

								fvector2d select_bone;
								if (entity.team_id() != Local_Player_Team)
									continue;
								if (Utilities->w2s(Utilities->retrieve_bone_position(bone_pointer, Entity_Bone_Position, 7), select_bone)) {
									if (globals->aimbot_key) {
										
										if (select_bone.x > 1 && select_bone.y > 1)
										{
											Utilities->cursor_to(select_bone.x, select_bone.y);
										}
									}
								}
								
							}
						

						}
					}

					
				}
			}

		}
		
	};
} static game::c_game* Game = new game::c_game();