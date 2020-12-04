# python3 script to convert gameids of empty(!) grids into text
import sys

if len(sys.argv) != 2:
	print("usage %s gameid" % sys.argv[0])
	exit(1)

size, data = sys.argv[1].split(":")
m, n = map(int, size.split("x"))
grid, i = ['.'] * n*m, 0
trees, *values = data.split(',')
for c in trees:
	if c >= 'a' and c <= 'z':
		i += min(25, ord(c) - ord('a') + 1)
	if c != 'z' and i < n*m:
		grid[i] = 'T'
		i += 1

print(n, m)
for i in range(n):
	print("".join(grid[i*m + j] for j in range(m)) + " " + values[m + i])
print(" ".join(values[:m]))
