#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>

// Define a structure to hold the temperature data
struct AnnualTemp {
    int year;
    double jd; // January-December average temperature anomaly
};

std::vector<AnnualTemp> readTemperatureData(const std::string& filename) {
    std::vector<AnnualTemp> data;
    std::ifstream file(filename);
    std::string line;

    // Skip the header line
    std::getline(file, line);

    int lineNumber = 1; // Start counting lines after the header

    while (std::getline(file, line)) {
        std::istringstream sstream(line);
        std::string field;
        AnnualTemp tempData;
        int fieldIndex = 0;
        bool isValidLine = true;

        while (std::getline(sstream, field, ',')) {
            try {
                if (fieldIndex == 0) { // First field is the year
                    tempData.year = std::stoi(field);
                } else if (fieldIndex == 13) { // 14th field is the J-D average
                    tempData.jd = std::stod(field);
                    data.push_back(tempData);
                    break; // No need to read the rest of the line
                }
            } catch (const std::invalid_argument& e) {
                //std::cerr << "Invalid argument at line " << lineNumber << ", field " << fieldIndex 
                  //        << ": " << field << std::endl;
                isValidLine = false;
                break;
            } 
            ++fieldIndex;
        }
         
    }
    return data;
}


// Function to calculate the mean (as shown previously)
// Function to calculate the slope (m) and intercept (b) of a linear trend line (as shown previously)
// Function to calculate the linear trend for each decade (as shown previously)
// Function to calculate the mean
double mean(const std::vector<double>& v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

// Function to calculate the slope (m) and intercept (b) of a linear trend line
void linearRegression(const std::vector<double>& x, const std::vector<double>& y, double& m, double& b) {
    double x_mean = mean(x);
    double y_mean = mean(y);
    double sum_xy = 0.0, sum_xx = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sum_xy += (x[i] - x_mean) * (y[i] - y_mean);
        sum_xx += (x[i] - x_mean) * (x[i] - x_mean);
    }
    
    m = sum_xy / sum_xx;
    b = y_mean - m * x_mean;
}

// Function to calculate the linear trend for each decade
void decadeTrendAnalysis(const std::vector<double>& years, const std::vector<double>& jdAnomalies) {
    // Assuming years are ordered and start from a full decade (e.g., 1880, 1890, ...)
    double m, b;
    
    for (size_t i = 0; i < years.size(); i += 10) {
        size_t end = std::min(i + 10, years.size());
        std::vector<double> decade_years(years.begin() + i, years.begin() + end);
        //std::vector<double> decade_temps(temps.begin() + i, temps.begin() + end);
        std::vector<double> decade_temps(jdAnomalies.begin() + i, jdAnomalies.begin() + end);

        linearRegression(decade_years, decade_temps, m, b);
        
        std::cout << "Decade " << static_cast<int>(decade_years.front()) << "s: "
                  << "Trend Slope (m) = " << m << ", "
                  << "Intercept (b) = " << b << std::endl;
    }
}

int main() {
    // Replace 'filename.csv' with your actual CSV filename
    std::vector<AnnualTemp> temperatureData = readTemperatureData("Global.csv");

   std::vector<double> years;
    std::vector<double> jdAnomalies;
    for (const auto& temp : temperatureData) {
        years.push_back(temp.year);
        jdAnomalies.push_back(temp.jd);
    }

    // Calculate overall trend
    double m, b;
    linearRegression(years, jdAnomalies, m, b);
    std::cout << "Overall Trend: Slope (m) = " << m << ", Intercept (b) = " << b << std::endl;

    // Decade-wise trend analysis
    decadeTrendAnalysis(years, jdAnomalies);
    
std::ofstream output_file("trend_analysis.csv");
output_file << "Decade, Slope, Intercept\n";

// Output for overall trend
output_file << "Overall," << m << "," << b << "\n";

// Output for decade-wise trends
for (size_t i = 0; i < years.size(); i += 10) {
    size_t end = std::min(i + 10, years.size());
    std::vector<double> decade_years(years.begin() + i, years.begin() + end);
    std::vector<double> decade_temps(jdAnomalies.begin() + i, jdAnomalies.begin() + end);

    linearRegression(decade_years, decade_temps, m, b);
    output_file << static_cast<int>(decade_years.front()) << "s," << m << "," << b << "\n";
}

output_file.close();



    return 0;
}
