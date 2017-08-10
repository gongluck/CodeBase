--Lua笔记--
--0.Lua开篇--
--http://www.cnblogs.com/stephen-liu74/archive/2012/06/11/2403210.html--

--1.Lua基础知识--
--[[
通过dofile()方法引用其他Lua文件中的函数
在Lua中全局变量不需要声明，直接赋值即可。如果直接访问未初始化的全局变量，Lua也不会报错，直接返回nil。如果不想再使用该全局变量，可直接将其置为nil。
命令行用法如下：
    lua [options] [lua-script [arguments] ]
    该工具的命令行选项主要有以下3个：
    -e: 可以直接执行命令行中Lua代码，如：lua -e "print(\"Hello World\")"
    -l: 加载该选项后的Lua库文件，如：lua -l mylib -e "x = 10"，该命令在执行之前先将mylib中的Lua代码加载到内存中，在后面的命令中就可以直接使用该文件中定义的Lua函数了。
    -i: 在执行完指定的Lua程序文件之后，并不退出解释器程序，而是直接进入该程序的交互模式。   
    在解释器程序的交互模式下，我们可以通过在表达式前加等号(=)标识符的方式直接输出表达式的执行结果。通过该方式，我们可以将该程序用于计算器，如：
    /> lua
    > = 3 + 1 + 4
    8
    该小节最后需要介绍的是lua脚本的命令行参数访问规则。如：
    /> lua lua-script.lua a b c
    在该脚本的程序入口，lua解释器会将所有命令行参数创建一个名为arg的table。其中脚本名(lua-script.lua)位于table索引的0位置上。它的第一个参数(a)则位于索引1，其它的参数以此类推。这种索引方式和C语言中读取命令行参数的规则相同。但是不同的是，Lua提供了负数索引，用以访问脚本名称之前的命令行参数，如：
    arg[-1] = lua
    arg[0] = lua-script.lua
    arg[1] = a
    arg[2] = b
    arg[3] = c
]]--

--2.Lua表达式和语句--
--[[
取模操作符(%)，Lua中对该操作符的定义为：
    a % b == a - floor(a / b) * b
    由此可以推演出x % 1的结果为x的小数部分，而x - x % 1的结果则为x的整数部分。类似的，x - x % 0.01则是x精确到小数点后两位的结果。
Lua支持的逻辑操作符有：and、or和not。与条件控制语句一样，所有的逻辑操作符都将false和nil视为假，其他的结果均为真。和其他大多数语言一样，Lua中的and和or都使用“短路原则”。在Lua中有一种惯用写法"x = x or v"，它等价于：if not x then x = v end。这里还有一种基于“短路原则”的惯用写法，如：
    max = (x > y) and x or y
    这等价于C语言中max = (x > y) ? x : y。由于x和y均为数值，因此它们的结果将始终为true。
foreach
    for i, v in ipairs(a) do  --ipairs是Lua自带的系统函数，返回遍历数组的迭代器。
        print(v)
	end
    for k in pairs(t) do      --打印table t中的所有key。
        print(k)
    end
]]--

