import argparse
import os
import pprint
import statistics
from datetime import datetime

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import ScalarFormatter


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
            try:
                entry_value = float(entry_value) if entry_value.replace('.', '', 1).isdigit() else entry_value
            except ValueError:
                print(f"Warning: Could not convert value for {entry_name} to float.")
            data["Parameters"][entry_name] = entry_value
        elif line.startswith('LogEntry') or line.startswith('Timer'):
            entry_type, entry_name = line.split(':', 1)
            entry_name = entry_name.strip()
            try:
                entry_value = float(next(line_iter).strip())
                if current_region == 'Single Observation Timers':
                    data["Single Observation Timers"][entry_name] = entry_value
                elif current_region == 'Single Observation Data':
                    data["Single Observation Data"][entry_name] = entry_value
            except ValueError:
                print(f"Warning: Could not convert value for {entry_name} to float.")
    return data


def parse_log_file(file_path):
    """
    Reads and parses a log file at the given path.
    """
    try:
        with open(file_path, 'r') as file:
            content = file.read()
        return parse_log_content(content)
    except IOError as e:
        print(f"Error reading file {file_path}: {e}")
        return None


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
            if data:
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
            else:
                print(f"Failed to read data from {file_name}")

    print("All files processed successfully.")
    return aggregated_data


def validate_numeric_data(data):
    """
    Ensure that all entries in the data dictionary are numeric where applicable.
    """
    for category in ['Single Observation Timers', 'Single Observation Data']:
        for entry, values in data[category].items():
            if not isinstance(values, list):
                values = [values]
            numeric_values = []
            for value in values:
                try:
                    numeric_values.append(float(value))
                except ValueError:
                    print(f"Warning: Non-numeric value found in {entry}: {value}")
            data[category][entry] = numeric_values
    return data


def compute_stats(values, precision=".4e"):
    if not values:
        return {
            'mean': 'NaN',
            'median': 'NaN',
            'std_dev': 'NaN',
            'min': 'NaN',
            'max': 'NaN',
            'range': 'NaN',
            'variance': 'NaN',
            'coeff_variation': 'NaN',
            '95th_percentile': 'NaN'
        }
    stats = {}
    stats['mean'] = format(statistics.mean(values), precision)
    stats['median'] = format(statistics.median(values), precision)
    stats['std_dev'] = format(statistics.stdev(values) if len(values) > 1 else 0.0, precision)
    stats['min'] = format(min(values), precision)
    stats['max'] = format(max(values), precision)
    stats['range'] = format(max(values) - min(values), precision)
    stats['variance'] = format(statistics.variance(values) if len(values) > 1 else 0.0, precision)
    stats['coeff_variation'] = format(
        (statistics.stdev(values) / statistics.mean(values)) if statistics.mean(values) != 0 else 0, precision)
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


def plot_data(data, category, parameters, save_path):
    entries = data.get(category, {})
    names = list(entries.keys())
    means = [float(entries[name]['mean']) for name in names]
    medians = [float(entries[name]['median']) for name in names]
    std_devs = [float(entries[name]['std_dev']) for name in names]

    x = np.arange(len(names))  # the label locations
    width = 0.25  # the width of the bars

    fig, ax = plt.subplots(figsize=(19, 9))
    rects1 = ax.bar(x - width, means, width, label='Mean', color='lightblue')
    rects2 = ax.bar(x, medians, width, label='Median', color='lightgreen')
    rects3 = ax.bar(x + width, std_devs, width, label='Std Dev', color='salmon')

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_xlabel('Entries')
    ax.set_ylabel('Values')
    ax.set_title(f'Statistics for {category}')
    ax.set_xticks(x)
    ax.set_xticklabels(names, rotation=45, ha="right", fontsize=8)
    ax.legend()

    # Add parameters as a box inside the graph
    param_text = '\n'.join([f'{key}: {value}' for key, value in parameters.items()])
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
    ax.text(1.01, 0.5, param_text, transform=ax.transAxes, fontsize=9,
            verticalalignment='center', bbox=props)

    def autolabel(rects, values):
        """Attach a text label above each bar, displaying its height in scientific notation."""
        for rect, value in zip(rects, values):
            height = rect.get_height()
            formatted_value = f"{value:.4e}"  # Convert value to scientific notation with precision 1e-4
            ax.annotate(formatted_value,
                        xy=(rect.get_x() + rect.get_width() / 2, height),
                        xytext=(10, 0),
                        textcoords="offset points",
                        ha='center', va='bottom', fontsize=7.5, rotation=angle)

    def highlight_extremes(rects, values, label):
        """Highlight minimum and maximum values with arrows and custom text colors."""

        min_value = min(values)
        max_value = max(values)
        for rect, value in zip(rects, values):
            xytext = (0, 50)  # Move the arrow annotation point higher
            arrow_style = 'simple,head_length=0.8,head_width=0.8,tail_width=0.4'
            shrinkB = 30
            if value == min_value:
                ax.annotate('MIN', xy=(rect.get_x() + rect.get_width() / 2, value),
                            xytext=xytext, textcoords="offset points",
                            arrowprops=dict(facecolor='green', edgecolor='green', arrowstyle=arrow_style,
                                            shrinkA=0, shrinkB=shrinkB),
                            ha='center', va='bottom', fontsize=7, color='green')
            if value == max_value:
                ax.annotate('MAX', xy=(rect.get_x() + rect.get_width() / 2, value),
                            xytext=xytext, textcoords="offset points",
                            arrowprops=dict(facecolor='red', edgecolor='red', arrowstyle=arrow_style,
                                            shrinkA=0, shrinkB=shrinkB),
                            ha='center', va='bottom', fontsize=7, color='red')

    # Call autolabel for all sets of bars
    angle = 25
    autolabel(rects1, means)
    autolabel(rects2, medians)
    autolabel(rects3, std_devs)

    # Highlight minimum and maximum values for each set
    highlight_extremes(rects1, means, 'Mean')
    highlight_extremes(rects2, medians, 'Median')
    highlight_extremes(rects3, std_devs, 'Std Dev')

    ax.yaxis.set_major_formatter(ScalarFormatter(useMathText=True))
    ax.yaxis.get_major_formatter().set_scientific(True)
    ax.yaxis.get_major_formatter().set_powerlimits((-4, 4))

    fig.tight_layout()
    ax.set_ylim(0, max(max(means), max(medians), max(std_devs)) * 1.2)
    # Generate a timestamp for the filename
    timestamp = datetime.now().strftime("%d_%m_%y_%H_%M_%S")
    #plt.savefig(os.path.join(save_path, f'{category}_{timestamp}.png'))
    plt.show()


