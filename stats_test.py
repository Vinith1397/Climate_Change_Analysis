import pandas as pd
import numpy as np
import statsmodels.api as sm
from statsmodels.tsa.stattools import adfuller
import matplotlib.pyplot as plt

def linear_regression(x, y):
    x = sm.add_constant(x)  # adding a constant
    model = sm.OLS(y, x).fit()
    return model.params[0], model.params[1], model.rsquared  # intercept, slope, R-squared

def adf_test(series):
    result = adfuller(series.dropna(), autolag='AIC')  # ADF test on non-NaN data
    labels = ['ADF test statistic', 'p-value', '# lags used', '# observations']
    out = pd.Series(result[0:4], index=labels)

    for key, val in result[4].items():
        out[f'critical value ({key})'] = val

    return out

# Load data
filename = 'GISTEMP_global_dataset.csv'
df = pd.read_csv(filename, header=1, na_values='***')  # Convert '***' to NaN

# Set the first column as the index
df.set_index('Year', inplace=True)

# Assuming the CSV has columns for each month following the year
months = df.columns[:12]  # Considering only the first 12 columns for monthly data

# Create a figure with subplots for each month
fig, axes = plt.subplots(nrows=4, ncols=3, figsize=(12, 8))
fig.subplots_adjust(hspace=0.5)
axes = axes.flatten()

adf_results_list = []  # Store ADF results for each month

for i, month in enumerate(months):
    series = df[month].dropna()
    
    # Check if the series is empty
    if series.empty:
        print(f"No data available for {month}")
        continue

    # Linear Regression
    intercept, slope, r_squared = linear_regression(np.array(df.index[:len(series)]).astype(float), series.astype(float))

    # Augmented Dickey-Fuller test
    adf_results = adf_test(series)
    adf_results_list.append(adf_results)

    # Plot the time series data
    axes[i].plot(df.index[:len(series)], series, label='Temperature Data')
    
    # Plot the linear regression line
    predicted = intercept + slope * np.array(df.index[:len(series)]).astype(float)
    axes[i].plot(df.index[:len(series)], predicted, label='Linear Regression', color='red')
    
    axes[i].set_title(month)
    axes[i].set_xlabel('Year')
    axes[i].set_ylabel('Temperature')
    axes[i].legend(loc='upper left', bbox_to_anchor=(1, 1))

plt.tight_layout()
plt.show()

# Create a new figure for ADF test results
adf_fig, adf_axes = plt.subplots(nrows=4, ncols=3, figsize=(12, 8))
adf_fig.subplots_adjust(hspace=0.5)
adf_axes = adf_axes.flatten()

# Display ADF test results in separate figure
for i, adf_results in enumerate(adf_results_list):
    adf_table_data = adf_results.values.reshape(-1, 1)  # Reshape to 2D array
    table = adf_axes[i].table(cellText=adf_table_data,
                              colLabels=['Value'],
                              rowLabels=adf_results.index,
                              cellLoc='center', rowLoc='center',
                              bbox=[0, 0, 1, 1])
    adf_axes[i].axis('off')  # Turn off axis for table subplot
    adf_axes[i].set_title(months[i])  # Set title for each subplot

adf_fig.tight_layout()
plt.show()
