// C++port of Nupic HTM with the aim of being lite and fast
//
// Copyright (c) 2017 Henk-Jan Lebbink
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero Public License version 3 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero Public License for more details.
//
// You should have received a copy of the GNU Affero Public License
// along with this program.  If not, see http://www.gnu.org/licenses.

#include <chrono>
#include <stdio.h>  // for printf

#include "..\Spike-Tools-Lib\log.ipp"
#include "..\Spike-Tools-Lib\timing.ipp"
#include "..\Spike-Tools-Lib\profiler.ipp"

#include "..\HTM-Lite-LIB\constants.ipp"
#include "..\HTM-Lite-LIB\types.ipp"
#include "..\HTM-Lite-LIB\layer.ipp"
#include "..\HTM-Lite-LIB\swarm.ipp"
#include "..\HTM-Lite-LIB\network.ipp"

using namespace htm;
using namespace htm::types;

inline void test_layer()
{
	// static properties: properties that need to be known at compile time:

	//const int N_BLOCKS = 8; // 512 columns: use sparsity 0.05 -> 25
	//const int N_BLOCKS = 4096; // 262144 columns: use sparsity of 0.005 -> 1310
	//const int N_BLOCKS = 16384; // 1048576 columns: use sparsity of 0.002 -> 2048
	const int N_BLOCKS = 1 * 8;
	const int N_COLUMNS = 64 * N_BLOCKS;
	const int N_BITS_CELL = 4;
	const int N_SENSORS_DIM1 = 20;
	const int N_SENSORS_DIM2 = 20;
	const int N_VISIBLE_SENSORS = N_SENSORS_DIM1 * N_SENSORS_DIM2;
	const int N_HIDDEN_SENSORS = 0;

	const int HISTORY = 8;
	const arch_t ARCH = arch_t::RUNTIME;

	// dynamic properties: properties that can be changed while the program is running.
	Dynamic_Param param;
	param.learn = true;
	param.n_time_steps = 300;
	param.n_times = 10;
	param.n_visible_sensors_dim1 = N_SENSORS_DIM1;
	param.n_visible_sensors_dim2 = N_SENSORS_DIM2;

	param.progress = false;
	param.progress_display_interval = 20;

	param.TP_DD_SEGMENT_ACTIVE_THRESHOLD = 19;
	param.MIN_DD_ACTIVATION_THRESHOLD = 14;
	param.TP_DD_MAX_NEW_SYNAPSE_COUNT = 25;

	const std::string input_filename = "../../Misc/data/ABBCBBA_20x20/input.txt";
	//const std::string input_filename = "../../Misc/data/AAAX_16x16/input.txt";

	using P = Static_Param<N_COLUMNS, N_BITS_CELL, N_VISIBLE_SENSORS, N_HIDDEN_SENSORS, HISTORY, ARCH>;
	Layer<P> layer;
	auto data = encoder::encode_pass_through<P>(input_filename, param);

	htm::layer::run_multiple_times(data, layer, param);
}