--3.Lua函数--
--[[
Lua为面对对象式的调用也提供了一种特殊的语法--冒号操作符。表达式o.foo(o,x)的另一种写法是o:foo(x)。冒号操作符使调用o.foo时将o隐含的作为函数的第一个参数。
Lua会调整一个函数的返回值数量以适应不同的调用情况。若将函数调用作为一条单独语句时，Lua会丢弃函数的所有返回值。若将函数作为表达式的一部分来调用时，Lua只保留函数的第一个返回值。只有当一个函数调用是一系列表达式中的最后一个元素时，才能获得所有返回值。
Lua中unpack函数，该函数将接收数组作为参数，并从下标1开始返回该数组的所有元素。
函数声明中的(...)表示该函数可以接受不同数量的参数。当这个函数被调用时，所有的参数都被汇聚在一起，函数中访问它时，仍需用3个点(...)。但不同的是，此时这3个点将作为表达式来使用，如{...}表示一个由所有变参构成的数组。在含有变长参数的函数中个，同样可以带有固定参数，但是固定参数一定要在变长参数之前声明.
由于变长参数中可能包含nil值，因此再使用类似获取table元素数量(#)的方式获取变参的数量就会出现问题。如果要想始终获得正确的参数数量，可以使用Lua提供的select函数
 closure(闭合函数)：
	在Lua中函数和所有其它值一样都是匿名的，即它们都没有名称。在使用时都是操作持有该函数的变量
	若将一个函数写在另一个函数之内，那么这个位于内部的函数便可以访问外部函数中的局部变量，Lua每次在给新的闭包变量赋值时，都会让不同的闭包变量拥有独立的"非局部变量"。
]]--

--4.Lua迭代器和泛型for--
--[[
泛型(for)的语法如下：
    for <var-list> in <exp-list> do
        <body>
    end
]]--

--5.Lua编译执行与错误--
--[[
Lua中提供了dofile函数，它是一种内置的操作，用于运行Lua代码块。但实际上dofile只是一个辅助函数，loadfile才是真正的核心函数。相比于dofile，loadfile只是从指定的文件中加载Lua代码块，然后编译这段代码块，如果有编译错误，就返回nil，同时给出错误信息，但是在编译成功后并不真正的执行这段代码块。因此，我们可以将dofile实现为：
	function dofile(filename)
		local f = assert(loadfile(filename))
		return f()
	end
Lua中还提供了另外一种动态执行Lua代码的方式，即loadstring函数。顾名思义，相比于loadfile，loadstring的代码源来自于其参数中的字符串，如：
    f = loadstring("i = i + 1")
    此时f就变成了一个函数，每次调用时就执行"i = i + 1"
对于loadstring，我们还需要注意的是，该函数总是在全局环境中编译它的字符串，因此它将无法文件局部变量，而是只能访问全局变量
Lua将所有独立的程序块视为一个匿名函数的函数体，并且该匿名函数还具有可变长实参，因此在调用loadstring时，可以为其传递参数(...)
事实上，Lua本身也支持动态加载C动态库中的代码，要完成该操作，我们需要借助于Lua内置的系统函数package.loadlib。该函数有两个字符串参数，分别是动态库的全文件名和该库包含的函数名称
在Lua中我们可以通过error()函数获取错误消息,assert函数将检查其第一个参数是否为true，如果是，则简单的返回该参数，否则就引发一个错误。第二个参数是可选字符串。
Lua提供了错误处理函数pcall，该函数的第一个参数为需要“保护执行”的函数，如果该函数执行失败，pcall将返回false及错误信息，否则返回true和函数调用的返回值。
通常在错误发生时，希望得到更多的调试信息，而不是只有发生错误的位置。至少等追溯到发生错误时和函数调用情况，显示一个完整的函数调用栈轨迹。要完成这一功能，我们需要使用Lua提供的另外一个内置函数xpcall。该函数除了接受一个需要被调用的函数之外，还接受第二个参数，即错误处理函数。当发生错误时，Lua会在调用栈展开前调用错误处理函数。这样，我们就可以在这个函数中使用debug库的debug.traceback函数，它会根据调用栈来构建一个扩展的错误消息。
]]--

--6.Lua数据结构--
--[[
在Lua中我们可以将包(Bag)看成MultiSet，与普通集合不同的是该容器中允许key相同的元素在容器中多次出现。下面的代码通过为table中的元素添加计数器的方式来模拟实现该数据结构:
	function insert(bag, element)
		bag[element] = (bag[element] or 0) + 1
	end
	function remove(bag, element)
		local count = bag[element]
		bag[element] = (count and count > 1) and count - 1 or nil
	end
如果想在Lua中将多个字符串连接成为一个大字符串的话，可以通过如下方式实现，如：
	local buff = ""
	for line in io.lines() do
		buff = buff .. line .. "\n"
	end
上面的代码确实可以正常的完成工作，然而当行数较多时，这种方法将会导致大量的内存重新分配和内存间的数据拷贝，由此而带来的性能开销也是相当可观的。事实上，在很多编程语言中String都是不可变对象，如Java，因此如果通过该方式多次连接较大字符串时，均会导致同样的性能问题。为了解决该问题，Java中提供了StringBuilder类，而Lua中则可以利用table的concat方法来解决这一问题，见如下代码：
	local t = {}
	for line in io.lines() do
		t[#t + 1] = line .. "\n"
	end
	local s = table.concat(t)
	--concat方法可以接受两个参数，因此上面的方式还可以改为：
	local t = {}
	for line in io.lines() do
		t[#t + 1] = line
	end
	local s = table.concat(t,"\n")
]]--

--7.Lua数据持久化--
--[[
相信有Java或C#开发经验的人对于这一术语并不陌生。就是将数据对象转换为字节流后在通过IO输出到文件或网络，读取的时候再将这些数据重新构造为与原始对象具有相同值的新对象。或者我们也可以将一段可执行的Lua代码作为序列化后的数据格式。比如：varname = <expr>，这里的<expr>表示计算变量varname的表达式。下面的示例代码用于序列化无环的table：
	function serialize(o)
		if type(o) == "number" then
			io.write(o)
		elseif type(o) == "string" then
			--string.format函数的"%q"参数可以转义字符串中的元字符。
			io.write(string.format("%q",o)) 
		elseif type(o) == "table" then
			io.write("{\n")
			--迭代table中的各个元素，同时递归的写出各个字段的value。
			--由此可以看出，这个简单例子可以支持嵌套的table。
			for k,v in pairs(o) do
				--这样做是为了防止k中包含非法的Lua标识符。
				io.write(" ["); serialize(k); io.write("] = ")
				serialize(v)
				io.write(",\n")
			end
			io.write("}\n")
		else
			error("cannot serialize a " .. type(o))
		end
	end
]]--

--8.Lua元表与元方法--
--[[
Lua中每个值都有一个元表。table和userdata可以有各自独立的元表，而其它数据类型的值则共享其类型所属的单一元表。缺省情况下，table在创建时没有元表，如：
    t = {}
    print(getmetatable(t))  --输出为nil
这里我们可以使用setmetatable函数来设置或修改任何table的元表。
    t1 = {}
    setmetatable(t,t1)
    assert(getmetatable(t) == t1)
任何table都可以作为任何值的元表，而一组相关的table也可以共享一个通用的元表，此元表将描述了它们共同的行为。一个table甚至可以作为它自己的元表，用于描述其特有的行为。在Lua代码中，只能设置table的元表，若要设置其它类型值的元表，则必须通过C代码来完成。
在元表中，每种算术操作符都有对应的字段名，除了__add(加法)和__mul(乘法)外，还有__sub(减法)、__div(除法)、__unm(相反数)、__mod(取模)和__pow(乘幂)。此外，还可以定义__concat字段，用于描述连接操作符的行为。
元表还可以指定关系操作符的含义，元方法分别为__eq(等于)、__lt(小于)和__le(小于等于)，至于另外3个关系操作符，Lua没有提供相关的元方法，可以通过前面3个关系运算符的取反获得。
Lua还提供了一些针对框架的元方法，如print函数总是调用tostring来格式化其输出。如果当前对象存在__tostring元方法时，tostring将用该元方法的返回值作为自己的返回值
函数setmetatable和getmetatable也会用到元表中的一个字段(__metatable)，用于保护元表,一旦设置了__metatable字段，getmetatable就会返回这个字段的值，而setmetatable将引发一个错误。
 当访问table中不存在的字段时，得到的结果为nil。如果我们为该table定义了元方法__index，那个访问的结果将由该方法决定。如果想在访问table时禁用__index元方法，可以通过函数rawget(table,key)完成。通过该方法并不会加速table的访问效率。
 当对一个table中不存在的索引赋值时，解释器就会查找__newindex元方法。如果有就调用它，而不是直接赋值。如果这个元方法指向一个table，Lua将对此table赋值，而不是对原有的table赋值。此外，和__index一样，Lua也同样提供了避开元方法而直接操作当前table的函数rawset(table,key,value)，其功能类似于rawget(table,key)。
 __index和__newindex都是在table中没有所需访问的index时才发挥作用的。因此，为了监控某个table的访问状况，我们可以为其提供一个空table作为代理，之后再将__index和__newindex元方法重定向到原来的table上
]]--

--9.Lua环境--
--[[
 Lua将其所有的全局变量保存在一个常规的table中，这个table被称为“环境”。它被保存在全局变量_G中。
 全局环境存在一个刚性的问题，即它的修改将影响到程序的所有部分。Lua 5为此做了一些改进，新的特征可以支持每个函数拥有自己独立的全局环境，而由该函数创建的closure函数将继承该函数的全局变量表。这里我们可以通过setfenv函数来改变一个函数的环境，该函数接受两个参数，一个是函数名，另一个是新的环境table。第一个参数除了函数名本身，还可以指定为一个数字，以表示当前函数调用栈中的层数。数字1表示当前函数，2表示它的调用函数，以此类推。
]]--

--10.Lua模块与包--
--[[
从Lua 5.1开始，我们可以使用require和module函数来获取和创建Lua中的模块。从使用者的角度来看，一个模块就是一个程序库，可以通过require来加载，之后便得到一个类型为table的全局变量。此时的table就像名字空间一样，可以访问其中的函数和常量
require函数的调用形式为require "模块名"。该调用会返回一个由模块函数组成的table，并且还会定义一个包含该table的全局变量。在使用Lua中的标准库时可以不用显式的调用require，因为Lua已经预先加载了他们。
    require函数在搜素加载模块时，有一套自定义的模式，如：
    ?;?.lua;c:/windows/?;/usr/local/lua/?/?.lua
    在上面的模式中，只有问号(?)和分号(;)是模式字符，分别表示require函数的参数(模块名)和模式间的分隔符。如：调用require "sql"，将会打开以下的文件：
    sql
    sql.lua
    c:/windows/sql
    /usr/local/lua/sql/sql.lua
    Lua将require搜索的模式字符串放在变量package.path中。当Lua启动后，便以环境变量LUA_PATH的值来初始化这个变量。如果没有找到该环境变量，则使用一个编译时定义的默认路径来初始化。如果require无法找到与模块名相符的Lua文件，就会找C程序库。C程序库的搜索模式存放在变量package.cpath中。而这个变量则是通过环境变量LUA_CPATH来初始化的。
编写模块的基本方法：
	--将模块名设置为require的参数，这样今后重命名模块时，只需重命名文件名即可。
	local modname = ...
	local M = {}
	_G[modname] = M
	M.i = {r = 0, i = 1}  --定义一个模块内的常量。
	function M.new(r,i) return {r = r, i = i} end
	function M.add(c1,c2) 
		return M.new(c1.r + c2.r,c1.i + c2.i)
	end
	function M.sub(c1,c2)
		return M.new(c1.r - c2.r,c1.i - c2.i)
	end
	--返回和模块对应的table。
	return M
在Lua 5.1中，我们可以用module(...)函数来代替以下代码，如：
	local modname = ...
	local M = {}
	_G[modname] = M
	package.loaded[modname] = M
		--[[
		和普通Lua程序块一样声明外部函数。
		--]]
	setfenv(1,M)
由于在默认情况下，module不提供外部访问，必须在调用它之前，为需要访问的外部函数或模块声明适当的局部变量。然后Lua提供了一种更为方便的实现方式，即在调用module函数时，多传入一个package.seeall的参数，如：
    module(...,package.seeall)
]]--

--11.Lua面向对象--
--[[
函数在添加一个参数self，他等价于Java/C++中的this.使用冒号(:)，这样可以在定义和调用时均隐藏self参数
类：
	--[[
	在这段代码中，我们可以将Account视为class的声明，如Java中的：
	public class Account 
	{
		public float balance = 0;
		public Account(Account o);
		public void deposite(float f);
	}
	]]--
	--这里balance是一个公有的成员变量。
	Account = {balance = 0}
	--new可以视为构造函数
	function Account:new(o)
		o = o or {} --如果参数中没有提供table，则创建一个空的。
		--将新对象实例的metatable指向Account表(类)，这样就可以将其视为模板了。
		setmetatable(o,self)
		--在将Account的__index字段指向自己，以便新对象在访问Account的函数和字段时，可被直接重定向。
		self.__index = self
		--最后返回构造后的对象实例
		return o
	end
	--deposite被视为Account类的公有成员函数
	function Account:deposit(v)
		--这里的self表示对象实例本身
		self.balance = self.balance + v
	end
	--下面的代码创建两个Account的对象实例
	--通过Account的new方法构造基于该类的示例对象。
	a = Account:new()
	--[[
	这里需要具体解释一下，此时由于table a中并没有deposite字段，因此需要重定向到Account，
	同时调用Account的deposite方法。在Account.deposite方法中，由于self(a对象)并没有balance
	字段，因此在执行self.balance + v时，也需要重定向访问Account中的balance字段，其缺省值为0。
	在得到计算结果后，再将该结果直接赋值给a.balance。此后a对象就拥有了自己的balance字段和值。
	下次再调用该方法，balance字段的值将完全来自于a对象，而无需在重定向到Account了。
	--]]
	a:deposit(100.00)
	print(a.balance) --输出100
	b = Account:new()
	b:deposit(200.00)
	print(b.balance) --输出200
继承：
	--需要说明的是，这段代码仅提供和继承相关的注释，和类相关的注释在上面的代码中已经给出。
	Account = {balance = 0}
	function Account:new(o)
		o = o or {}
		setmetatable(o,self)
		self.__index = self
		return o
	end
	function Account:deposit(v)
		self.balance = self.balance + v
	end
	function Account:withdraw(v)
		if v > self.balance then
			error("Insufficient funds")
		end
		self.balance = self.balance - v
	end
	--下面将派生出一个Account的子类，以使客户可以实现透支的功能。
	SpecialAccount = Account:new()  --此时SpecialAccount仍然为Account的一个对象实例
	--派生类SpecialAccount扩展出的方法。
	--下面这些SpecialAccount中的方法代码(getLimit/withdraw)，一定要位于SpecialAccount被Account构造之后。
	function SpecialAccount:getLimit()
		--此时的self将为对象实例。
		return self.limit or 0
	end
	--SpecialAccount将为Account的子类，下面的方法withdraw可以视为SpecialAccount
	--重写的Account中的withdraw方法，以实现自定义的功能。
	function SpecialAccount:withdraw(v)
		--此时的self将为对象实例。
		if v - self.balance >= self:getLimit() then
			error("Insufficient funds")
		end
		self.balance = self.balance - v
	end
	--在执行下面的new方法时，table s的元表已经是SpecialAccount了，而不再是Account。
	s = SpecialAccount:new{limit = 1000.00}
	--在调用下面的deposit方法时，由于table s和SpecialAccount均未提供该方法，因此访问的仍然是
	--Account的deposit方法。
	s:deposit(100)
	--此时的withdraw方法将不再是Account中的withdraw方法，而是SpecialAccount中的该方法。
	--这是因为Lua先在SpecialAccount(即s的元表)中找到了该方法。
	s:withdraw(200.00)
	print(s.balance) --输出-100
私密性：
	--这里我们需要一个闭包函数作为类的创建工厂
	function newAccount(initialBalance)
		--这里的self仅仅是一个普通的局部变量，其含义完全不同于前面示例中的self。
		--这里之所以使用self作为局部变量名，也是为了方便今后的移植。比如，以后
		--如果改为上面的实现方式，这里应用了self就可以降低修改的工作量了。
		local self = {balance = initialBalance} --这里我们可以将self视为私有成员变量
		local withdraw = function(v) self.balance = self.balance - v end
		local deposit = function(v) self.balance = self.balance + v end
		local getBalance = function() return self.balance end
		--返回对象中包含的字段仅仅为公有方法。事实上，我们通过该种方式，不仅可以实现
		--成员变量的私有性，也可以实现方法的私有性，如：
		--local privateFunction = function() --do something end
		--只要我们不在输出对象中包含该方法的字段即可。
		return {withdraw = withdraw, deposit = deposit, getBalance = getBalance}
	end
	--和前面两个示例不同的是，在调用对象方法时，不再需要self变量，因此我们可以直接使用点(.)，
	--而不再需要使用冒号(:)操作符了。
	accl = newAccount(100.00)
	--在函数newAccount返回之后，该函数内的“非局部变量”表self就不再能被外部访问了，只能通过
	--该函数返回的对象的方法来操作它们。
	accl.withdraw(40.00)
	print(acc1.getBalance())
]]--

--12.Lua弱引用table--
--[[
Lua中的弱引用表提供了3中弱引用模式，即key是弱引用、value是弱引用，以及key和value均是弱引用。不论是哪种类型的弱引用table，只要有一个key或value被回收，那么它们所在的整个条目都会从table中删除。
一个table的弱引用类型是通过其元表的__mode字段来决定的。如果该值为包含字符"k"，那么table就是key弱引用，如果包含"v"，则是value若引用，如果两个字符均存在，就是key/value弱引用。
]]--

--13.Lua字符串库--
--[[
基础字符串函数：
    字符串库中有一些函数非常简单，如：
    1). string.len(s) 返回字符串s的长度；
    2). string.rep(s,n) 返回字符串s重复n次的结果；
    3). string.lower(s) 返回s的副本，其中所有的大写都被转换为了小写形式，其他字符不变；
    4). string.upper(s) 和lower相反，将小写转换为大写；
    5). string.sub(s,i,j) 提取字符串s的第i个到第j个字符。Lua中，第一个字符的索引值为1，最后一个为-1，以此类推，如：
    print(string.sub("[hello world]",2,-2))      --输出hello world
    6). string.format(s,...) 返回格式化后的字符串，其格式化规则等同于C语言中printf函数，如：
    print(string.format("pi = %.4f",math.pi)) --输出pi = 3.1416
    7). string.char(...) 参数为0到多个整数，并将每个整数转换为对应的字符。然后返回一个由这些字符连接而成的字符串，如：
    print(string.char(97,98,99)) --输出abc
    8). string.byte(s,i) 返回字符串s的第i个字符的Ascii值，如果没有第二个参数，缺省返回第一个字符的Ascii值。
    print(string.byte("abc"))      --输出97
    print(string.byte("abc",-1))  --输出99
    由于字符串类型的变量都是不可变类型的变量，因此在所有和string相关的函数中，都无法改变参数中的字符串值，而是生成一个新值返回。
