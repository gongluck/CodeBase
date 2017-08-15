--test.lua--

dofile("test2.lua")

io.input("test2.lua")
print(io.read("*all"))
io.input():close()

f = io.open("test2.lua","r")
print(f:read(50,"*line"))
f:close();

t = os.time()
print(t)
print(os.date("%Y-%m-%d %H:%M:%S"))