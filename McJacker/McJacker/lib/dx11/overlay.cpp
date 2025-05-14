#include "../pch.h"
#include "../offsets/offsets.h"
#include "../namespace/namespace.h"
#include "../util/vector.h"
#include "entity.h"
#include "draw.h"
#include "../bones/bones.hpp"

// microsoft safe release of pointers
template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
	if (NULL != p) {
		p->Release();
		p = NULL;
	}
}

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

BOOL c_overlay::init_device() {
	DXGI_SWAP_CHAIN_DESC vSwapChainDesc;
	ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
	vSwapChainDesc.BufferCount = 2;
	vSwapChainDesc.BufferDesc.Width = 0;
	vSwapChainDesc.BufferDesc.Height = 0;
	vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	vSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	vSwapChainDesc.OutputWindow = window_handle;
	vSwapChainDesc.SampleDesc.Count = 1;
	vSwapChainDesc.SampleDesc.Quality = 0;
	vSwapChainDesc.Windowed = TRUE;
	vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL vFeatureLevel;
	D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, vFeatureLevelArray, 2, D3D11_SDK_VERSION, &vSwapChainDesc, &swap_chain, &d3d_device, &vFeatureLevel, &device_context) != S_OK)
		return false;

	init_render_target();
	
	ShowWindow(window_handle, SW_SHOWNORMAL);
	UpdateWindow(window_handle);
	return true;
}

VOID c_overlay::dest_device() {
	dest_render_target();
	safe_release(swap_chain);
	safe_release(device_context);
	safe_release(d3d_device);
}

VOID c_overlay::init_imgui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsClassic();
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowTitleAlign.x = 0.50f;
	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 4.0f;
	style.FramePadding = ImVec2(2, 2);
	style.FrameRounding = 2.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 3.0f;

	style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.043f, 0.043f, 0.043f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.043f, 0.043f, 0.043f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.043f, 0.043f, 0.043f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.012f, 0.91f, 0.957f, 1.f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.043f, 0.043f, 0.043f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.043f, 0.043f, 0.043f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.012f, 0.91f, 0.957f, 1.f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.043f, 0.043f, 0.043f, 0.73f);

	ImGuiIO& io = ImGui::GetIO();
	font->im_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 24.0f);
	font->font_size = 24.f;

	ImGui_ImplWin32_Init(window_handle);
	ImGui_ImplDX11_Init(d3d_device, device_context);
}

VOID c_overlay::dest_imgui() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

VOID c_overlay::init_render_target() {
	ID3D11Texture2D* back_buffer = nullptr;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (back_buffer)
		d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
	// todo: error here if it can't get the backbuffer of the render target
	back_buffer->Release();
}

VOID c_overlay::dest_render_target() {
	if (!render_target_view)
		return;

	render_target_view->Release();
	render_target_view = NULL;
}


///////////////////////////////////////////////
//
//
//
//		You can ignore everything above here
//		Below is where the hijacking happens	
//
///////////////////////////////////////////////

c_overlay::c_overlay(HANDLE driver_handle, std::uintptr_t client) :
	ready(false),
	exit(false),
	window_handle(nullptr),
	client(client),
	driver(driver_handle),
	window_class({}),
	window_width((FLOAT)GetSystemMetrics(SM_CXSCREEN)),
	window_height((FLOAT)GetSystemMetrics(SM_CYSCREEN)),
	font(nullptr),
	d3d_device(nullptr),
	device_context(nullptr),
	swap_chain(nullptr),
	render_target_view(nullptr),
	draw_list(nullptr),
	breath(NULL),
	menu_ticks(NULL)
{
	// get a handle to the nvidia window
	window_handle = FindWindowA("CEF-OSC-WIDGET", "NVIDIA GeForce Overlay"); //Hijack nvidia share, bc for some reason this is fucking possible?
	auto getInfo = GetWindowLongA(window_handle, -20);
	auto changeAttributes = SetWindowLongA(window_handle, -20, (LONG_PTR)(getInfo | 0x20));
	
	// avoiding window flags: https://www.unknowncheats.me/forum/general-programming-and-reversing/229070-overlay-window-topmost-top-game-windows-video.html
	MARGINS margins = { -1 };
	margins.cyBottomHeight = margins.cyTopHeight = margins.cxLeftWidth = margins.cxRightWidth = -1;
	DwmExtendFrameIntoClientArea(window_handle, &margins);
	SetLayeredWindowAttributes(window_handle, 0x000000, 0xFF, 0x02);
	SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, 0x0002 | 0x0001);

	if (!window_handle) {

#ifdef _DEBUG
		MessageBoxW(NULL, L"Failed to get a window handle", L"Error", MB_OK | MB_ICONERROR);
#endif 

		return; // error grabbing nvidia overlay
	}

	font = new s_font();

	if (!init_device())
		return; // error creating device
	
	init_imgui();

	std::thread(&c_overlay::input_handler, this).detach();
	std::thread(&c_overlay::anim_handler, this).detach();
}



