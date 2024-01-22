#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>


// Function to check if the string can be converted to double
bool isDouble(const std::string& str) {
    std::istringstream iss(str);
    double dbl;
    iss >> std::noskipws >> dbl; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail();
}

int main() {
    std::string filename = "GISTEMP_global_dataset.csv";
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::vector<std::string>> monthly_deviation;

    // Skip header line to avoid reading column titles
    std::getline(file, line);

    // Read file line by line
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> deviations;

        // Read year which is the first token before the comma
        std::getline(iss, token, ',');
        deviations.push_back(token);

        // Read each subsequent token after the year
        while (std::getline(iss, token, ',')) {
            deviations.push_back(token);
        }

        monthly_deviation.push_back(deviations);
    }

    file.close();

    // Initialize previous_record with the same shape as monthly_deviation
    std::vector<std::vector<int>> previous_record(monthly_deviation.size(), std::vector<int>(13, -1));

    // Iterate over each month and year
    for (size_t year = 1; year < monthly_deviation.size(); ++year) {
        for (size_t month = 1; month <= 12; ++month) {
            double current_dev = isDouble(monthly_deviation[year][month]) ? std::stod(monthly_deviation[year][month]) : std::numeric_limits<double>::lowest();
            double max_dev = std::numeric_limits<double>::lowest();
            int max_dev_year = -1;

            // Find the maximum deviation for all previous years
            for (size_t prev_year = 1; prev_year < year; ++prev_year) {
                double prev_dev = isDouble(monthly_deviation[prev_year][month]) ? std::stod(monthly_deviation[prev_year][month]) : std::numeric_limits<double>::lowest();
                if (prev_dev > max_dev) {
                    max_dev = prev_dev;
                    max_dev_year = std::stoi(monthly_deviation[prev_year][0]);
                }
            }

            // Update previous_record
            if (current_dev >= max_dev) {
                previous_record[year][month] = std::stoi(monthly_deviation[year][0]);
            } else {
                previous_record[year][month] = max_dev_year;
            }
        }
    }

    // Initialize gapyears and gapsizes vectors
    std::vector<std::vector<int>> gapyears(12);
    std::vector<std::vector<int>> gapsizes(12);

    // Iterate over each month to find gaps between records
    for (size_t month = 1; month <= 12; ++month) {
        int last_record_year = -1;

        for (size_t year = 1; year < monthly_deviation.size(); ++year) {
            if (previous_record[year][month] == std::stoi(monthly_deviation[year][0])) {
                // Record the start of a new gap
                if (last_record_year != -1) {
                    gapyears[month - 1].push_back(last_record_year);
                    gapsizes[month - 1].push_back(std::stoi(monthly_deviation[year][0]) - last_record_year);
                }
                last_record_year = std::stoi(monthly_deviation[year][0]);
            }
        }
    }

    // Printing the results for verification
    for (size_t month = 1; month <= 12; ++month) {
        std::cout << "Month: " << month << std::endl;
        for (size_t i = 0; i < gapyears[month - 1].size(); ++i) {
            std::cout << "Gap Start Year: " << gapyears[month - 1][i]
                      << ", Gap Size: " << gapsizes[month - 1][i] << " years" << std::endl;
        }
    }

    return 0;
}