Lua的字符串库提供了一组强大的模式匹配函数，如find、match、gsub和gmatch。
    1). string.find函数：
    在目标字符串中搜索一个模式，如果找到，则返回匹配的起始索引和结束索引，否则返回nil。string.find函数还有一个可选参数，它是一个索引，用于告诉函数从目标字符串的哪个位置开始搜索。主要用于搜索目标字符串中所有匹配的子字符串，且每次搜索都从上一次找到的位置开始。
    2). string.match函数：
    该函数返回目标字符串中和模式字符串匹配的部分
    3). string.gsub函数：
    该函数有3个参数，目标字符串、模式和替换字符串。基本用法是将目标字符串中所有出现模式的地方替换为替换字符串。如：
    print(string.gsub("Lua is cute","cute","great"))  --输出Lua is great
    该函数还有可选的第4个参数，即实际替换的次数。
    print(string.gsub("all lii","l","x",1))  --输出axl lii
    print(string.gsub("all lii","l","x",2))  --输出axx lii
    函数string.gsub还有另一个结果，即实际替换的次数。
    count = select(2, string.gsub(str," "," "))  --输出str中空格的数量
    4). string.gmatch函数：
    返回一个函数，通过这个返回的函数可以遍历到一个字符串中所有出现指定模式的地方。
