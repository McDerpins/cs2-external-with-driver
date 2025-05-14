#ifndef OVERLAY_H_
#define OVERLAY_H_

constexpr int menu_anim_time = 350;
constexpr int breath_anim_time = 1000;

// default hack settings
static bool l_crosshair = true;
static bool draw_lines = false;
static bool esp = true;
static bool show_team = false;
static bool draw_fov = true;
static float fov_radius = 30.0f;
static bool aimbot = true;
static bool draw_boxes = false;
static bool draw_skeleton = true;
static float smoothingFactor = 0.4f;

class c_overlay {
private:
	HANDLE								driver;
	std::uintptr_t client;
	HWND								window_handle;
	WNDCLASSEX							window_class;
	ID3D11Device*						d3d_device;
	ID3D11DeviceContext*				device_context;
	IDXGISwapChain*						swap_chain;
	ID3D11RenderTargetView*				render_target_view;
	ImDrawList*							draw_list;
	c_timer								breath_timer, menu_timer;

	struct s_font {
		ImFont* im_font;
		FLOAT font_size;
	};
private:
	template <typename T>
	inline VOID safe_release(T*& p);

	BOOL init_device();
	VOID dest_device();

	VOID init_imgui();
	VOID dest_imgui();

	VOID init_render_target();
	VOID dest_render_target();
	const VOID render(FLOAT width, FLOAT height);

	void input_handler();
	void anim_handler();
public:
	c_overlay(HANDLE driver, uintptr_t client);
	~c_overlay();

	BOOL msg_loop();

	typedef struct
	{
		ImU32 R;
		ImU32 G;
		ImU32 B;
	} RGB;

	ImU32 Color(RGB color)
	{
		return IM_COL32(color.R, color.G, color.B, 255);
	}
	//Entity* get_closest_enemy(const std::vector<Entity>& entities, const view_matrix_t& view_matrix, int localTeam, float fov);
	void draw_circle(float x, float y, float radius, RGB color);
	VOID DrawLine(int x1, int y1, int x2, int y2, RGB color, int thickness);
	VOID DrawRect(int x, int y, int w, int h, RGB color, int thickness);
	VOID init_draw_list();
	VOID crosshair(const FLOAT aSize, ImU32 color);
	VOID box(const ImVec2& pos, const FLOAT width, const FLOAT height, ImU32 color, const FLOAT line_width = 2.f);
	VOID line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width = 2.f);
	VOID circle(const ImVec2& point, const FLOAT radius, ImU32 color);
	VOID text(const ImVec2& pos, const std::string& text, ImU32 color = ImColor(240, 248, 255), bool outline = true);
	VOID radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out);

	BOOL in_screen(const ImVec2& pos);
public:
	FLOAT window_width, window_height;
	bool exit, ready, show_menu = true;
	INT breath, menu_ticks;
	s_font* font;
};

#endif