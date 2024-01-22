#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm> // for std::min

// Define a structure to hold the temperature data
struct AnnualTemp {
    int year;
    double jd; // January-December average temperature anomaly
    double seasonal[4]; // Seasonal temperature anomalies (DJF, MAM, JJA, SON)
    double monthly[12]; // Monthly temperature anomalies
    double yearlyDeviation; // Yearly mean deviation (J-D column)
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
                } else if (fieldIndex >= 1 && fieldIndex <= 4) { // Fields 2-5 are DJF, MAM, JJA, SON
                    tempData.seasonal[fieldIndex - 1] = std::stod(field);
                } else if (fieldIndex >= 5 && fieldIndex < 17) { // Fields 6-17 are monthly data
                    tempData.monthly[fieldIndex - 5] = std::stod(field);
                }
            } catch (const std::invalid_argument& e) {
                isValidLine = false;
                break;
            }
            ++fieldIndex;
        }

        if (!isValidLine) {
            std::cerr << "Invalid data at line " << lineNumber << ": " << line << std::endl;
        } else {
            // Calculate yearly deviation (J-D column)
            tempData.yearlyDeviation = tempData.jd;
            data.push_back(tempData);
        }

        ++lineNumber;
    }
    return data;
}

// Function to calculate the mean of a vector of values
double calculateMean(const std::vector<double>& values) {
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

// Function to calculate the slope (m) and intercept (b) of a linear trend line
void linearRegression(const std::vector<double>& x, const std::vector<double>& y, double& m, double& b) {
    double x_mean = calculateMean(x);
    double y_mean = calculateMean(y);
    double sum_xy = 0.0, sum_xx = 0.0;

    for (size_t i = 0; i < x.size(); ++i) {
        sum_xy += (x[i] - x_mean) * (y[i] - y_mean);
        sum_xx += (x[i] - x_mean) * (x[i] - x_mean);
    }

    m = sum_xy / sum_xx;
    b = y_mean - m * x_mean;
}

// Function to calculate the linear trend for each decade
void decadeTrendAnalysis(const std::vector<double>& years, const std::vector<double>& temps) {
    // Assuming years are ordered and start from a full decade (e.g., 1880, 1890, ...)
    double m, b;

    for (size_t i = 0; i < years.size(); i += 10) {
        size_t end = std::min(i + 10, years.size());
        std::vector<double> decade_years(years.begin() + i, years.begin() + end);
        std::vector<double> decade_temps(temps.begin() + i, temps.begin() + end);

        linearRegression(decade_years, decade_temps, m, b);

        std::cout << "Decade " << static_cast<int>(decade_years.front()) << "s: "
                  << "Trend Slope (m) = " << m << ", "
                  << "Intercept (b) = " << b << std::endl;
    }
}

// Function to calculate seasonal averages and identify changes in seasonal temperature patterns
void seasonalAnalysis(std::vector<AnnualTemp>& temperatureData) {
    for (AnnualTemp& data : temperatureData) {
        // Calculate seasonal averages (DJF, MAM, JJA, SON)
        data.seasonal[0] = (data.monthly[11] + data.monthly[0] + data.monthly[1]) / 3.0; // DJF
        data.seasonal[1] = (data.monthly[2] + data.monthly[3] + data.monthly[4]) / 3.0;  // MAM
        data.seasonal[2] = (data.monthly[5] + data.monthly[6] + data.monthly[7]) / 3.0;  // JJA
        data.seasonal[3] = (data.monthly[8] + data.monthly[9] + data.monthly[10]) / 3.0; // SON
    }
}

// Function to analyze monthly deviations and identify months with the greatest deviations
void monthlyAnalysis(const std::vector<AnnualTemp>& temperatureData) {
    // Initialize vectors to store monthly deviations
    std::vector<std::vector<double>> monthlyDeviations(12);

    // Calculate monthly deviations for each year
    for (const AnnualTemp& data : temperatureData) {
        for (int month = 0; month < 12; ++month) {
            double deviation = data.monthly[month] - data.seasonal[month / 3];
            monthlyDeviations[month].push_back(deviation);
        }
    }

    // Identify months with the greatest positive and negative deviations
    for (int month = 0; month < 12; ++month) {
        double meanDeviation = calculateMean(monthlyDeviations[month]);
        double maxDeviation = *std::max_element(monthlyDeviations[month].begin(), monthlyDeviations[month].end());
        double minDeviation = *std::min_element(monthlyDeviations[month].begin(), monthlyDeviations[month].end());

        std::cout << "Month " << month + 1 << " Deviations:" << std::endl;
        std::cout << "  Mean Deviation: " << meanDeviation << std::endl;
        std::cout << "  Max Deviation: " << maxDeviation << std::endl;
        std::cout << "  Min Deviation: " << minDeviation << std::endl;
    }
}

// Function to analyze yearly mean deviations and identify the warmest and coldest years
void yearlyAnalysis(const std::vector<AnnualTemp>& temperatureData) {
    for (const AnnualTemp& data : temperatureData) {
        int hottestMonthIndex = 0;
        int coldestMonthIndex = 0;
        double maxTemp = data.monthly[0];
        double minTemp = data.monthly[0];

        // Find the hottest and coldest month
        for (int i = 1; i < 12; ++i) {
            if (data.monthly[i] > maxTemp) {
                maxTemp = data.monthly[i];
                hottestMonthIndex = i;
            }
            if (data.monthly[i] < minTemp) {
                minTemp = data.monthly[i];
                coldestMonthIndex = i;
            }
        }

        std::cout << "Year " << data.year << std::endl;
        std::cout << "  Hottest Month: " << hottestMonthIndex + 1 << " with anomaly of " << maxTemp << std::endl;
        std::cout << "  Coldest Month: " << coldestMonthIndex + 1 << " with anomaly of " << minTemp << std::endl;
    }
}

int main() {
    // Replace 'Global.csv' with your actual CSV filename
    std::vector<AnnualTemp> temperatureData = readTemperatureData("Global.csv");

    std::vector<double> years(temperatureData.size());
    std::vector<double> jdAnomalies(temperatureData.size());

    for (size_t i = 0; i < temperatureData.size(); ++i) {
        years[i] = static_cast<double>(temperatureData[i].year);
        jdAnomalies[i] = temperatureData[i].jd;
    }

    // Calculate overall trend
    double overallSlope, overallIntercept;
    linearRegression(years, jdAnomalies, overallSlope, overallIntercept);
    std::cout << "Overall Trend: Slope (m) = " << overallSlope << ", Intercept (b) = " << overallIntercept << std::endl;

    // Perform seasonal analysis
    //seasonalAnalysis(temperatureData);

    // Perform monthly analysis
    monthlyAnalysis(temperatureData);

    // Perform yearly analysis
    yearlyAnalysis(temperatureData);

    return 0;
}
