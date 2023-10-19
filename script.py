

l = []
with open('data.csv', 'r') as file:
    data = file.read()
    for line in data.splitlines():
        a = [float(x) for x in line.split(',')]
        l.append(a)

l1 = []
with open('output.csv', 'r') as file:
    data = file.read()
    for line in data.splitlines():
        a = [float(x) for x in line.split(',')]
        l1.append(a)

if len(l) == 0:
    l = l1
    with open('data.csv', 'w') as file:
        for i in l:
            a = []
            a.append(str(i[0]))
            for j in range(1,len(i)):
                a.append(str(i[j]))
            file.write(','.join(a) + '\n')
else:
    for i in range(len(l)):
        for j in range(1,len(l[i])): 
            l[i][j] += l1[i][j]
            
    with open('data.csv', 'w') as file:
        for i in l:
            a = []
            a.append(str(i[0]))
            for j in range(1,len(i)):
                a.append(str(i[j]))
            file.write(','.join(a) + '\n')