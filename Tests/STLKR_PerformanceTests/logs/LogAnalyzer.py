import argparse
import os
import pprint
import statistics
from datetime import datetime

import matplotlib.pyplot as plt
import numpy as np



def parse_log_content(content):
    data = {
        "Parameters": {},
        "Single Observation Timers": {},
        "Single Observation Data": {}
    }
    lines = content.split('\n')
    line_iter = iter(lines)
    current_region = None
    for line in line_iter:
        line = line.strip()
        if line.startswith('#Region'):
            current_region = line.split(': ')[1]
        elif line.startswith('Parameter'):
            entry_name = line.split(':')[1].strip()
            entry_value = next(line_iter).strip()
            data["Parameters"][entry_name] = entry_value
        elif line.startswith('LogEntry') or line.startswith('Timer'):
            entry_type, entry_name = line.split(':', 1)
            entry_name = entry_name.strip()
            entry_value = float(next(line_iter).strip())
            if current_region == 'Single Observation Timers':
                data["Single Observation Timers"][entry_name] = entry_value
            elif current_region == 'Single Observation Data':
                data["Single Observation Data"][entry_name] = entry_value
    return data


def parse_log_file(file_path):
    """
    Reads and parses a log file at the given path.
    """
    with open(file_path, 'r') as file:
        content = file.read()
    return parse_log_content(content)


def handle_multiple_files(directory_path):
    """
    Processes all CSV files within the specified directory, aggregating data.
    Checks for the presence of timers or data observations and handles accordingly.
    Parameters are assumed to be consistent across files and are captured only from the first file.
    """
    aggregated_data = {
        "Parameters": {},
        "Single Observation Timers": {},
        "Single Observation Data": {}
    }
    parameters_captured = False  # Flag to check if parameters have already been captured

    for file_name in os.listdir(directory_path):
        if file_name.endswith('.csv'):
            file_path = os.path.join(directory_path, file_name)
            print(f"Opening file {file_name}...")
            data = parse_log_file(file_path)
            print(f"File {file_name} read successfully.")

            # Handle parameters: Capture only once
            if not parameters_captured:
                for entry, value in data["Parameters"].items():
                    aggregated_data["Parameters"][entry] = value
                parameters_captured = True  # Set flag after capturing parameters

            # Handle other data
            for key in ['Single Observation Timers', 'Single Observation Data']:
                for entry, value in data[key].items():
                    if entry not in aggregated_data[key]:
                        aggregated_data[key][entry] = []
                    aggregated_data[key][entry].append(value)

    print("All files processed successfully.")
    return aggregated_data



def compute_stats(values, precision=".4e"):
    stats = {}
    stats['mean'] = format(statistics.mean(values), precision)
    stats['median'] = format(statistics.median(values), precision)
    stats['std_dev'] = format(statistics.stdev(values) if len(values) > 1 else 0.0, precision)
    stats['min'] = format(min(values), precision)
    stats['max'] = format(max(values), precision)
    stats['range'] = format(max(values) - min(values), precision)
    stats['variance'] = format(statistics.variance(values) if len(values) > 1 else 0.0, precision)
    stats['coeff_variation'] = format((statistics.stdev(values) / statistics.mean(values)) if statistics.mean(values) != 0 else 0, precision)
    stats['95th_percentile'] = format(np.percentile(values, 95), precision)
    return stats


def analyze_data(data):
    results = {}
    categories_to_analyze = ['Single Observation Timers', 'Single Observation Data']
    for category in categories_to_analyze:
        entries = data.get(category, {})
        results[category] = {name: compute_stats(values) for name, values in entries.items()}
    return results


def find_maximum_minimum(data):
    """
    Finds the maximum and minimum statistical values across timers and data observations.
    Handles string representations of scientific notation correctly.
    """
    max_min_stats = {
        'max_mean': {'value': '-inf', 'name': None},
        'min_mean': {'value': 'inf', 'name': None},
        'max_median': {'value': '-inf', 'name': None},
        'min_median': {'value': 'inf', 'name': None},
        'max_std_dev': {'value': '-inf', 'name': None},
        'min_std_dev': {'value': 'inf', 'name': None}
    }

    categories_to_analyze = ['Single Observation Timers', 'Single Observation Data']
    for category in categories_to_analyze:
        for name, values in data.get(category, {}).items():
            if float(values['mean']) > float(max_min_stats['max_mean']['value']):
                max_min_stats['max_mean'].update({'value': values['mean'], 'name': name})
            if float(values['mean']) < float(max_min_stats['min_mean']['value']):
                max_min_stats['min_mean'].update({'value': values['mean'], 'name': name})
            if float(values['median']) > float(max_min_stats['max_median']['value']):
                max_min_stats['max_median'].update({'value': values['median'], 'name': name})
            if float(values['median']) < float(max_min_stats['min_median']['value']):
                max_min_stats['min_median'].update({'value': values['median'], 'name': name})
            if float(values['std_dev']) > float(max_min_stats['max_std_dev']['value']):
                max_min_stats['max_std_dev'].update({'value': values['std_dev'], 'name': name})
            if float(values['std_dev']) < float(max_min_stats['min_std_dev']['value']):
                max_min_stats['min_std_dev'].update({'value': values['std_dev'], 'name': name})
    return max_min_stats


