import pandas as pd


def main():
    data = pd.read_csv("./data/games.txt", sep="\t")
    print(data["FG_PCT_home"].describe())


if __name__ == "__main__":
    main()