模式元字符	描述
.			所有字符
%a			字母
%c 			控制字符
%d 			数字
%l 			小写字母
%p 			标点符号
%s 			空白字符
%u 			大写字母
%w 			字母和数字字符
%x 			十六进制数字
%z 			内部表示为0的字符
这些元字符的大写形式表示它们的补集，如%A，表示所有非字母字符。
%表示转义字符，如%.表示点(.)，%%表示百分号(%)。
方括号[]表示将不同的字符分类，即可创建出属于自己的字符分类，如[%w_]表示匹配字符、数字和下划线。
横线(-)表示连接一个范围，比如[0-9A-Z]
如果^字符在方括号内，如[^\n]，表示除\n之外的所有字符，即表示方括号中的分类的补集。如果^不在方括号内，则表示以后面的字符开头，$和它正好相反，表示以前面的字符结束。如：^Hello%d$，匹配的字符串可能为Hello1、Hello2等。
在Lua中还提供了4种用来修饰模式中的重复部分，如：+(重复1次或多次)、*(重复0次或多次)、-(重复0次或多次)和?(出现0或1次)。
星号(*)和横线(-)的主要差别是，星号总是试图匹配更多的字符，而横线则总是试图匹配最少的字符。
捕获功能可根据一个模式从目标字符串中抽出匹配于该模式的内容。在指定捕获是，应将模式中需要捕获的部分写到一对圆括号内。对于具有捕获的模式，函数string.match会将所有捕获到的值作为单独的结果返回。即它会将目标字符串切成多个捕获到的部分。
还可以对模式本身使用捕获。即%1表示第一个捕获，以此类推，%0表示整个匹配
string.gsub函数的第三个参数不仅可以是字符串，也可以是函数或table，如果是函数，string.gsub会在每次找到匹配时调用该函数，调用时的参数就是捕获到的内容，而该函数的返回值则作为要替换的字符串。当用一个table来调用时，string.gsub会用每次捕获到的内容作为key，在table中查找，并将对应的value作为要替换的字符串。如果table中不包含这个key，那么string.gsub不改变这个匹配。
]]--

