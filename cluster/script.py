import ray
import os
import json


@ray.remote(scheduling_strategy="SPREAD")
def attack(filename, key):
	print(f"Attacking {filename} with key {key}")
	# Execute attack_cluster
	wd = os.getcwd()
	result = os.popen(f"{wd}/build/attack {filename} {key}")
	# Parse result end extract values after "GKOV estimate: " and "Hist estimate: "
	result = result.read().replace("\n", "")
	if "GKOV estimate: " not in result:
		print(f"Error in attack: {result}")
	result = result.split("GKOV estimate: ")[1]
	result = result.split("Hist estimate: ")
	result = [float(item) for item in result]
	return key, result[0], result[1]


if __name__ == '__main__':
	ray.init(address="localhost:6379")
	for index in range(0, 15):
		print(f"Starting attack with {10*(2**index)} traces")
		filename = f"data/traces/{10*(2**index)}_traces.h5"
		results = [attack.remote(filename, i) for i in range(0, 256)]
		results = ray.get(results)
		json_results = {}
		for item in results:
			json_results[item[0]] = {"GKOV": item[1], "Hist": item[2]}
		with open(f"{os.getcwd()}/data/results/{10*(2**index)}_traces.json", "w") as file:
			json.dump(json_results, file, indent=4)
		print(f"Finished attack with {10*(2**index)} traces")