inline void test_network()
{
	// static properties: properties that need to be known at compile time:

	//const int N_BLOCKS = 8; // 512 columns: use sparsity 0.05 -> 25
	//const int N_BLOCKS = 4096; // 262144 columns: use sparsity of 0.005 -> 1310
	//const int N_BLOCKS = 16384; // 1048576 columns: use sparsity of 0.002 -> 2048
	const int N_BLOCKS_L1 = 4 * 4 * 8;
	const int N_COLUMNS_L1 = 64 * N_BLOCKS_L1;
	const int N_BITS_CELL_L1 = 4;
	const int N_SENSORS_DIM1 = 20;
	const int N_SENSORS_DIM2 = 20;
	const int N_VISIBLE_SENSORS_L1 = N_SENSORS_DIM1 * N_SENSORS_DIM2;
	const int HISTORY_L1 = 8;

	const int N_BLOCKS_L2 = 2* 4 * 8;
	const int N_COLUMNS_L2 = 64 * N_BLOCKS_L2;
	const int N_BITS_CELL_L2 = 4;
	const int HISTORY_L2 = 8;

	const arch_t ARCH = arch_t::RUNTIME;

	// dynamic properties: properties that can be changed while the program is running.
	Dynamic_Param param1;
	param1.learn = true;
	param1.n_time_steps = 300;
	param1.n_times = 1;
	param1.n_visible_sensors_dim1 = N_SENSORS_DIM1;
	param1.n_visible_sensors_dim2 = N_SENSORS_DIM2;

	param1.progress = false;
	param1.progress_display_interval = 20;

	param1.TP_DD_SEGMENT_ACTIVE_THRESHOLD = 19;
	param1.MIN_DD_ACTIVATION_THRESHOLD = 14;
	param1.TP_DD_MAX_NEW_SYNAPSE_COUNT = 25;

	Dynamic_Param param2(param1);

	const std::string input_filename = "../../Misc/data/ABBCBBA_20x20/input.txt";
	if (false)
	{
		using Network_Config = network::network_2Layer<
			N_VISIBLE_SENSORS_L1,
			N_COLUMNS_L1, N_BITS_CELL_L1, HISTORY_L1,
			N_COLUMNS_L2, N_BITS_CELL_L2, HISTORY_L2,
			ARCH>;

		using P1 = Network_Config::P_L1;
		using P2 = Network_Config::P_L2;

		Layer<P1> layer1;
		Layer<P2> layer2;

		auto data = encoder::encode_pass_through<P1>(input_filename, param1);
		htm::network::run_multiple_times(data, layer1, layer2, param1, param2);
	}
	else
	{
		Dynamic_Param param3(param1);

		const int N_BLOCKS_L3 = 1 * 8;
		const int N_COLUMNS_L3 = 64 * N_BLOCKS_L3;
		const int N_BITS_CELL_L3 = 4;
		const int HISTORY_L3 = 8;

		using Network_Config = network::network_3Layer<
			N_VISIBLE_SENSORS_L1,
			N_COLUMNS_L1, N_BITS_CELL_L1, HISTORY_L1,
			N_COLUMNS_L2, N_BITS_CELL_L2, HISTORY_L2,
			N_COLUMNS_L3, N_BITS_CELL_L3, HISTORY_L3,
			ARCH>;

		using P1 = Network_Config::P_L1;
		using P2 = Network_Config::P_L2;
		using P3 = Network_Config::P_L3;
		
		Layer<P1> layer1;
		Layer<P2> layer2;
		Layer<P3> layer3;

		auto data = encoder::encode_pass_through<P1>(input_filename, param1);
		htm::network::run_multiple_times(data, layer1, layer2, layer3, param1, param2, param3);
	}
}

inline void test_swarm()
{
	// static properties: properties that need to be known at compile time:

	//const int N_BLOCKS = 9; // 1024 columns
	//const int N_BLOCKS = 4096; // 262144 columns
	//const int N_BLOCKS = 16384; // 1048576 columns
	const int N_BLOCKS = 10 * 8;
	const int N_COLUMNS = 64 * N_BLOCKS;
	const int N_BITS_CELL = 4;
	const int N_SENSOR_DIM1 = 20;
	const int N_SENSOR_DIM2 = 20;
	const int N_HIDDEN_VISIBLE_SENSORS = 0;
	const arch_t ARCH = arch_t::RUNTIME;
	const int HISTORY_SIZE = 8;
	const int N_VISIBLE_SENSORS = N_SENSOR_DIM1 * N_SENSOR_DIM2;

	Dynamic_Param param;
	param.learn = true;
	param.n_time_steps = 1000;
	param.n_times = 1;
	param.progress = false;

	const std::string input_filename = "../../Misc/data/ABBCBBA_20x20/input.txt";
	//const std::string input_filename = "../../Misc/data/AAAX_16x16/input.txt";
	//htm::swarm::run_random(input_filename, param);

	// Swarm options
	const std::string outputfilename = "C:/Temp/VS/htm_ga_results.csv";
	swarm::Swarm_Options options;
	options.population_size = 100;
	options.n_epochs = 100;
	options.mutation_rate = 0.1;
	options.quiet = false;

	std::ofstream file; // out file stream
	options.outputfile.open(outputfilename, std::ios::out | std::ios::trunc);
	if (options.outputfile.good())
		log_INFO("swarm:run_ga: writing results to file ", outputfilename, "\n");
	else
		log_WARNING("swarm:run_ga: could not write to file ", outputfilename, "\n");

	using P = Static_Param<N_COLUMNS, N_BITS_CELL, N_VISIBLE_SENSORS, N_HIDDEN_VISIBLE_SENSORS, HISTORY_SIZE, ARCH>;
	htm::swarm::run_ga<P, N_SENSOR_DIM1, N_SENSOR_DIM2>(input_filename, param, options);
}

int main()
{
	const auto start_time = std::chrono::system_clock::now();
	//test_layer();
	test_network();
	//test_swarm();
	const auto end_time = std::chrono::system_clock::now();

	printf("Elapsed Time: %s\n", ::tools::timing::elapsed_time_str(start_time, end_time).c_str());
	printf("\n-------------------\n");
	printf("\nPress RETURN to finish:");
	getchar();
	return 0;
}