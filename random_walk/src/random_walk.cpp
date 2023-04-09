// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <thread>
#include <chrono>
#include <vector>
#include <math.h>
#include <list>
#include <numeric>
#include <random>
#include <algorithm>

//----------- system dynamic parameters --------------------

int W = 800;
int H = 800;
float step = 10.0f;
int N = ((int)W / step);
int D = 4; // number possible direction for the walker
float dt = 1.0f;

//---------------------------------------------------------------
int generateRandom(int n)
{

	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_int_distribution<int> dist(0, n);

	return dist(engine);
}

//---------------------------------------------------------------

struct Walker
{

	int x, y;
	std::vector<std::vector<int>> *visited;

	Walker()
	{
		std::vector<int> vi(N, 0);
		visited = new std::vector<std::vector<int>>(N, vi);

		this->x = generateRandom(N);
		this->y = generateRandom(N);

		for (int ii = 1; ii < visited->size() - 1; ii++)
		{
			for (int jj = 1; jj < visited->at(ii).size() - 1; jj++)
			{
				visited->at(ii)[jj] = 0;
			}
		}
		visited->at(this->y)[this->x] = 1;
	}

	/*
	0 - left
	1 - up
	2 - right
	3 - down
	*/

	std::vector<int> checkFreeStep(int ii, int jj)
	{

		std::vector<int> free(D, 1);
		if (ii < N - 1 && ii > 1)
		{
			free[1] = visited->at(ii - 1)[jj];
			free[3] = visited->at(ii + 1)[jj];
		}
		if (jj < N - 1 && jj > 1)
		{

			free[0] = visited->at(ii)[jj - 1];
			free[2] = visited->at(ii)[jj + 1];
		}
		return free;
	}

	void move()
	{

		auto free = checkFreeStep(this->x, this->y);
		if (std::count(free.begin(), free.end(), 0) == 0)
		{
			std::cout << "Game Over"
					  << "\n";
			return;
		}
		int next_step = generateRandom(D - 1);

		while (free[next_step] == 1)
		{

			next_step = generateRandom(D - 1);
		}

		if (next_step == 0)
		{
			visited->at(this->y)[this->x] = 1;
			if (this->x == 1)
			{
				return;
			}
			this->x -= 1;
		}
		else if (next_step == 1)
		{

			visited->at(this->x)[this->x] = 1;
			if (this->y == 1)
			{
				return;
			}
			this->y -= 1;
		}
		else if (next_step == 2)
		{

			visited->at(this->y)[this->x] = 1;
			if (this->x == N - 1)
			{
				return;
			}
			this->x += 1;
		}
		else
		{

			visited->at(this->y)[this->x] = 1;
			if (this->y == N - 1)
			{
				return;
			}
			this->y += 1;
		}

	}
};

//---------------------------------------------------------------

int main(int argc, char const *argv[])
{

	std::string title = "Random-walk";
	initImgui(W, H, title);

	// simulation state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 0.0f / 255.0, 1.00f);
	ImVec4 white_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 pink_color = ImVec4(245.0f / 255.0, 5.0f / 255.0, 150.0f / 255.0, 1.00f);
	ImVec4 blue_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 2550.0f / 255.0, 1.00f);

	// Main loop
	bool flag = true;
	Walker walker;

	std::vector<ImVec2> route;

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		static int num_sig = 3;

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(W, H), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		route.push_back({(float)walker.x * step, (float)walker.y * step});
		walker.move();

		for (int ii = 0; ii < route.size() - 1; ii++)
		{

			draw_list->AddCircleFilled({(float)route[ii].x, (float)route[ii].y}, 1.0f, ImColor(white_color));
			draw_list->AddLine(route[ii], route[ii+1], ImColor(pink_color), 1.0);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
