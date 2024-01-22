#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

struct TemperatureData {
    int year;
    std::vector<double> monthlyDeviations;  // Store all monthly deviations in a single vector
};

void readDataset(const std::string& filename, std::vector<TemperatureData>& data) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    // Read and ignore the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        // Remove potential white spaces at the beginning of the line
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));

        if (line.empty() || line.front() == 'Y') {
            continue; // Skip the header line or empty lines
        }

        std::istringstream iss(line);
        TemperatureData entry;

        std::string token;
        if (!std::getline(iss, token, ',')) {
            std::cerr << "Error: Unable to read year from line." << std::endl;
            continue; // Skip to the next line
        }
        entry.year = std::stoi(token);  // Parse year

        // Read monthly temperature deviations
        double value;
        for (int i = 0; i < 12; ++i) { // Only read the first 12 values for the months
            if (!std::getline(iss, token, ',')) {
                std::cerr << "Error: Unable to read month data for year " << entry.year << std::endl;
                break; // Exit the loop for this line and move to the next
            }
            value = std::stod(token);
            entry.monthlyDeviations.push_back(value);
        }

        // Only add this entry if all 12 months have been read successfully
        if (entry.monthlyDeviations.size() == 12) {
            data.push_back(entry);
        } else {
            std::cerr << "Error: Incorrect number of monthly values for year " << entry.year << std::endl;
        }
    }

    file.close();
}


bool isExtremeEvent(const std::vector<double>& deviations, int consecutiveMonthsThreshold, bool isHeatwave) {
    int consecutiveMonths = 0;

    for (double deviation : deviations) {
        if (isHeatwave ? (deviation > 0) : (deviation < 0)) {
            if (++consecutiveMonths >= consecutiveMonthsThreshold) {
                return true;
            }
        } else {
            consecutiveMonths = 0;
        }
    }

    return false;
}



void analyzeEventFrequencyDuration(const std::vector<TemperatureData>& data, int consecutiveMonthsThreshold) {
    int heatwaveCount = 0, coldSnapCount = 0;
    int heatwaveDuration = 0, coldSnapDuration = 0;
    bool inHeatwave = false, inColdSnap = false;

    for (const auto& entry : data) {
        bool isHeatwave = isExtremeEvent(entry.monthlyDeviations, consecutiveMonthsThreshold, true);
        bool isColdSnap = isExtremeEvent(entry.monthlyDeviations, consecutiveMonthsThreshold, false);

        // Heatwave logic
        if (isHeatwave) {
            if (!inHeatwave) {
                inHeatwave = true;
                heatwaveCount++;
            }
            heatwaveDuration++;
        } else if (inHeatwave) {
            std::cout << "Heatwave ended in year " << entry.year << " with a duration of " << heatwaveDuration << " months." << std::endl;
            heatwaveDuration = 0;
            inHeatwave = false;
        }

        // Cold snap logic
        if (isColdSnap) {
            if (!inColdSnap) {
                inColdSnap = true;
                coldSnapCount++;
            }
            coldSnapDuration++;
        } else if (inColdSnap) {
            std::cout << "Cold snap ended in year " << entry.year << " with a duration of " << coldSnapDuration << " months." << std::endl;
            coldSnapDuration = 0;
            inColdSnap = false;
        }
    }

    // Handle ongoing events at the end of data
    if (inHeatwave) {
        std::cout << "Ongoing heatwave with a duration of " << heatwaveDuration << " months." << std::endl;
    }
    if (inColdSnap) {
        std::cout << "Ongoing cold snap with a duration of " << coldSnapDuration << " months." << std::endl;
    }

    std::cout << "Total Heatwaves: " << heatwaveCount << std::endl;
    std::cout << "Total Cold Snaps: " << coldSnapCount << std::endl;
}

int main() {
    const std::string datasetFilename = "Global.csv";
    std::vector<TemperatureData> temperatureData;

    readDataset(datasetFilename, temperatureData);
    int consecutiveMonthsThreshold = 3;

    analyzeEventFrequencyDuration(temperatureData, consecutiveMonthsThreshold);

    return 0;
}
