/*
Copyright [C] 2024 Armand FreeWorld pre-alpha
All right reserved
*/

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <queue>
#include <cmath>

void clearScreen() {
#ifdef _WIN32
	system("cls"); // Windows 系统
#else
	system("clear"); // Unix/Linux 系统
#endif
}

#define VERSION "" // 版本号
#define AUTHOR "Armand" // 作者
#define COPYRIGHT "Copyright [C] 2024 Armand FreeWorld " + VERSION // 版权信息

namespace FreeWorld {
	using namespace std;

	class MapPrinter;

	class TerrainGenerator {
	private:
		int MAP_WIDTH = 20; // 地图宽度
		int MAP_HEIGHT = 256; // 地图高度

		int terrain_height = MAP_HEIGHT / 2; // 地形高度
		vector<vector<int>> game_map; // 地图数据
		vector<vector<int>> new_game_map; // 地图副本

		int player_x = 0; // 玩家x坐标
		int player_y = 0; // 玩家y坐标

		int seed = 2022812; // 随机种子

		int getRandomSeed(int range) {
			return rand() % range;
		} // 随机种子生成器

		pair<vector<vector<int>>, int> terrainGenerator(int seed, int new_terrain_height) {
			vector<vector<int>> new_game_map(MAP_HEIGHT, vector<int>(MAP_WIDTH, 0));
			srand(seed);
			for (int i = MAP_WIDTH - 1; i >= 0; i--) {
				switch (getRandomSeed(2)) {
				case 0:
					new_terrain_height -= getRandomSeed(3);
					break;
				case 1:
					new_terrain_height += getRandomSeed(3);
					break;
				}

				for (int j = MAP_HEIGHT - 1; j > new_terrain_height; j--) {
					new_game_map[j][i] = 1;
				}
				int soil_height = getRandomSeed(2) + 1;
				for (int m = 0; m < soil_height; m++) {
					new_game_map[new_terrain_height - m][i] = 2;
				}
				new_game_map[new_terrain_height - soil_height][i] = 3;

				int grass_height = soil_height + 1;
				int tree_height = getRandomSeed(4) + 4;
				switch (getRandomSeed(24)) {
				case 0:
					for (int n = 0; n < tree_height; n++) {
						new_game_map[new_terrain_height - grass_height - n][i] = 4;
					}
					new_game_map[new_terrain_height - grass_height + 1][i] = 2;

					// 这不是一个很好的解决方案，待修改
					new_game_map[new_terrain_height - grass_height - tree_height - 3][i] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 1][i - 1] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 1][i + 1] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 1][i - 2] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 1][i] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 1][i + 2] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 2][i - 1] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 2][i] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height - 2][i + 1] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height][i - 2] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height][i + 2] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height][i - 1] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height][i] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height][i + 1] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height][i - 3] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height][i + 3] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height + 1][i - 1] = 5;
					new_game_map[new_terrain_height - grass_height - tree_height + 1][i + 1] = 5;
	
					break;
				}

			}

			return make_pair(new_game_map, new_terrain_height);

		} // 地形生成器

	public:
		TerrainGenerator(void) {
			vector<vector<int>> game_map(MAP_HEIGHT, vector<int>(MAP_WIDTH, 0)); // 地图数据
			//auto new_game_map = game_map; // 地图副本
		}

		friend class MapPrinter; 
		friend void gameStart(TerrainGenerator& terrain_generator); // 友元声明

		void setBornPoint() {
			player_y = 30;
			player_x = MAP_HEIGHT - 1;

			while (game_map[player_x][player_y] != 0) {
				player_x--;
			}
		} // 设置出生点

		void terrrainGeneratorPublic(int number) {
			int new_terrain_height;
			tie(game_map,terrain_height) = terrainGenerator(seed, terrain_height);
			terrain_height = terrain_height - 10;

			for (int i = 1; i < number; i++) {
				MAP_WIDTH += 20;
				tie(new_game_map, terrain_height) = terrainGenerator(seed, terrain_height);
				for (int j = 0; j < MAP_HEIGHT; j++) {
					for (auto it = new_game_map[j].begin(); it != new_game_map[j].end(); ++it) {
						game_map[j].push_back(*it);
					}
				}
			}
		} // 地形生成器公共接口

		void setPlayerPosition(int x, int y) {
			player_x += x;
			player_y += y;
		} // 设置玩家位置

	};

	TerrainGenerator terrain_generator; // 地形生成器实例

	class MapPrinter {
	private:
		map<int, string> colours = {
			{0, "\033[48;5;12m"}, // 空气
			{1, "\033[48;2;192;192;192m"}, // 石头
			{2, "\033[48;5;52m"}, // 泥土
			{3, "\033[48;5;118m"}, // 草
			{4, "\033[48;2;128;128;16m"}, // 树木
			{5, "\033[48;2;64;192;32m"}, // 树叶
		}; // 颜色lookup表

		const int PRINT_WIDTH = 20; // 打印宽度
		const int PRINT_HEIGHT = 20; // 打印高度

	public:
		void printWholeMap() {
			for (int i = 0; i < terrain_generator.MAP_HEIGHT; i++) {
				for (int j = 0; j < terrain_generator.MAP_WIDTH; j++) {
					cout << colours[terrain_generator.game_map[i][j]] << "  \033[0m";
				}
				cout << endl;
			}
		} // 测试：打印全部地图

		void printMap() {
			int startX = terrain_generator.player_x - PRINT_HEIGHT / 2;
			int startY = terrain_generator.player_y - PRINT_WIDTH / 2;
			int endX = terrain_generator.player_x + PRINT_HEIGHT / 2;
			int endY = terrain_generator.player_y + PRINT_WIDTH / 2;

			for (int i = startX; i < endX; i++) {
				for (int j = startY; j < endY; j++) {
					if (terrain_generator.player_x == i && terrain_generator.player_y == j) {
						cout << colours[terrain_generator.game_map[i][j]] << "！\033[0m"; // 玩家位置
					}
					else {
						//if (game_map[i][j] == 3) {
						//	cout << colours[game_map[i][j]] << "▀▀\033[0m";
						//}
						//else if (game_map[i][j] == 4) {
						//	cout << colours[game_map[i][j]] << "▒▒\033[0m";
						//}
						//else {
						cout << colours[terrain_generator.game_map[i][j]] << "  \033[0m"; // 其他地图部分
						//}
					}
				}
				cout << endl;
			}
		} // 打印以玩家为中心的10x10地图
	};

	MapPrinter map_printer; // 地图打印器实例

	class Debuger {
	private:
		queue<string> debug_queue; // 调试信息队列

	public:
		void attendDebug(string info) {
			debug_queue.push(info);
		} // 调试信息记录器

		void printDebug() {
			queue<string> temp_debug_queue = debug_queue;
			cout << "\n----------Debug info:-----------" << endl;
			while (!temp_debug_queue.empty()) {
				cout << temp_debug_queue.front() << endl;
				temp_debug_queue.pop();
			}
			cout << "-------------End Debug------------" << endl;
		} // 调试信息打印器
	};

    Debuger debuger; // 调试管理器实例

	string update_info = R"(FreeWorld 更新日志：
