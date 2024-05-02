import argparse
import os
import pprint
import statistics


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
    """
    aggregated_data = {
        "Parameters": {},
        "Single Observation Timers": {},
        "Single Observation Data": {}
    }

    for file_name in os.listdir(directory_path):
        if file_name.endswith('.csv'):
            file_path = os.path.join(directory_path, file_name)
            print(f"Opening file {file_name}...")
            data = parse_log_file(file_path)
            print(f"File {file_name} read successfully.")

            for key in ['Single Observation Timers', 'Single Observation Data']:
                for entry, value in data[key].items():
                    if entry not in aggregated_data[key]:
                        aggregated_data[key][entry] = []
                    aggregated_data[key][entry].append(value)

    print("All files processed successfully.")
    return aggregated_data


def analyze_data(data):
    """
    Analyzes data, computing statistical metrics, ensuring handling if regions are missing.
    """
    results = {}
    categories_to_analyze = ['Single Observation Timers', 'Single Observation Data']
    for category in categories_to_analyze:
        entries = data.get(category, {})
        results[category] = {}
        for name, values in entries.items():
            mean_val = statistics.mean(values)
            median_val = statistics.median(values)
            std_dev_val = statistics.stdev(values) if len(values) > 1 else 0.0
            min_val = min(values)
            max_val = max(values)

            results[category][name] = {
                'mean': mean_val,
                'median': median_val,
                'std_dev': std_dev_val,
                'min': min_val,
                'max': max_val
            }
    return results


def find_maximum_minimum(data):
    """
    Finds the maximum and minimum statistical values across timers and data observations.
    """
    max_min_stats = {
        'max_mean': {'value': float('-inf'), 'name': None},
        'min_mean': {'value': float('inf'), 'name': None},
        'max_median': {'value': float('-inf'), 'name': None},
        'min_median': {'value': float('inf'), 'name': None},
        'max_std_dev': {'value': float('-inf'), 'name': None},
        'min_std_dev': {'value': float('inf'), 'name': None}
    }

    categories_to_analyze = ['Single Observation Timers', 'Single Observation Data']
    for category in categories_to_analyze:
        for name, values in data.get(category, {}).items():
            if values['mean'] > max_min_stats['max_mean']['value']:
                max_min_stats['max_mean'].update({'value': values['mean'], 'name': name})
            if values['mean'] < max_min_stats['min_mean']['value']:
                max_min_stats['min_mean'].update({'value': values['mean'], 'name': name})
            if values['median'] > max_min_stats['max_median']['value']:
                max_min_stats['max_median'].update({'value': values['median'], 'name': name})
            if values['median'] < max_min_stats['min_median']['value']:
                max_min_stats['min_median'].update({'value': values['median'], 'name': name})
            if values['std_dev'] > max_min_stats['max_std_dev']['value']:
                max_min_stats['max_std_dev'].update({'value': values['std_dev'], 'name': name})
            if values['std_dev'] < max_min_stats['min_std_dev']['value']:
                max_min_stats['min_std_dev'].update({'value': values['std_dev'], 'name': name})
    return max_min_stats


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


if __name__ == "__main__":
    main()