--14.Lua输入输出库--
--[[
1. 简单模型：
    I/O库会将进程标准输入输出作为其缺省的输入文件和输出文件。我们可以通过io.input(filename)和io.output(filename)这两个函数来改变当前的输入输出文件。
	1). io.write函数：
    函数原型为io.write(...)。该函数将所有参数顺序的写入到当前输出文件中。
	2). io.read函数：
	下表给出了该函数参数的定义和功能描述：
		参数字符串 	含义
		"*all" 		读取整个文件
		"*line" 	读取下一行
		"*number" 	读取一个数字
		<num> 		读取一个不超过<num>个字符的字符串
	如果只是为了迭代文件中的所有行，可以使用io.lines函数，以迭代器的形式访问文件中的每一行数据
	调用io.read("*number")会从当前输入文件中读取一个数字。此时read将直接返回一个数字，而不是字符串。"*number"选项会忽略数字前面所有的空格，并且能处理像-3、+5.2这样的数字格式。如果当前读取的数据不是合法的数字，read返回nil。在调用read是可以指定多个选项，函数会根据每个选项参数返回相应的内容。
	调用io.read(<num>)会从输入文件中最多读取n个字符，如果读不到任何字符，返回nil。否则返回读取到的字符串。
	io.read(0)是一种特殊的情况，用于检查是否到达了文件的末尾。如果没有到达，返回空字符串，否则nil
2. 完整I/O模型：
	 通过io.open函数打开指定的文件，并且在参数中给出对该文件的打开模式，其中"r"表示读取，"w"表示覆盖写入，即先删除文件原有的内容，"a"表示追加式写入，"b"表示以二进制的方式打开文件。在成功打开文件后，该函数将返回表示该文件的句柄，后面所有基于该文件的操作，都需要将该句柄作为参数传入。如果打开失败，返回nil。其中错误信息由该函数的第二个参数返回
	 文件读写函数read/write。这里需要用到冒号语法
	 此外，I/O库还提供了3个预定义的文件句柄，即io.stdin(标准输入)、io.stdout(标准输出)、io.stderr(标准错误输出)。
	 由于一次性读取整个文件比逐行读取要快一些，但对于较大的文件，这样并不可行，因此Lua提供了一种折中的方式，即一次读取指定字节数量的数据，如果当前读取中的最后一行不是完整的一行，可通过该方式将该行的剩余部分也一并读入，从而保证本次读取的数据均为整行数据，以便于上层逻辑的处理。如：
		local lines,rest = f:read(BUFSIZE,"*line") --rest变量包含最后一行中没有读取的部分。
	io.flush函数会将io缓存中的数据刷新到磁盘文件上。io.close函数将关闭当前打开的文件。io.seek函数用于设置或获取当前文件的读写位置，其函数原型为f:seek(whence,offset)，如果whence的值为"set"，offset的值则表示为相对于文件起始位置的偏移量。如为"cur"(默认值),offset则为相对于当前位置的偏移量，如为"end"，则为相对于文件末尾的偏移量。函数的返回值与whence参数无关，总是返回文件的当前位置，即相对于文件起始处的偏移字节数。offset的默认值为0。
]]--

--15.Lua系统库--
--[[
Lua为了保证高度的可移植性，因此，它的标准库仅仅提供了非常少的功能，特别是和OS相关的库。但是Lua还提供了一些扩展库，比如Posix库等。对于文件操作而言，该库仅提供了os.rename函数和os.remove函数。
在Lua中，函数time和date提供了所有的日期和时间功能。
如果不带任何参数调用time函数，它将以数字形式返回当前的日期和时间。如果以一个table作为参数，它将返回一个数字，表示该table中所描述的日期和时间。该table的有效字段如下：
	字段名 	描述
	year 	一个完整的年份
	month 	01-12
	day 	01-31
	hour 	00-23
	min 	00-59
	sec 	00-59
	isdst 	布尔值，true表示夏令时
print(os.time{year = 1970, month = 1, day = 1, hour = 8, min = 0}) --北京是东八区，所以hour等于时表示UTC的0。
print(os.time())  --输出当前时间距离1970-1-1 00:00:00所经过的秒数。输出值为 1333594721
函数date是time的反函数，即可以将time返回的数字值转换为更高级的可读格式，其第一个参数是格式化字符串，表示期望的日期返回格式，第二个参数是日期和时间的数字，缺省为当前日期和时间。
如果格式化字符串为"*t"，函数将返回table形式的日期对象。如果为"!*t"，则表示为UTC时间格式。
date函数的格式化标识和C运行时库中的strftime函数的标识完全相同，见下表：
	关键字 	描述
	%a 		一星期中天数的缩写，如Wed
	%A 		一星期中天数的全称，如Friday
	%b 		月份的缩写，如Sep
	%B 		月份的全称，如September
	%c 		日期和时间
	%d 		一个月中的第几天(01-31)
	%H 		24小时制中的小时数(00-23)
	%I 		12小时制中的小时数(01-12)
	%j 		一年中的第几天(001-366)
	%M 		分钟(00-59)
	%m 		月份(01-12)
	%p 		"上午(am)"或"下午(pm)"
	%S 		秒数(00-59)
	%w 		一星期中的第几天(0--6等价于星期日--星期六)
	%x 		日期，如09/16/2010
	%X 		时间，如23:48:20
	%y 		两位数的年份(00-99)
	%Y 		完整的年份(2012)
	%% 		字符'%'
函数os.clock()返回CPU时间的描述，通常用于计算一段代码的执行效率。
函数os.exit()可中止当前程序的执行。函数os.getenv()可获取一个环境变量的值。如：
    print(os.getenv("PATH"))  --如果环境变量不存在，返回nil。
os.execute函数用于执行和操作系统相关的命令，如：
    os.execute("mkdir " .. "hello")
]]--