def plot_data(data, category, parameters):
    entries = data.get(category, {})
    names = list(entries.keys())
    means = [float(entries[name]['mean']) for name in names]
    medians = [float(entries[name]['median']) for name in names]
    std_devs = [float(entries[name]['std_dev']) for name in names]
    max_vals = [entries[name]['max'] for name in names]

    x = np.arange(len(names))  # the label locations
    width = 0.25  # the width of the bars

    fig, ax = plt.subplots(figsize=(20, 10))
    rects1 = ax.bar(x - width, means, width, label='Mean', color='lightblue')
    rects2 = ax.bar(x, medians, width, label='Median', color='lightgreen')
    rects3 = ax.bar(x + width, std_devs, width, label='Std Dev', color='salmon')

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_xlabel('Entries')
    ax.set_ylabel('Values')
    ax.set_title(f'Statistics for {category}: {parameters}')
    ax.set_xticks(x)
    ax.set_xticklabels(names, rotation=45, ha="right", fontsize=9)
    ax.legend()

    def autolabel(rects, max_vals):
        """Attach a text label above each bar in rect, displaying its height in scientific notation."""
        for rect, max_val in zip(rects, max_vals):
            height = rect.get_height()
            formatted_max_val = f"{float(max_val):.4e}"  # Convert max value to scientific notation with precision 1e-4
            ax.annotate(formatted_max_val,
                        xy=(rect.get_x() + rect.get_width() / 2, height),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom', fontsize=8)

    def highlight_extremes(rects, values):
        """Highlight maximum and minimum values with arrows."""
        max_value = max(values)
        min_value = min(values)
        for rect, value in zip(rects, values):
            if value == max_value:
                ax.annotate('Max', xy=(rect.get_x() + rect.get_width() / 2, value),
                            xytext=(0, -15), textcoords="offset points",
                            arrowprops=dict(facecolor='red', shrink=0.05),
                            ha='center', va='top')
            elif value == min_value:
                ax.annotate('Min', xy=(rect.get_x() + rect.get_width() / 2, value),
                            xytext=(0, -15), textcoords="offset points",
                            arrowprops=dict(facecolor='yellow', shrink=0.005),
                            ha='center', va='top', fontsize=8)

    # Call autolabel for all sets of bars
    autolabel(rects1, max_vals)
    autolabel(rects2, max_vals)
    autolabel(rects3, max_vals)

    # Highlight extremes for each set
    highlight_extremes(rects1, means)
    highlight_extremes(rects2, medians)
    highlight_extremes(rects3, std_devs)

    fig.tight_layout()

    path = os.path.join(os.getcwd(), 'plots')
    if not os.path.exists(path):
        os.makedirs(path)
    plt.savefig(os.path.join(path, f'{category}_{datetime.now().strftime("%Y-%m-%d")}.png'))
    plt.show()


def main():
    parser = argparse.ArgumentParser(description="Process log CSV files from a directory.")
    parser.add_argument("directory", type=str, help="Directory containing the CSV files.")
    args = parser.parse_args()

    # Process files in the specified directory
    aggregated_data = handle_multiple_files(args.directory)
    analysis_results = analyze_data(aggregated_data)
    # Pretty print the results
    pprinter = pprint.PrettyPrinter(indent=4)
    pprinter.pprint(analysis_results)
    # # Find the maximum and minimum values
    max_min_results = find_maximum_minimum(analysis_results)
    pprinter.pprint(max_min_results)
    for category in ['Single Observation Timers']:
    #for category in ['Single Observation Timers', 'Single Observation Data']:
        plot_data(analysis_results, category, aggregated_data["Parameters"])


if __name__ == "__main__":
    main()