def compute_running_statistics(values):
    running_mean = []
    running_median = []
    running_std_dev = []
    cumulative_values = []

    for value in values:
        cumulative_values.append(value)
        running_mean.append(statistics.mean(cumulative_values))
        running_median.append(statistics.median(cumulative_values))
        running_std_dev.append(statistics.stdev(cumulative_values) if len(cumulative_values) > 1 else 0.0)

    return running_mean, running_median, running_std_dev


def plot_running_statistics(data, category, save_path):
    entries = data.get(category, {})
    symbols = ['o', '^', 's']
    stats_labels = ['Mean', 'Median', 'Std Dev']
    colors = plt.cm.get_cmap('tab10', len(entries))

    plt.figure(figsize=(20, 10))

    for i, (entry_name, values) in enumerate(entries.items()):
        running_mean, running_median, running_std_dev = compute_running_statistics(values)

        # Plot only every 100th point
        step = 10
        plt.plot(running_mean[::step], label=f'{entry_name} Mean', marker=symbols[0], linestyle='-', color=colors(i),
                 markerfacecolor='none')
        plt.plot(running_median[::step], label=f'{entry_name} Median', marker=symbols[1], linestyle='--',
                 color=colors(i), markerfacecolor='none')
        plt.plot(running_std_dev[::step], label=f'{entry_name} Std Dev', marker=symbols[2], linestyle=':',
                 color=colors(i), markerfacecolor='none')

    plt.xlabel('Run Number')
    plt.ylabel('Value')
    plt.title(f'Running Statistics for {category}')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    timestamp = datetime.now().strftime("%d_%m_%y_%H_%M_%S")
    plt.savefig(os.path.join(save_path, f'{category}_running_stats_{timestamp}.png'))
    plt.show()


def main():
    parser = argparse.ArgumentParser(description="Process log CSV files from a directory.")
    parser.add_argument("directory", type=str, help="Directory containing the CSV files.")
    args = parser.parse_args()

    # Process files in the specified directory
    aggregated_data = handle_multiple_files(args.directory)

    # Validate the data to ensure all values are numeric where applicable
    aggregated_data = validate_numeric_data(aggregated_data)

    analysis_results = analyze_data(aggregated_data)
    # Pretty print the results
    pprinter = pprint.PrettyPrinter(indent=4)
    pprinter.pprint(analysis_results)
    # Find the maximum and minimum values
    max_min_results = find_maximum_minimum(analysis_results)
    pprinter.pprint(max_min_results)

    # Create directory for plots if it does not exist
    plots_path = os.path.join(args.directory, 'plots')
    if not os.path.exists(plots_path):
        os.makedirs(plots_path)

    # Compute and plot running statistics for diagnostics
    #for category in ['Single Observation Timers']:
    #plot_running_statistics(aggregated_data, category, plots_path)

    for category in ['Single Observation Timers']:
        plot_data(analysis_results, category, aggregated_data["Parameters"], plots_path)


if __name__ == "__main__":
    main()