--16.Lua-C API简介--
--[[
Lua是一种嵌入式脚本语言，即Lua不是可以单独运行的程序，在实际应用中，主要存在两种应用形式。第一种形式是，C/C++作为主程序，调用Lua代码，此时可以将Lua看做“可扩展的语言”，我们将这种应用称为“应用程序代码”。第二种形式是Lua具有控制权，而C/C++代码则作为Lua的“库代码”。在这两种形式中，都是通过Lua提供的C API完成两种语言之间的通信的。
1. 基础知识：
    C API是一组能使C/C++代码与Lua交互的函数。其中包括读写Lua全局变量、调用Lua函数、运行一段Lua代码，以及注册C函数以供Lua代码调用等。这里先给出一个简单的示例代码：
	#include <stdio.h>
	#include <string.h>
	#include <lua.hpp>
	#include <lauxlib.h>
	#include <lualib.h>
	int main(void)
	{
		const char* buff = "print(\"hello\")";
		int error;
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		error = luaL_loadbuffer(L,buff,strlen(buff),"line") || lua_pcall(L,0,0,0);
		int s = lua_gettop(L);
		if (error) {
			fprintf(stderr,"%s",lua_tostring(L,-1));
			lua_pop(L,1);
		}
		lua_close(L);
		return 0;
	}
    下面是针对以上代码给出的具体解释：
    1). 上面的代码是基于我的C++工程，而非C工程，因此包含的头文件是lua.hpp，如果是C工程，可以直接包含lua.h。
    2). Lua库中没有定义任何全局变量，而是将所有的状态都保存在动态结构lua_State中，后面所有的C API都需要该指针作为第一个参数。
    3). luaL_openlibs函数是用于打开Lua中的所有标准库，如io库、string库等。
    4). luaL_loadbuffer编译了buff中的Lua代码，如果没有错误，则返回0，同时将编译后的程序块压入虚拟栈中。
    5). lua_pcall函数会将程序块从栈中弹出，并在保护模式下运行该程序块。执行成功返回0，否则将错误信息压入栈中。
    6). lua_tostring函数中的-1，表示栈顶的索引值，栈底的索引值为1，以此类推。该函数将返回栈顶的错误信息，但是不会将其从栈中弹出。
    7). lua_pop是一个宏，用于从虚拟栈中弹出指定数量的元素，这里的1表示仅弹出栈顶的元素。
    8). lua_close用于释放状态指针所引用的资源。