c_overlay::~c_overlay() {
	dest_imgui();
	dest_device();

	delete font;
}

//static float smoothingFactor = 0.4f; // aimbot smoothing.


//////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
//
//
//
//		
//		The render loop is where the magic happens
//
///////////////////////////////////////////////

const void c_overlay::render(FLOAT width, FLOAT height) {
	if (exit) {
		ready = true;
		return;
	}

	ImGuiStyle& style = ImGui::GetStyle();

	init_draw_list(); // we only do this once a loop

	// a double value between 0 && 1 based on the anim time
	double menu_anim = sin((M_PI / 2.0) * (((menu_ticks == 1 ? 0 : 1) * pow(menu_ticks, 2)) / ((double)pow(menu_anim_time - 1, 2))));
	circle({ window_width / 2, window_height / 2 }, menu_anim * (window_width * 0.573674 /*Magical constant to calc radius of a 16:9 window found using some basic trig*/), ImColor(11, 11, 11, 80));
	style.Alpha = menu_anim;
	style.Colors[ImGuiCol_Border] = ImVec4(0.012f, 0.91f, 0.957f, ((breath / (float)breath_anim_time) * .7f) + .3f);


	// commented out by default, not sure what this does. 
	//auto io = &ImGui::GetIO();
	//overlay.text({io->MousePos.x, io->MousePos.y + 10}, "CURSOR IS RIGHT HERE");

	//static bool l_crosshair = true;
	//static bool draw_lines = false;
	//static bool esp = true;
	//static bool show_team = false;
	//static bool draw_fov = true;
	//static float fov_radius = 30.0f;
	//static bool aimbot = true;
	//static bool draw_boxes = false;
	//static bool draw_skeleton = true;
	//static float smoothingFactor = 2.0f;


	// this is inefficient.
	uintptr_t localPlayer = driver::read_memory<uintptr_t>(driver, client + offsets::dwLocalPlayerPawn);
	Vector3 Localorgin = driver::read_memory<Vector3>(driver, localPlayer + offsets::m_vOldOrigin);
	view_matrix_t view_matrix = driver::read_memory<view_matrix_t>(driver, client + offsets::dwViewMatrix);
	uintptr_t entity_list = driver::read_memory<uintptr_t>(driver, client + offsets::dwEntityList);
	uintptr_t localPlayerController = driver::read_memory<uintptr_t>(driver, client + offsets::dwLocalPlayerController);
	int localTeam = driver::read_memory<int>(driver, localPlayerController + offsets::m_iTeamNum);

	// RGB 
	static int r = 255;
	static int g = 0;
	static int b = 255;

	// RGB team
	static int t_r = 0;
	static int t_g = 160;
	static int t_b = 255;

	RGB enemy = { r, g, b };
	RGB team = { t_r, t_g, t_b };

	// aimbot variables
	ImVec2 screenCenter(screenWidth / 2.0f, screenHeight / 2.0f);
	Entity* closestEnemy = nullptr;
	float closestDistance = FLT_MAX;


	// Hack loop
	for (int playerIndex = 0; playerIndex < 64; ++playerIndex) {
		uintptr_t listentry = driver::read_memory<uintptr_t>(driver, entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);

		if (!listentry)
			continue;

		uintptr_t player = driver::read_memory<uintptr_t>(driver, listentry + 120 * (playerIndex & 0x1FF));

		if (!player)
			continue;

		int playerTeam = driver::read_memory<int>(driver, player + offsets::m_iTeamNum);
		//bool isDormant = driver::read_memory<int>(driver, player+ offsets::)
	
		uint32_t playerPawn = driver::read_memory<uint32_t>(driver, player + offsets::m_hPlayerPawn);

		uintptr_t listentry2 = driver::read_memory<uintptr_t>(driver, entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);

		if (!listentry2)
			continue;

		uintptr_t pCSPlayerPawn = driver::read_memory<uintptr_t>(driver, listentry2 + 120 * (playerPawn & 0x1FF));

		if (!pCSPlayerPawn)
			continue;

		
		uintptr_t gameSceneNode = driver::read_memory<uintptr_t>(driver, pCSPlayerPawn + offsets::m_pGameSceneNode);
		uintptr_t bonearray = driver::read_memory<uintptr_t>(driver, gameSceneNode + offsets::m_modelState + 0x80);


		int health = driver::read_memory<int>(driver, pCSPlayerPawn + offsets::m_iHealth);

		if (health <= 0 || health > 100)
			continue;

		if (pCSPlayerPawn == localPlayer)
			continue;

		Vector3 origin = driver::read_memory<Vector3>(driver, pCSPlayerPawn + offsets::m_vOldOrigin);
		Vector3 head = driver::read_memory<Vector3>(driver, bonearray + bones::head * 32);

		Vector3 screenPos = origin.WTS(view_matrix);
		Vector3 screenHead = head.WTS(view_matrix);
		float headHeight = (screenPos.y - screenHead.y) / 8;

		float height = screenPos.y - screenHead.y;
		float width = height / 2.4f;

		int rectBottomX = screenHead.x;
		int rectBottomY = screenHead.y + height;

		int bottomCenterX = screenWidth / 2;
		int bottomCenterY = screenHeight;

		if (!show_team) {
			if (playerTeam == localTeam)
				continue;
		}

		if (screenHead.x - width / 2 >= 0 &&
			screenHead.x + width / 2 <= screenWidth &&
			screenHead.y >= 0 &&
			screenHead.y + height <= screenHeight &&
			screenHead.z > 0 &&
			esp == true) {

		
			float distance = sqrtf(powf(screenHead.x - screenCenter.x, 2) + powf(screenHead.y - screenCenter.y, 2));

			if (playerTeam != localTeam && distance < closestDistance) {
				closestDistance = distance;
				closestEnemy = reinterpret_cast<Entity*>(pCSPlayerPawn); // Keep a reference to the closest enemy
			}

			if (playerTeam == localTeam) {
				// teammates with team color
				if (draw_lines) {
					DrawLine(bottomCenterX, bottomCenterY, rectBottomX, rectBottomY, team, 1.5f);
				}
				// ESP boxes for friendlies
				if (draw_boxes) {
					DrawRect(
						screenHead.x - width / 2,
						screenHead.y,
						width,
						height,
						team,
						1.5
					);
				}
				// Bone esp for friendlies
				if (draw_skeleton) {
					for (int i = 0; i < sizeof(boneConnections) / sizeof(boneConnections[0]); i++) {
						int bone1 = boneConnections[i].bone1;
						int bone2 = boneConnections[i].bone2;
						Vector3 vectorBone1 = driver::read_memory<Vector3>(driver, bonearray + bone1 * 32);
						Vector3 vectorBone2 = driver::read_memory<Vector3>(driver, bonearray + bone2 * 32);
						//Vector3 bonePos1 = driver::read_memory<Vector3>(driver, bonearray + (boneID * 32));
						Vector3 screenBone1 = vectorBone1.WTS(view_matrix);
						Vector3 screenBone2 = vectorBone2.WTS(view_matrix);
						if (screenBone1.z > 0 && screenBone2.z > 0) {
							DrawLine(screenBone1.x, screenBone1.y, screenBone2.x, screenBone2.y, { 255,255,255 }, 1.5f);
						}
					}
				}
			}
			else {
				
				// enemies with enemy color
				if (draw_lines) {
					DrawLine(bottomCenterX, bottomCenterY, rectBottomX, rectBottomY, enemy, 1.5f);
				}
				if (draw_boxes) {
					DrawRect(
						screenHead.x - width / 2,
						screenHead.y,
						width,
						height,
						enemy,
						1.5
					);
				}
				if (draw_skeleton) {
					//draw circle on head

					
					for (int i = 0; i < sizeof(boneConnections) / sizeof(boneConnections[0]); i++) {
						int bone1 = boneConnections[i].bone1;
						int bone2 = boneConnections[i].bone2;
						Vector3 vectorBone1 = driver::read_memory<Vector3>(driver, bonearray + bone1 * 32);
						Vector3 vectorBone2 = driver::read_memory<Vector3>(driver, bonearray + bone2 * 32);
						//Vector3 bonePos1 = driver::read_memory<Vector3>(driver, bonearray + (boneID * 32));
						Vector3 screenBone1 = vectorBone1.WTS(view_matrix);
						Vector3 screenBone2 = vectorBone2.WTS(view_matrix);
						if (screenBone1.z > 0 && screenBone2.z > 0) {
							DrawLine(screenBone1.x, screenBone1.y, screenBone2.x, screenBone2.y, { 255,255,255 }, 1.5f);
						}
					}
					draw_circle(screenHead.x, screenHead.y, headHeight - 3, { 255,255,255 });
					
				}
			}

		

		}
	}// end of entity loop


	if (l_crosshair)
		crosshair(10, ImColor(3, 232, 244));

	if (draw_fov) {
		ImU32 color = ImColor(255, 255, 0); // Yellow color for the FOV circle
		draw_list->AddCircle(ImVec2(screenWidth / 2, screenHeight / 2), fov_radius, color, 100, 1.5f);
	}

	if (aimbot) { // Check if aimbot is enabled 
		if (closestEnemy) {

			// Read the enemy's head position
			Vector3 closestOrigin = driver::read_memory<Vector3>(driver, reinterpret_cast<uintptr_t>(closestEnemy) + offsets::m_vOldOrigin);

			uintptr_t gameSceneNode = driver::read_memory<uintptr_t>(driver, reinterpret_cast<uintptr_t>(closestEnemy) + offsets::m_pGameSceneNode);
			uintptr_t bonearray = driver::read_memory<uintptr_t>(driver, gameSceneNode + offsets::m_modelState + 0x80);

			Vector3 closestHead = driver::read_memory<Vector3>(driver, bonearray + bones::head * 32);

			// Convert enemy head position to screen space
			Vector3 screenHead = closestHead.WTS(view_matrix);

			// This line binds the aimbot to the right mouse button, you may want to change it. 
			if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {

				if (screenHead.z > 0) { // Ensure the enemy is on the screen
					// Calculate the relative position to move the mouse
					float deltaX = screenHead.x - (screenWidth / 2.0f); // Horizontal distance to center
					float deltaY = screenHead.y - (screenHeight / 2.0f); // Vertical distance to center


					//CHECK HERE IF ENEMY IS WITHIN FOV
					float distanceToCenter = sqrtf(deltaX * deltaX + deltaY * deltaY); // Distance from screen center
					if (distanceToCenter > fov_radius) {
						return; // Exit if the enemy is outside the FOV circle
					}


					// Apply smoothing factor (optional)
					//static float smoothingFactor = 0.1f;
					deltaX *= smoothingFactor;
					deltaY *= smoothingFactor;

					// Simulate mouse movement
					INPUT input = { 0 };
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_MOVE;
					input.mi.dx = static_cast<LONG>(deltaX); // Relative movement
					input.mi.dy = static_cast<LONG>(deltaY); // Relative movement
					
					// This is the worst possible implementation of aimbot, but it works. 
					SendInput(1, &input, sizeof(INPUT));
				}
			}

			
		}
	}
	
	if (menu_ticks == 1)
		return;

	// imgui menu itself
	if (show_menu)
	{

		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin("Derp Derp Derpin' 420", nullptr, ImGuiWindowFlags_NoCollapse);

		ImGui::Text("https://github.com/Extravi/cs2-kernel-esp");

	
		static ImVec4 color = ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

		r = static_cast<int>(color.x * 255.0f);
		g = static_cast<int>(color.y * 255.0f);
		b = static_cast<int>(color.z * 255.0f);

		static ImVec4 t_color = ImVec4(t_r / 255.0f, t_g / 255.0f, t_b / 255.0f, 1.0f);

		t_r = static_cast<int>(t_color.x * 255.0f);
		t_g = static_cast<int>(t_color.y * 255.0f);
		t_b = static_cast<int>(t_color.z * 255.0f);

		if (ImGui::CollapsingHeader("Colors")) {
			ImGui::ColorEdit3("Enemy Color", (float*)&color);
			ImGui::ColorEdit3("Team Color", (float*)&t_color);
		}

		if (ImGui::CollapsingHeader("ESP")) {
			ImGui::Checkbox("Enable ESP", &esp);
			ImGui::Checkbox("Boxes", &draw_boxes);
			ImGui::Checkbox("Snap Lines", &draw_lines);
			ImGui::Checkbox("Skeleton", &draw_skeleton);
			ImGui::Checkbox("Show Team", &show_team);
		}

		if (ImGui::CollapsingHeader("Aim")) {
			ImGui::Checkbox("Enable", &aimbot);
			ImGui::Checkbox("DrawFOV", &draw_fov);
			ImGui::SliderFloat("FOV Radius", &fov_radius, 10.0f, 800.0f, "%.1f");
			ImGui::SliderFloat("Smoothing", &smoothingFactor, 0.1f, 2.0f, "%.2f");
			ImGui::Checkbox("Crosshair", &l_crosshair);
		}

		//ImGui::Checkbox("FPS Counter", &l_show_fps);
		ImGui::End();
	}
}

