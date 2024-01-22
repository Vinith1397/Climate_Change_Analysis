
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>

// Function to check if the string can be converted to double
bool isDouble(const std::string &str)
{
    std::istringstream iss(str);
    double dbl;
    iss >> std::noskipws >> dbl; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}

// Function to perform linear regression and find the slope and intercept
void linearRegression(const std::vector<int> &x, const std::vector<double> &y, double &slope, double &intercept)
{
    size_t n = x.size(); // Number of data points

    double sum_x = 0.0;         // Sum of x
    double sum_y = 0.0;         // Sum of y
    double sum_xy = 0.0;        // Sum of (x * y)
    double sum_x_squared = 0.0; // Sum of (x^2)

    for (size_t i = 0; i < n; ++i)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x_squared += x[i] * x[i];
    }

    double mean_x = sum_x / n;
    double mean_y = sum_y / n;
    double ss_xy = sum_xy - n * mean_x * mean_y;
    double ss_xx = sum_x_squared - n * mean_x * mean_x;

    slope = ss_xy / ss_xx;
    intercept = mean_y - slope * mean_x;
}

int main()
{
    std::string filename = "Global.csv";
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::vector<std::string> > monthly_deviation;

    // Skip header line to avoid reading column titles
    std::getline(file, line);

    // Read file line by line
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> deviations;

        // Read year which is the first token before the comma
        std::getline(iss, token, ',');
        deviations.push_back(token);

        // Read each subsequent token after the year
        while (std::getline(iss, token, ','))
        {
            deviations.push_back(token);
        }

        monthly_deviation.push_back(deviations);
    }

    file.close();

    // Initialize vectors for yearly increase and stationary distribution slopes for each month
    std::vector<double> yearly_increase_slopes(12, 0.0);
    std::vector<double> stationary_distribution_slopes(12, 0.0);

    // Iterate over each month and calculate slopes
    for (size_t month = 1; month <= 12; ++month)
    {
        std::vector<int> years_with_data;
        std::vector<double> yearly_increase_by_month;

        for (size_t year = 1; year < monthly_deviation.size(); ++year)
        {
            if (isDouble(monthly_deviation[year][month]))
            {
                years_with_data.push_back(std::stoi(monthly_deviation[year][0]));
                yearly_increase_by_month.push_back(std::stod(monthly_deviation[year][month]));
            }
        }

        if (years_with_data.size() > 1)
        {
            double yearly_increase_slope, yearly_increase_intercept;
            linearRegression(years_with_data, yearly_increase_by_month, yearly_increase_slope, yearly_increase_intercept);
            yearly_increase_slopes[month - 1] = yearly_increase_slope;
        }

        std::vector<double> stationary_distribution_by_month;

        for (size_t year = 1; year < monthly_deviation.size(); ++year)
        {
            if (isDouble(monthly_deviation[year][month]))
            {
                double max_dev = -std::numeric_limits<double>::max();
                for (size_t prev_year = 1; prev_year <= year; ++prev_year)
                {
                    double prev_dev = isDouble(monthly_deviation[prev_year][month]) ? std::stod(monthly_deviation[prev_year][month]) : -std::numeric_limits<double>::max();
                    if (prev_dev > max_dev)
                    {
                        max_dev = prev_dev;
                    }
                }
                stationary_distribution_by_month.push_back(max_dev);
            }
        }

        if (stationary_distribution_by_month.size() > 1)
        {
            double stationary_distribution_slope, stationary_distribution_intercept;
            linearRegression(years_with_data, stationary_distribution_by_month, stationary_distribution_slope, stationary_distribution_intercept);
            stationary_distribution_slopes[month - 1] = stationary_distribution_slope;
        }
    }

    // Compare the slopes for each month and interpret the results
    for (size_t month = 1; month <= 12; ++month)
{
    std::cout << "Month: " << month << "\n";
    std::cout << "Yearly Increase Slope: " << yearly_increase_slopes[month - 1] << "\n";
    std::cout << "Stationary Distribution Slope: " << stationary_distribution_slopes[month - 1] << "\n";

    double slope_diff = yearly_increase_slopes[month - 1] - stationary_distribution_slopes[month - 1];

    if (yearly_increase_slopes[month - 1] > stationary_distribution_slopes[month - 1])
    {
        if (slope_diff < 0.001)
        {
            std::cout << "Interpretation: Stationary climate change in this month.\n";
        }
        else
        {
            std::cout << "Interpretation: Potential global warming in this month.\n";
        }
    }
    else
    {
        std::cout << "Interpretation: Potential global cooling in this month.\n";
    }

    std::cout << std::endl;
}


    return 0;
}
