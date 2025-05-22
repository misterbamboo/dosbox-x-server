#include <iostream>
#include <vector>
#include <random>
#include <utility>
#undef max
#undef min
#include <cmath>

class PathGenerator {
public:
    static std::vector<std::pair<double, double>> generateHumanLikePath(
        double start_x,
        double start_y,
        double end_x,
        double end_y,
        int control_points = 3
    ) {
        double distance = std::sqrt((end_x - start_x) * (end_x - start_x) +
            (end_y - start_y) * (end_y - start_y));
        int steps = std::max(10, (int)(distance / 10.0));

        std::vector<double> control_xs = { start_x };
        std::vector<double> control_ys = { start_y };

        std::random_device rd;
        std::mt19937 gen(rd());
        double max_offset = std::min(100.0, (distance * 0.2));

        for(int i = 0; i < control_points; ++i) {
            double t = (i + 1.0) / (control_points + 1.0);
            double line_x = start_x + t * (end_x - start_x);
            double line_y = start_y + t * (end_y - start_y);

            std::uniform_real_distribution<> dis(-max_offset, max_offset);
            double control_x = line_x + dis(gen);
            double control_y = line_y + dis(gen);

            control_xs.push_back(control_x);
            control_ys.push_back(control_y);
        }

        control_xs.push_back(end_x);
        control_ys.push_back(end_y);

        std::vector<std::pair<double, double>> path;
        for(int i = 0; i <= steps; ++i) {
            double t = static_cast<double>(i) / steps;
            double x = bezierPoint(t, control_xs);
            double y = bezierPoint(t, control_ys);
            path.emplace_back(x, y);
        }

        return path;
    }

private:
    static double bezierPoint(double t, const std::vector<double>& control_points) {
        int n = control_points.size() - 1;
        double point = 0.0;

        for(int i = 0; i <= n; ++i) {
            double binomial = combination(n, i);
            double bernstein = binomial * std::pow(t, i) * std::pow(1 - t, n - i);
            point += control_points[i] * bernstein;
        }

        return point;
    }

    // Compute binomial coefficient "n choose k"
    static unsigned long long combination(int n, int k) {
        if(k > n) return 0;
        if(k == 0 || k == n) return 1;
        unsigned long long result = 1;
        for(int i = 1; i <= k; ++i) {
            result *= (n - (k - i));
            result /= i;
        }
        return result;
    }
};