///////////////////////////////////////////////
//
//
//
//		You can ignore everything below here
//			
//
///////////////////////////////////////////////

BOOL c_overlay::msg_loop() {
	if (window_handle) {
		MSG msg{ 0 };
		ImVec4 clear_clr = { 0,0,0,0 };

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		render(window_width, window_height);

		ImGui::Render();
		device_context->OMSetRenderTargets(1, &render_target_view, NULL);
		device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr); // might break, if so then put cheat color of type ImVec4 { 0,0,0,0 }
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swap_chain->Present(1, 0);

		return msg.message != WM_QUIT;
	}
	return false;
}

void c_overlay::input_handler() {
	// added a sleep per user @colbyfortnite's suggestion
	for (; !exit; Sleep(2)) {
		// for our imgui menu interaction
		ImGuiIO& io = ImGui::GetIO();

		POINT p{};
		GetCursorPos(&p);
		io.MousePos = ImVec2((float)p.x, (float)p.y);

		io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;
	}
}

void c_overlay::anim_handler() {
	// timers for our animations
	breath_timer.start();
	menu_timer.start();

	for (;!exit;) {
		RECT rect{ 0 };
		POINT point{ 0 };
		GetClientRect(window_handle, &rect);
		SetWindowPos(window_handle, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOREDRAW);

		// END Key to end hack
		exit = GetAsyncKeyState(VK_END) & 0x8000;
		
		// timer for our breathing animation
		int breath_tmr = breath_timer.end();
		if (breath_tmr >= breath_anim_time * 2) {
			breath = breath_anim_time; 
			breath_timer.start();
		}
		else if (breath_tmr > breath_anim_time) 
			breath = breath_anim_time - (breath_tmr - breath_anim_time);
		else if (breath_tmr <= breath_anim_time) 
			breath = breath_tmr;

		// timer for menu circle animation & fade in
		int menu_tmr = menu_timer.end();
		// INSERT key brings menu up and down
		if (GetAsyncKeyState(VK_INSERT) & 0x8000 && menu_tmr > menu_anim_time) {
			show_menu = !show_menu;
			menu_ticks = menu_anim_time;
			menu_timer.start();
		}
		else if (menu_tmr < menu_anim_time)
			menu_ticks = (show_menu ? menu_tmr : menu_anim_time - menu_tmr);
	}
}

