import numpy as np
import matplotlib.pyplot as plt

def get_data(filename):
    with open(filename) as file:
        lines = file.readlines()
        
    row = lines[0].rstrip()
    column = lines[1].rstrip()
    dim = lines[2].rstrip()
    data = lines[3].rstrip()

    data = data.split(",")
    data.pop()
    data = [float(string) for string in data]
    data = np.array(data)
    data = data.reshape((int(row), int(column), int(dim)))
    return data



def show_map(m, name, itera):
    fig = plt.figure(figsize=(6,10))
    plt.imshow(m.sum(axis=2))
    plt.title(itera)
    #plt.show()
    plt.savefig(name,bbox_inches='tight')


show_map(get_data("net_before"), "net_before.png", "before training")
show_map(get_data("net_after"), "net_after.png", "after training")



with open("map") as file:
    lines = file.readlines()

row = lines[0].rstrip()
column = lines[1].rstrip()
dim = lines[2].rstrip()
data = lines[3].rstrip()

data = data.split(",")
data.pop()
data = [int(string) for string in data]
data = np.array(data)
data = data.reshape((int(row), int(column)))

fig = plt.figure(figsize=(6,10))
plt.imshow(data)
plt.colorbar(orientation="horizontal")
plt.title("Map")
#plt.show()
plt.savefig("Map",bbox_inches='tight')