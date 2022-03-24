import string
# https://stackoverflow.com/questions/21184673/algorithm-to-compress-a-lot-of-small-strings

if __name__ == "__main__":
    lines = []
    with open('/home/jacob/ACS_Encode/Small-Size-Column.txt', 'r') as f:
        lines = f.readlines()

    print(len(lines))
    alpha = list(string.ascii_lowercase)
    alpha.append('\n')
    print(alpha)
    freq = {}
    lens = {}

    for c in alpha:
        freq[c] = 0

    for num in range (0,20):
        lens[num] = 0

    for line in lines:
        lens[len(line)] = lens[len(line)] + 1
        for c in line:
            freq[c] = freq[c] + 1

    total_file_sz = 0
    for it in freq.keys():
        total_file_sz += 5 * freq[it]

    print(total_file_sz // 8)
    print(freq)
    print(lens)