2. 栈：
    在Lua和C语言之间进行数据交换时，由于两种语言之间有着较大的差异，比如Lua是动态类型，C语言是静态类型，Lua是自动内存管理，而C语言则是手动内存管理。为了解决这些问题，Lua的设计者使用了虚拟栈作为二者之间数据交互的介质。在C/C++程序中，如果要获取Lua的值，只需调用Lua的C API函数，Lua就会将指定的值压入栈中。要将一个值传给Lua时，需要先将该值压入栈，然后调用Lua的C API，Lua就会获取该值并将其从栈中弹出。为了可以将不同类型的值压入栈，以及从栈中取出不同类型的值，Lua为每种类型均设定了一个特定函数。
    1). 压入元素：
    Lua针对每种C类型，都有一个C API函数与之对应，如：
    void lua_pushnil(lua_State* L);  --nil值
    void lua_pushboolean(lua_State* L, int b); --布尔值
    void lua_pushnumber(lua_State* L, lua_Number n); --浮点数
    void lua_pushinteger(lua_State* L, lua_Integer n);  --整型
    void lua_pushlstring(lua_State* L, const char* s, size_t len); --指定长度的内存数据
    void lua_pushstring(lua_State* L, const char* s);  --以零结尾的字符串，其长度可由strlen得出。
    对于字符串数据，Lua不会持有他们的指针，而是调用在API时生成一个内部副本，因此，即使在这些函数返回后立刻释放或修改这些字符串指针，也不会有任何问题。
    在向栈中压入数据时，可以通过调用下面的函数判断是否有足够的栈空间可用，一般而言，Lua会预留20个槽位，对于普通应用来说已经足够了，除非是遇到有很多参数的函数。
    int lua_checkstack(lua_State* L, int extra) --期望得到extra数量的空闲槽位，如果不能扩展并获得，返回false。
    2). 查询元素：
    API使用“索引”来引用栈中的元素，第一个压入栈的为1，第二个为2，依此类推。我们也可以使用负数作为索引值，其中-1表示为栈顶元素，-2为栈顶下面的元素，同样依此类推。
    Lua提供了一组特定的函数用于检查返回元素的类型，如：
    int lua_isboolean (lua_State *L, int index);
    int lua_iscfunction (lua_State *L, int index);
    int lua_isfunction (lua_State *L, int index);
    int lua_isnil (lua_State *L, int index);
    int lua_islightuserdata (lua_State *L, int index);
    int lua_isnumber (lua_State *L, int index);
    int lua_isstring (lua_State *L, int index);
    int lua_istable (lua_State *L, int index);
    int lua_isuserdata (lua_State *L, int index);
    以上函数，成功返回1，否则返回0。需要特别指出的是，对于lua_isnumber而言，不会检查值是否为数字类型，而是检查值是否能转换为数字类型。
    Lua还提供了一个函数lua_type，用于获取元素的类型，函数原型如下：
    int lua_type (lua_State *L, int index);
    该函数的返回值为一组常量值，分别是：LUA_TNIL、LUA_TNUMBER、LUA_TBOOLEAN、LUA_TSTRING、LUA_TTABLE、LUA_TFUNCTION、LUA_TUSERDATA、LUA_TTHREAD和LUA_TLIGHTUSERDATA。这些常量通常用于switch语句中。
    除了上述函数之外，Lua还提供了一组转换函数，如：
    int lua_toboolean (lua_State *L, int index);
    lua_CFunction lua_tocfunction (lua_State *L, int index);
    lua_Integer lua_tointeger (lua_State *L, int index);    
    const char *lua_tolstring (lua_State *L, int index, size_t *len);
    lua_Number lua_tonumber (lua_State *L, int index);
    const void *lua_topointer (lua_State *L, int index);
    const char *lua_tostring (lua_State *L, int index);
    void *lua_touserdata (lua_State *L, int index);
    --string类型返回字符串长度，table类型返回操作符'#'等同的结果，userdata类型返回分配的内存块长度。
    size_t lua_objlen (lua_State *L, int index);　
    对于上述函数，如果调用失败，lua_toboolean、lua_tonumber、lua_tointeger和lua_objlen均返回0，而其他函数则返回NULL。在很多时候0不是一个很有效的用于判断错误的值，但是ANSI C没有提供其他可以表示错误的值。因此对于这些函数，在有些情况下需要先使用lua_is*系列函数判断是否类型正确，而对于剩下的函数，则可以直接通过判断返回值是否为NULL即可。
    对于lua_tolstring函数返回的指向内部字符串的指针，在该索引指向的元素被弹出之后，将无法保证仍然有效。该函数返回的字符串末尾均会有一个尾部0。
    下面将给出一个工具函数，可用于演示上面提到的部分函数，如：
	static void stackDump(lua_State* L) 
	{
		int top = lua_gettop(L);
		for (int i = 1; i <= top; ++i) {
			int t = lua_type(L,i);
			switch(t) {
			case LUA_TSTRING:
				printf("'%s'",lua_tostring(L,i));
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L,i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%g",lua_tonumber(L,i));
				break;
			default:
				printf("%s",lua_typename(L,t));
				break;
			}
			printf("");
		}
		printf("\n");
	}
	3). 其它栈操作函数：
	除了上面给出的数据交换函数之外，Lua的C API还提供了一组用于操作虚拟栈的普通函数，如：
	int lua_gettop(lua_State* L); --返回栈中元素的个数。
	void lua_settop(lua_State* L, int index); --将栈顶设置为指定的索引值。
	void lua_pushvalue(lua_State* L, int index); --将指定索引的元素副本压入栈。
	void lua_remove(lua_State* L, int index); --删除指定索引上的元素，其上面的元素自动下移。
	void lua_insert(lua_State* L, int index); --将栈顶元素插入到该索引值指向的位置。
	void lua_replace(lua_State* L, int index); --弹出栈顶元素，并将该值设置到指定索引上。
	Lua还提供了一个宏用于弹出指定数量的元素：#define lua_pop(L,n)  lua_settop(L, -(n) - 1)    
	见如下示例代码：
	int main()
	{
		lua_State* L = luaL_newstate();
		lua_pushboolean(L,1);
		lua_pushnumber(L,10);
		lua_pushnil(L);
		lua_pushstring(L,"hello");
		stackDump(L); //true 10 nil 'hello'
		lua_pushvalue(L,-4);
		stackDump(L); //true 10 nil 'hello' true
		lua_replace(L,3);
		stackDump(L); //true 10 true 'hello'
		lua_settop(L,6);
		stackDump(L); //true 10 true 'hello' nil nil
		lua_remove(L,-3);
		stackDump(L); //true 10 true nil nil
		lua_settop(L,-5);
		stackDump(L); //true
		lua_close(L);
		return 0;
	}
3. C API中的错误处理：
    1). C程序调用Lua代码的错误处理：
    通常情况下，应用程序代码是以“无保护”模式运行的。因此，当Lua发现“内存不足”这类错误时，只能通过调用“紧急”函数来通知C语言程序，之后在结束应用程序。用户可通过lua_atpanic来设置自己的“紧急”函数。如果希望应用程序代码在发生Lua错误时不会退出，可通过调用lua_pcall函数以保护模式运行Lua代码。这样再发生内存错误时，lua_pcall会返回一个错误代码，并将解释器重置为一致的状态。如果要保护与Lua的C代码，可以使用lua_cpall函数，它将接受一个C函数作为参数，然后调用这个C函数。
    2). Lua调用C程序：
    通常而言，当一个被Lua调用的C函数检测到错误时，它就应该调用lua_error，该函数会清理Lua中所有需要清理的资源，然后跳转回发起执行的那个lua_pcall，并附上一条错误信息。
]]--

--17.C调用Lua--
--[[
lua_getglobal是宏，其原型为：#define lua_getglobal(L,s)  lua_getfield(L, LUA_GLOBALSINDEX, (s))。每次调用这个宏的时候，都会将Lua代码中与之相应的全局变量值压入栈中
void lua_getfield(lua_State *L, int idx, const char *k); 第二个参数是table变量在栈中的索引值，最后一个参数是table的键值，该函数执行成功后会将字段值压入栈中。
void lua_setfield(lua_State *L, int idx, const char *k); 第二个参数是table变量在栈中的索引值，最后一个参数是table的键名称，而字段值是通过上一条命令lua_pushnumber(L,[number])压入到栈中的，该函数在执行成功后会将刚刚压入的字段值弹出栈。
lua_newtable是宏，其原型为：#define lua_newtable(L) lua_createtable(L, 0, 0)。调用该宏后，Lua会生成一个新的table对象并将其压入栈中。
lua_setglobal是宏，其原型为：#define lua_setglobal(L,s) lua_setfield(L,LUA_GLOBALSINDEX,(s))。调用该宏后，Lua会将当前栈顶的值赋值给第二个参数指定的全局变量名。该宏在执行成功后，会将刚刚赋值的值从栈顶弹出。
luaL_dostring 等同于luaL_loadstring() || lua_pcall()
]]--