VOID c_overlay::init_draw_list() {
	draw_list = ImGui::GetBackgroundDrawList();
}

VOID c_overlay::DrawLine(int x1, int y1, int x2, int y2, RGB color, int thickness) {
	draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), Color(color), thickness);
}

VOID c_overlay::DrawRect(int x, int y, int w, int h, RGB color, int thickness) {
	draw_list->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), Color(color), 0, 0, thickness);
}

VOID c_overlay::crosshair(const FLOAT aSize, ImU32 color) {
	draw_list->AddLine({ window_width / 2,window_height / 2 - (aSize + 1) }, { window_width / 2 ,window_height / 2 + (aSize + 1) }, color, 2);
	draw_list->AddLine({ window_width / 2 - (aSize + 1),window_height / 2 }, { window_width / 2 + (aSize + 1), window_height / 2 }, color, 2);
}

VOID c_overlay::box(const ImVec2& pos, const FLOAT width, const FLOAT height, ImU32 color, const FLOAT line_width) {
	std::array<ImVec2, 4> box_lines{ pos,ImVec2{pos.x + width,pos.y},ImVec2{pos.x + width,pos.y + height},ImVec2{pos.x,pos.y + height} };
	draw_list->AddPolyline(box_lines.data(), box_lines.size(), color, true, 2);
}