1. 增加了开头
	·进入游戏后，将展示游戏版本，作者和更新日志
	·玩家可以自行输入种子
2.地形修改
	·加入了四种新的方块：
		·-泥土：自然生成于石头上方，告诉取决于种子
		·-草：自然生成于泥土上方，采用特殊字符代替
		·-树木：以不定规律生成，若草方块上生成树木，则该草方块被替换为泥土
		·-树叶：以不定规律生成，若树木方块上生成树叶
3.修改了已知问题
	·修复了玩家生成点可能在地形下方的问题，重写了SetBornPoint函数
4.数据抽象和封装
	·将地形生成等过程封装为类
	·将地图数据，玩家位置等封装为类私有变量，提供接口进行访问
    ·定义友元函数，方便地图数据访问
)";

	void shut() {
		cout << "任意键继续>>>";
		//cin.ignore(numeric_limits<streamsize>::max(), '\n');
		//cin.get();
		string answer;
		cin >> answer;
	}

	void gameStart(TerrainGenerator& terrain_generator) {
		cout << "欢迎来到FreeWorld " << VERSION << "！" << endl;
		cout << "作者：" << AUTHOR << endl;
		cout << update_info << endl;
		shut();

		cout << "请输入种子：";
		int seed;
		cin >> seed;
		terrain_generator.seed = seed;

		clearScreen();
	} // 游戏开始

	void mainLoop() {
		gameStart(terrain_generator);

		terrain_generator.terrrainGeneratorPublic(5);
		terrain_generator.setBornPoint();

		cout << "地形生成中..." << endl;

		clearScreen();
			
		while (true) {
			map_printer.printMap();
			string input;
			cin >> input;
			if (input == "w") {
				terrain_generator.setPlayerPosition(-1, 0);
				debuger.attendDebug("玩家向上行进一个单位。");
			}
			else if (input == "s") {
				terrain_generator.setPlayerPosition(1, 0);
				debuger.attendDebug("玩家向下行进一个单位。");
			}
			else if (input == "a") {
				terrain_generator.setPlayerPosition(0, -1);
				debuger.attendDebug("玩家向左行进一个单位。");
			}
			else if (input == "d") {
				terrain_generator.setPlayerPosition(0, 1);
				debuger.attendDebug("玩家向右行进一个单位。");
			}
			else if (input == "/debug") {
				debuger.attendDebug("显示调试信息。");
				debuger.printDebug();
				shut();
			}

			clearScreen();
		}
	} // 游戏主循环

}

int main(int argc, char* argv[]) {
	try {
		FreeWorld::mainLoop();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		FreeWorld::debuger.printDebug();
		return 1;
	}
	return 0;  
}
