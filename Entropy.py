import math

f = open("test.jpg", "rb")

# 用于存储统计结果的字典
stats = {}

while(1):
    char = str(f.read(1))[2:-1]

    # 判断文件结束
    if (char == ""):
        break

    if (stats.__contains__(char)):
        stats[char] = stats[char] + 1
    else:
        stats[char] = 1

# 返回所有可遍历的键值对的元组的列表,按值排序
itemlist = sorted(stats.items(), key = lambda x: x[1], reverse = True)

# 总字符数
total = 0
for i in itemlist:
    total = total + i[1]

print("总字符数: %d" % total, end = "\n\n")

freqlist = []
for i in itemlist:
    print("%-4s    次数: %d" % (i[0], i[1]), end = "    ")
    freq = i[1] / total
    freqlist.append(freq)
    print("频率: %f" % freq)

# 计算信息熵
entropy = 0
for i in freqlist:
    entropy = entropy - (i * math.log(i, 2))

print()
print("信息熵: %f" % entropy)

f.close()