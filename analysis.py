import h5py
import os
import json
import pandas as pd
import matplotlib.pyplot as plt


def plot_results(data_frame):
    plt.figure(figsize=(10, 5))
    plt.scatter(data_frame["samples"], data_frame["gkov_mi"], marker="o", color="red")
    plt.plot(data_frame["samples"], data_frame["gkov_mi"], label="GKOV")
    plt.xlabel("Number of samples")
    plt.ylabel("Mutual Information")
    plt.title("Mutual Information vs Number of Samples")
    plt.legend()
    plt.grid()
    plt.savefig(f"{os.getcwd()}/data/plots/gkov_mi.png")


def create_dataframe():
    data_frame = pd.DataFrame(columns=["samples", "key", "gkov", "gkov_mi", "hist", "hist_mi"])
    results = [item for item in os.listdir(f"{os.getcwd()}/data/results") if item.endswith(".json")]
    for result in results:
        file_name_without_extension = result.split(".json")[0]
        with h5py.File(f"{os.getcwd()}/data/traces/{file_name_without_extension}.h5", "r") as file:
            key = int(file["traces"].attrs["secret_key"][0])
        with open(f"{os.getcwd()}/data/results/{result}", "r") as file:
            data = json.load(file)
        gkov_key = max(data, key=lambda x: data[x]["GKOV"])
        gkov_mi = data[gkov_key]["GKOV"]
        hist_key = max(data, key=lambda x: data[x]["Hist"])
        hist_mi = data[hist_key]["Hist"]
        data_frame = pd.concat([data_frame, pd.DataFrame({
            "samples": [file_name_without_extension.split("_")[0]],
            "key": [key],
            "gkov": [gkov_key],
            "gkov_mi": [gkov_mi],
            "hist": [hist_key],
            "hist_mi": [hist_mi]
        })])
    return data_frame


if __name__ == '__main__':
    data_frame = create_dataframe()
    data_frame["samples"] = data_frame["samples"].astype(int)
    data_frame = data_frame.sort_values(by=["samples"])
    data_frame["gkov_mi"] = data_frame["gkov_mi"].astype(float)
    # print(data_frame.to_latex(index=False))
    plot_results(data_frame)

