import pandas as pd


def main():
    data = pd.read_csv("./data/games.txt", sep="\t")

    null_values = data[data["FG_PCT_home"].isna()]
    print(f'Missing values: {len(null_values)}\n')

    cleaned_data = data.dropna()
    print(cleaned_data["FG_PCT_home"].describe())

    exp2_query = cleaned_data
    exp3_query = cleaned_data[cleaned_data["FG_PCT_home"] == 0.5]
    exp4_query = cleaned_data[(cleaned_data["FG_PCT_home"] >= 0.6) & (cleaned_data["FG_PCT_home"] <= 1)]
    exp5_query = cleaned_data[cleaned_data["FG_PCT_home"] > 0.35]

    print(f'\nExperiment 2: ')
    print(f'Number of records retrieved: {len(exp2_query)}')

    print(f'\nExperiment 3: ')
    print(f'Number of records retrieved: {len(exp3_query)}')
    print(f'Average for FG3_PCT_home when 0.5 : {exp3_query["FG3_PCT_home"].mean()}')

    print(f'\nExperiment 4: ')
    print(f'Number of records retrieved: {len(exp4_query)}')
    print(f'Average for FG3_PCT_home when 0.6 to 1 : {exp4_query["FG3_PCT_home"].mean()}')

    print(f'\nExperiment 5: ')
    print(f'Number of records remaining: {len(exp5_query)}')

    print()

if __name__ == "__main__":
    main()
