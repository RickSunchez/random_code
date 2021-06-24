class Eiler_Finder:
	def __init__(self, _path_count, _brige_num, _map):
		self.pc = _path_count
		self.bn = _map[_brige_num-1]
		self.m  = _map
		self.current_path = ""

	def findPath(self, a,b, _map, path=""):
		if a == b:
			self.current_path = "%s%d" % (path,b)
			return path

		path += "%d-" % a

		for i in range( len(_map) ):
			if a in _map[i]:
				_a = _map[i][(_map[i].index(a)+1)%2]
				self.findPath(_a, b, list(filter(lambda p: p != _map[i], _map)), path)

	def calculate(self):
		full_path = ""
		for i in range(1, self.pc+1):
			j = i+1 if i+1 < self.pc+1 else 1
			self.findPath(i, j, self.m)
			full_path += self.current_path + "-"

		fwd = "-".join(list(map(str, self.bn)))
		bwd = "-".join(list(map(str, self.bn[::-1])))

		counter = 0
		counter += full_path.count(fwd)
		counter += full_path.count(bwd)

		return counter

n, b = map(int, input().split(" "))
tracks = []

for i in range(n-1):
	tracks.append( list(map(int, input().split(" "))) )

print(tracks)

ef = Eiler_Finder(n, b, tracks)

print(ef.calculate())