--18.Lua调用C函数--
--[[
Lua可以调用C函数的能力将极大的提高Lua的可扩展性和可用性。对于有些和操作系统相关的功能，或者是对效率要求较高的模块，我们完全可以通过C函数来实现，之后再通过Lua调用指定的C函数。对于那些可被Lua调用的C函数而言，其接口必须遵循Lua要求的形式，即typedef int (*lua_CFunction)(lua_State* L)。简单说明一下，该函数类型仅仅包含一个表示Lua环境的指针作为其唯一的参数，实现者可以通过该指针进一步获取Lua代码中实际传入的参数。返回值是整型，表示该C函数将返回给Lua代码的返回值数量，如果没有返回值，则return 0即可。需要说明的是，C函数无法直接将真正的返回值返回给Lua代码，而是通过虚拟栈来传递Lua代码和C函数之间的调用参数和返回值的。
lua_register把C函数注册到Lua中
typedef struct luaL_Reg{
	const char *name;
	lua_CFunction func;
}luaL_Reg;
]]--

--19.编写C函数的技巧--
--[[
在Lua中，“数组”只是table的一个别名，是指以一种特殊的方法来使用table。出于性能原因，Lua的C API为数组操作提供了专门的函数，如：
    void lua_rawgeti(lua_State* L, int index, int key);
    void lua_rawseti(lua_State* L, int index, int key);
以上两个函数分别用于读取和设置数组中的元素值。其中index参数表示待操作的table在栈中的位置，key表示元素在table中的索引值。由于这两个函数均为原始操作，比涉及元表的table访问更快。通常而言，作为数组使用的table很少会用到元表。
当一个C函数从Lua收到一个字符串参数时，必须遵守两条规则：不要在访问字符串时从栈中将其弹出，不要修改字符串。在Lua的C API中主要提供了两个操作Lua字符串的函数，即：
    void  lua_pushlstring(lua_State *L, const char *s, size_t l);
    const char* lua_pushfstring(lua_State* L, const char* fmt, ...);
第一个API用于截取指定长度的子字符串，同时将其压入栈中。而第二个API则类似于C库中的sprintf函数，并将格式化后的字符串压入栈中。和sprintf的格式说明符不同的是，该函数只支持%%(表示字符%)、%s(表示字符串)、%d(表示整数)、%f(表示Lua中的number)及%c(表示字符)。除此之外，不支持任何例如宽度和精度的选项。
Lua API中提供了lua_concat函数，其功能类似于Lua中的".."操作符，用于连接(并弹出)栈顶的n个值，然后压入连接后的结果。其原型为：
    void  lua_concat(lua_State *L, int n);
参数n表示栈中待连接的字符串数量。该函数会调用元方法。然而需要说明的是，如果连接的字符串数量较少，该函数可以很好的工作，反之，则会带来性能问题。为此，Lua API提供了另外一组函数专门解决由此而带来的性能问题，
	#include <stdio.h>
	#include <string.h>
	#include <lua.hpp>
	#include <lauxlib.h>
	#include <lualib.h>
	extern "C" int strUpperFunc(lua_State* L)
	{
		size_t len;
		luaL_Buffer b;
		//检查参数第一个参数是否为字符串，同时返回字符串的指针及长度。
		const char* s = luaL_checklstring(L,1,&len);
		//初始化Lua的内部Buffer。
		luaL_buffinit(L,&b);
		//将处理后的字符依次(luaL_addchar)追加到Lua的内部Buffer中。
		for (int i = 0; i < len; ++i)
			luaL_addchar(&b,toupper(s[i]));
		//将该Buffer及其内容压入栈中。
		luaL_pushresult(&b);
		return 1;
	}
使用缓冲机制的第一步是声明一个luaL_Buffer变量，并用luaL_buffinit来初始化它。初始化后，就可通过luaL_addchar将一个字符放入缓冲。除该函数之外，Lua的辅助库还提供了直接添加字符串的函数，如：
    void luaL_addlstring(luaL_Buffer* b, const char* s, size_t len);
    void luaL_addstring(luaL_Buffer* b, const char* s);
最后luaL_pushresult会更新缓冲，并将最终的字符串留在栈顶。通过这些函数，就无须再关心缓冲的分配了。但是在追加的过程中，缓冲会将一些中间结果放到栈中。因此，在使用时要留意此细节，只要保证压入和弹出的次数相等既可。Lua API还提供一个比较常用的函数，用于将栈顶的字符串或数字也追加到缓冲区中，函数原型为：
    void luaL_addvalue(luaL_Buffer* b);
注册表是一个全局的table，只能被C代码访问。通常用于保存多个模块间的共享数据。我们可以通过LUA_REGISTRYINDEX索引值来访问注册表。
如果需要保存一个模块的私有数据，即模块内各函数需要共享的数据，应该使用环境。我们可以通过LUA_ENVIRONINDEX索引值来访问环境。
upvalue是和特定函数关联的，我们可以将其简单的理解为函数内的静态变量。
]]--

--20.userdata--
--[[
事实上，轻量级userdata仅仅表示的是C指针的值，即(void*)。由于它只是一个值，所以不用创建。如果需要将一个轻量级userdata放入栈中，调用lua_pushlightuserdata即可。full userdata和light userdata之间最大的区别来自于相等性判断，对于一个full userdata，它只是与自身相等，而light userdata则表示为一个C指针，因此，它与所有表示同一指针的light userdata相等。再有就是light userdata不会受到垃圾收集器的管理，使用时就像一个普通的整型数字一样。
]]--