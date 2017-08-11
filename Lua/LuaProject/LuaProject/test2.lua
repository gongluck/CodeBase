--test2.lua--

print("test2.lua")

s = {}
for i = 1, 10, 1 do
	s[i] = i
end

print("for k, v in ipairs(s)")
for k, v in ipairs(s) do
	print(k .. "---" .. v)
end

print("for k in pairs(s)") 
for k,v in pairs(s) do
	if k ~= nil
	then
		print(k .. "---" .. v)
	end
end

print("table.unpack(s)")
print(table.unpack(s)) --输出 1 2 3 4 5 6 7 8 9 10

print("table.unpack(s,2)")
print(table.unpack(s,2)) --输出 2 3 4 5 6 7 8 9 10

print("r = table.unpack(s)")
r = table.unpack(s)
print(r) --输出 1

print("r = table.unpack(s,2)")
r = table.unpack(s,2)
print(r) --输出 2

print("select(\"#\",table.unpack(s,2))")
print(select("#",table.unpack(s,2))) --输出 9

print("select(5,table.unpack(s,2))")
print(select(5,table.unpack(s,2))) --输出 6 7 8 9 10

function fun1()--fun1()返回的是匿名函数
    local i = 0
    return function ()      -- 匿名函数，返回“真正的结果”
        i = i + 1
        return i
    end
end
fun = fun1();
print(fun())     --> 1
print(fun())     --> 2

str1 = "str1: hello lua !"
str2 = "str2: hello world !"
print(table.concat({str1,str2},"\n"))
