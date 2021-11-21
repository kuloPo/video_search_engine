import json
from collections import defaultdict

if __name__ == "__main__":
	f = open("../data/interval.json")
	json_obj = json.loads(f.read())
	f.close()
	
	invert_index = defaultdict(list)
	for video in json_obj:
		filename = video["filename"]
		for interval in video["interval"]:
			if (filename not in invert_index[interval]):
				invert_index[interval].append(filename)

	str = json.dumps(invert_index)
	f = open("../data/invert_index.json", "w")
	f.write(str)
