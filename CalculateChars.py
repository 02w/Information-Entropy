import re
import math

# rb 表示读二进制数据
f = open("test.jpg", "rb")
content = f.readlines()

# 匹配 \xhh 格式的正则
hexpattern = r'\\x[0-9a-f]{2}'

special = ["\\n", "\\t", "\\a", "\\b", "\\f", "\\r", "\\v","\\\"","\\'","\\0","\\\\"]

# 用于存储统计结果的字典
stats = {}

for line in content:

    # str()函数处理 b'xxx' 时多余 b 和 ' 
    string = str(line)[2:-1]

    # 处理 \xhh 格式字符
    l = re.findall(hexpattern, string)
    for hexchar in l:
        if (stats.__contains__(hexchar)):
            stats[hexchar] = stats[hexchar] + 1
        else:
            stats[hexchar] = 1

    # 处理转义字符
    new = str(re.sub(hexpattern, '', string))
    for c in special:
        if c in new:
            stats[c] = new.count(c)
        new = new.replace(c, "")

    # 处理普通字符
    for char in new:
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