VOID c_overlay::line(const ImVec2& point1, const ImVec2 point2, ImU32 color, const FLOAT line_width) {
	draw_list->AddLine(point1, point2, color, line_width);
}

void c_overlay::draw_circle(float x, float y, float radius, RGB color) {
	draw_list->AddCircle(ImVec2(x, y), radius, Color(color), 0, 1);
}

VOID c_overlay::circle(const ImVec2& point, const FLOAT radius, ImU32 color) {
	draw_list->AddCircleFilled(point, radius, color, 200); // num segments is how circular the circle actually is
}

VOID c_overlay::text(const ImVec2& pos, const std::string& text, ImU32 color, bool outline) {
	if (outline) {
		draw_list->AddText(font->im_font, 18.f, { pos.x + 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x - 1, pos.y }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x, pos.y + 1 }, IM_COL32_BLACK, text.c_str());
		draw_list->AddText(font->im_font, 18.f, { pos.x, pos.y - 1 }, IM_COL32_BLACK, text.c_str());
	}
	draw_list->AddText(font->im_font, 18.f, pos, color, text.c_str());
}

VOID c_overlay::radial_gradient(const ImVec2& center, float radius, ImU32 col_in, ImU32 col_out) {
	if (((col_in | col_out) & IM_COL32_A_MASK) == 0 || radius < 0.5f)
		return;

	draw_list->_PathArcToFastEx(center, radius, 0, IM_DRAWLIST_ARCFAST_SAMPLE_MAX, 0);
	const int count = draw_list->_Path.Size - 1;

	unsigned int vtx_base = draw_list->_VtxCurrentIdx;
	draw_list->PrimReserve(count * 3, count + 1);

	const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
	draw_list->PrimWriteVtx(center, uv, col_in);
	for (int n = 0; n < count; n++)
		draw_list->PrimWriteVtx(draw_list->_Path[n], uv, col_out);
	
	for (int n = 0; n < count; n++) {
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
		draw_list->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
	}
	draw_list->_Path.Size = 0;
}

BOOL c_overlay::in_screen(const ImVec2& pos) {
	return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}