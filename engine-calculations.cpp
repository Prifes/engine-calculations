#include <iostream>
#include <math.h>
#include "file_handler.h"
#include "plot.h"

//#define __GRAPHICS
#ifdef __GRAPHICS__
#include "raylib.h"
#endif

static constexpr double PI = 3.14159265358979323846;
static constexpr int p_sea_level = 101325;
static constexpr float g = 9.81;
static constexpr float air_molar_mass = 0.02896968;
static constexpr float T_standard = 288.15;
static constexpr float R = 8.314462618;

static double height_to_pressure(double h) {
	return p_sea_level * exp(-g * h * air_molar_mass / (T_standard * R));
}

static double pressure_to_height(double p) {
	return -log(p / p_sea_level) * T_standard * R / (g * air_molar_mass);
}

static double area_ratio(double M, double gamma) {
	double term = 1.0f + (gamma - 1.0f) / 2.0f * M * M;
	double exponent = (gamma + 1.0f) / (2.0f * (gamma - 1.0f));
	return (1.0f / M) * pow((2.0f / (gamma + 1.0f)) * term, exponent);
}

static double find_mach_number(double Ae_At, double gamma, double initial_guess = 2.0f, double tol = 1e-6, int max_iter = 100) {
	double M = initial_guess;
	for (int i = 0; i < max_iter; ++i) {
		double f = Ae_At - area_ratio(M, gamma);

		double h = 1e-5;
		double f_plus = Ae_At - area_ratio(M + h, gamma);
		double f_minus = Ae_At - area_ratio(M - h, gamma);
		double f_deriv = (f_plus - f_minus) / (2.0f * h);

		double M_new = M - f / f_deriv;

		if (std::abs(M_new - M) < tol) {
			return M_new;
		}
		M = M_new;
	}
	return M;
}

int main() {
	// Reading input paramters
	FileHandler fh("input.txt");
	FileHandler::InputData data;
	if (!fh.Read(data)) {
		std::cout << "error reading the file\n";
		throw 1;
	}

	// Converting read values to SI units
	data.p1 *= 100000;
	data.T1 += 273;
	data.h0 *= 1000;
	data.h1 *= 1000;
	data.F0 *= 1000;

	// Calculating c*
	const double m_dot = (data.m0 - data.m1) / data.t;
	const double A_throat = m_dot / data.p1 * sqrt(data.R * data.T1 / (data.kap * pow(2.0f / (data.kap + 1.0f), (data.kap + 1.0f) / (data.kap - 1.0f))));
	const double c_star = data.p1 * A_throat / m_dot;
	std::cout 
		<< "Task 1\n"
		<< "Characteristic speed = " << c_star << " m/s\n";

	// Calculating throat paramters
	const double D_throat = sqrt(A_throat / PI) * 2.0f;
	const double T_throat = 2 * data.T1 / (data.kap + 1.0f);
	const double p_throat = data.p1 * pow(2.0f / (data.kap + 1.0f), data.kap / (data.kap - 1.0f));
	const double V_throat = data.R * T_throat / p_throat;
	std::cout
		<< "\nTask 2\n"
		<< "Throat pressure = " << p_throat/1000 << " kPa\n"
		<< "Throat temperature = " << T_throat << " K\n"
		<< "Throat density = " << 1 / V_throat << " kg/m3\n"
		<< "Throat diameter = " << D_throat << " m\n"
		<< "Throat area = " << A_throat << " m2\n"
		<< "Mass flow rate = " << m_dot << " kg/s\n";

	// Calculating exit parameters and height optimum
	const double A_exit = A_throat * data.eps;
	const double M_exit = find_mach_number(data.eps, data.kap);
	const double p_exit = data.p1 * pow(1 + (data.kap - 1.0f) / 2.0f * M_exit * M_exit, - data.kap / (data.kap - 1.0f));
	const double h_optimal = pressure_to_height(p_exit);
	std::cout
		<< "\nTask 3\n"
		<< "Exit pressure = " << p_exit/1000 << " kPa\n"
		<< "Optimal height = " << h_optimal << " m\n";

	// Initializing graphics
	std::cout << "\nTask4\n";

	#ifdef __GRAPHICS__
	SetTraceLogLevel(LOG_ERROR);
	InitWindow(1500, 750, "Altitude - Thrust Plots");
	SetTargetFPS(10);

	Plot thrust_to_height(0, 0, 750, 750, "km", "kN", RED);
	Plot thrust_coefficient_to_height(750, 0, 750, 750, "km", "", BLUE);
	#endif

	// Calculating plot values
	const double w = (data.F0 - (p_exit - p_sea_level) * A_exit) / m_dot;
	for (double h = data.h0; h <= data.h1; h += (data.h1 - data.h0) / 20.0f) {
		const double p_env = height_to_pressure(h);
		const double thrust = w * m_dot + (p_exit - p_env) * A_exit;
		#ifdef __GRAPHICS__
		thrust_to_height.AddPoint(h / 1000, thrust / 1000);
		#endif
		std::cout << "F(" << h << " m) = " << thrust / 1000 << " kN; ";

		const double thrust_coefficient = sqrt(
			(2.0f * data.kap * data.kap) / (data.kap - 1.0f) *
			pow(2.0f / (data.kap + 1.0f), (data.kap + 1.0f) / (data.kap - 1.0f)) *
			(1.0f - pow(p_exit / data.p1, (data.kap - 1.0f) / data.kap))
		) + ((p_exit - p_env) / data.p1) * (A_exit / A_throat);
		#ifdef __GRAPHICS__
		thrust_coefficient_to_height.AddPoint(h / 1000, thrust_coefficient);
		#endif
		std::cout << "CF(" << h << " m) = " << thrust_coefficient << "\n";
	}

	#ifdef __GRAPHICS__
	//Drawing plots
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(WHITE);
		thrust_to_height.Draw();
		thrust_coefficient_to_height.Draw();
		EndDrawing();
	}

	CloseWindow();

	#endif

	return 0;

}
