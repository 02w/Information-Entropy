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

# 返回所有可遍历的键值对的元组的列表
itemlist = stats.items()

# 总字符数
total = 0
for i in itemlist:
    print(i[0], end=":")
    print(i[1])
    total = total + i[1]

# 计算信息熵
freq = []
for i in itemlist:
    freq.append(i[1] / total)

entropy = 0
for i in freq:
    entropy = entropy - (i * math.log(i))

print("信息熵", end=":")
print(entropy)

f.close()